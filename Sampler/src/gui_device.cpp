#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <vector>

#include "widgets.h"
#include "gui_device.h"


#define PARAM_VALUE_POS 55





//------ guiDeviceSampler -------
guiDeviceSampler::guiDeviceSampler( Adafruit_SSD1306 *disp, 
                      const uint16_t x, const uint16_t y, 
                      const uint16_t width, const uint16_t height) : guiDevice()
{
  m_display = disp;

  m_pos_x = x;
  m_pos_y = y;

  m_width  = width;
  m_height = height;

  m_param_y_pos = PARAM_VALUE_POS;

  m_widget_type  = widget::DEVICE;      


#ifdef DEBUG_GUI_DEVICE_SAMPLER
  sprintf(str_, "create guiSampler: x(%d), y(%d), w(%d), h(%d)\n",
    m_pos_x, m_pos_y, m_width, m_height );
  Serial.print(str_);
#endif  

  //param list
  m_param_list = new widgetList(m_display, m_pos_x, m_pos_y, m_width, m_height);

  //------------------ | Params | ---------------------- 
  widgetParamFloat *param_f;

  param_f = new widgetParamFloat(m_display, F("Volume"), m_param_y_pos, widgetParamFloat::PERCENT);
  param_f->setMax(100);
  m_param_list->pushWidget(param_f);

  param_f = new widgetParamFloat(m_display, F("Start"), m_param_y_pos, widgetParamFloat::PERCENT);
  param_f->setMax(100);
  m_param_list->pushWidget(param_f);  

  param_f = new widgetParamFloat(m_display, F("Attack"), m_param_y_pos, widgetParamFloat::SECONDS);
  param_f->setMax(9000);
  m_param_list->pushWidget(param_f);

  param_f = new widgetParamFloat(m_display, F("Decay"), m_param_y_pos, widgetParamFloat::SECONDS);
  param_f->setMax(9000);
  m_param_list->pushWidget(param_f);

  param_f = new widgetParamFloat(m_display, F("Sustain"), m_param_y_pos, widgetParamFloat::SECONDS);
  param_f->setMax(9000);
  m_param_list->pushWidget(param_f);

  param_f = new widgetParamFloat(m_display, F("Relase"), m_param_y_pos, widgetParamFloat::SECONDS);
  param_f->setMax(9000);
  m_param_list->pushWidget(param_f);

  m_param_list->setIndex(0);

}








//------ guiDevice -------
void guiDevice::inc_rotary(bool inc)
{

#ifdef DEBUG_GUI_DEVICE_SAMPLER
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


void guiDevice::set_edit()
{

#ifdef DEBUG_GUI_DEVICE_SAMPLER
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

#ifdef DEBUG_GUI_DEVICE_SAMPLER
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
