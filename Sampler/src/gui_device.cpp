#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <vector>

#include "widgets.h"
#include "gui_device.h"


#define PARAM_VALUE_POS 55
#define PARAM_VALUE_POS_BROWSER 45



//------ guiDeviceSampler -------
guiDeviceSampler::guiDeviceSampler( Adafruit_SSD1306 *disp, 
                                    const __FlashStringHelper *device, 
                                    const uint16_t x, const uint16_t y, 
                                    const uint16_t width, const uint16_t height) : guiDevice()
{
  m_display = disp;

  m_pos_x = x;
  m_pos_y = y;

  m_width  = width;
  m_height = height;

  m_id = 0;

  l_device = reinterpret_cast<const char *>(device); 

  m_param_y_pos = PARAM_VALUE_POS;
  m_widget_type  = widget::DEVICE;      


#if defined(DEBUG_GUI_DEVICE_SAMPLER) && defined(DEBUG_GUI_DEVICE)
  sprintf(str_, "create guiSampler: x(%d), y(%d), w(%d), h(%d)\n",
    m_pos_x, m_pos_y, m_width, m_height );
  Serial.print(str_);
#endif  

  //param list
  m_param_list = new widgetList(m_display, m_pos_x, m_pos_y, m_width, m_height);

  //------------------ | Params | ---------------------- 
  widgetParamFloat *param_f;

  param_f = new widgetParamFloat( m_display, l_device, 
                                  F("Volume"), F("Main_Volume"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(100);
  param_f->setValueDefault(100);
  m_params.push_back( param_f );
  m_param_list->pushWidget(param_f);

  param_f = new widgetParamFloat( m_display, l_device, 
                                  F("Start"), F("SRC_Start"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(100);
  param_f->setValueDefault(0);
  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f);


  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Length"), F("SRC_Length"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(100);
  param_f->setValueDefault(100);  
  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f); 


  widgetParamBrowser *param_b = new widgetParamBrowser( m_display, l_device,
                                                        F("Sample"), F("SRC_SampleName"), 
                                                        PARAM_VALUE_POS_BROWSER);

  param_b->setDir(F("/samples"));
  m_params.push_back( param_b  );  
  m_param_list->pushWidget( param_b );   

  //push to gui list
  m_param_list->setIndex(0);

}








//------ guiDevice -------
void guiDevice::inc_rotary(bool inc)
{

#if defined(DEBUG_GUI_DEVICE_SAMPLER) && defined(DEBUG_GUI_DEVICE)
  sprintf(str_, "gui Sampler inc: m_edit(%s), inc(%s)\n",
    (m_edit ? "true" : "false"),  (inc ? "true" : "false") );
  Serial.print(str_);
#endif  

  if(m_edit){
    widgetParam* w = reinterpret_cast<widgetParam* >(m_param_list->getActiveWidget());
    if(w==NULL){return;}
    if(!w->isWidgetType(widget::PARAM)){return;}   

    w->incValue(inc);
  }
  else{
    m_param_list->incIndex(inc);
  }
}

void guiDevice::rstIndex()
{
  m_param_list->setIndex(0);
}


void guiDevice::set_edit()
{

#if defined(DEBUG_GUI_DEVICE_SAMPLER) && defined(DEBUG_GUI_DEVICE)
  sprintf(str_, "gui Sampler set: m_edit(%s)\n", (m_edit ? "true" : "false") );
  Serial.print(str_);
#endif  


  widgetParam* w = reinterpret_cast<widgetParam* >(m_param_list->getActiveWidget());
  if(w==NULL){return;}
  if(!w->isWidgetType(widget::PARAM)){return;}

  w->setEdit(m_edit);
}


void guiDevice::draw()
{

#if defined(DEBUG_GUI_DEVICE_SAMPLER) && defined(DEBUG_GUI_DEVICE)
  sprintf(str_, "gui Sampler force draw: m_active(%s) m_edit(%s)\n", 
   (m_active ? "true" : "false"), (m_edit ? "true" : "false") );
  Serial.print(str_);
#endif  

  m_param_list->setActive(m_active);

  if(m_edit){
    widget* w = m_param_list->getActiveWidget();
    if(w==NULL){return;}  
    w->forceDraw();  
  }
  else{
    m_param_list->forceDraw();
  }
  
}


void guiDevice::read_handler(patchHandler *m_handler)
{

#ifdef  DEBUG_GUI_DEVICE
  sprintf(str_, "gui device (%s): readHandler, m_param.size(%d)\n", l_device, m_params.size());
  Serial.print(str_);
#endif  

  for(auto pa : m_params){

    if       (pa->isParamType(widgetParam::FLOAT)){
      widgetParamFloat *p = reinterpret_cast<widgetParamFloat *>(pa); 
      float val;
      if(!m_handler->getParamValue(p->getLDevice(), p->getLParam(), val)){
        p->useDefaultVal();
      }
      else{
        p->setValue(val);
      }
    }

    else if(pa->isParamType(widgetParam::BROWSER)){
      widgetParamBrowser * p = reinterpret_cast<widgetParamBrowser *>(pa); 
      String val;
      if(m_handler->getParamValue(p->getLDevice(), p->getLParam(), val)){
        p->setValue(val);
      }
    }
  }
}

void guiDevice::save_handler(patchHandler *m_handler)
{

#ifdef  DEBUG_GUI_DEVICE
  sprintf(str_, "gui device (%s): saveHandler\n", l_device);
  Serial.print(str_);
#endif  

  for(auto pa: m_params){
    if      (pa->isParamType(widgetParam::FLOAT)){
      widgetParamFloat *p = reinterpret_cast<widgetParamFloat *>(pa); 
      m_handler->saveParamValue(p->getLDevice(), p->getLParam(), p->getValue());
    }
    else if(pa->isParamType(widgetParam::BROWSER)){
      widgetParamBrowser * p = reinterpret_cast<widgetParamBrowser *>(pa); 
      String val = p->getValue();
      m_handler->saveParamValue(p->getLDevice(), p->getLParam(), val);
    }
  }  
}
