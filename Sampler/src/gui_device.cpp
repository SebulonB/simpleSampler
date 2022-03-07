#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <String.h>
#include <vector>
#include <functional>

#include "widgets/widgets.h"
#include "widgets/widget_midi_index_list.h"
#include "gui_device.h"
#include "audio_engine/audio_device.h"


#define PARAM_VALUE_POS 53
#define PARAM_VALUE_POS_BROWSER 40



//------ guiDeviceSampler -------
guiDeviceSampler::guiDeviceSampler( Adafruit_SSD1306 *disp, 
                                    audioEngine * engine,
                                    enum audioEngine::AUDIO_ENGINE_DEVICES i_engine_sampler,
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

  //-------------------{ Sampler Main }-----------------------

  widgetParamFloat *param_f;

  //volume
  param_f = new widgetParamFloat( m_display, l_device, 
                                  F("Volume"), F("Main_Volume"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(100);
  param_f->setValueDefault(100);
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setVolume,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice( i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );
  m_param_list->pushWidget(param_f);


  // //attack
  // param_f = new widgetParamFloat( m_display, l_device, 
  //                                 F("Attack"), F("SRC_Attack"), 
  //                                 m_param_y_pos, widgetParamFloat::SECONDS );
  // param_f->setMax(9999);
  // param_f->setValueDefault(0);
  // param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setAttack,
  //           reinterpret_cast<audioDeviceSampler *>(engine->getDevice( i_engine_sampler )),
  //           std::placeholders::_1));

  // m_params.push_back( param_f );  
  // m_param_list->pushWidget(param_f);


  // //hold
  // param_f = new widgetParamFloat( m_display, l_device,
  //                                 F("Hold"), F("SRC_Hold"), 
  //                                 m_param_y_pos, widgetParamFloat::SECONDS );
  // param_f->setMax(9999);
  // param_f->setValueDefault(100);  
  // param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setHold,
  //           reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
  //           std::placeholders::_1));

  // m_params.push_back( param_f );  
  // m_param_list->pushWidget(param_f); 


  // //decay
  // param_f = new widgetParamFloat( m_display, l_device,
  //                                 F("Decay"), F("SRC_Decay"), 
  //                                 m_param_y_pos, widgetParamFloat::SECONDS );
  // param_f->setMax(9999);
  // param_f->setValueDefault(100);  
  // param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setDecay,
  //           reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
  //           std::placeholders::_1));

  // m_params.push_back( param_f );  
  // m_param_list->pushWidget(param_f); 


  // //sustain
  // param_f = new widgetParamFloat( m_display, l_device,
  //                                 F("Sustain"), F("SRC_Sustain"), 
  //                                 m_param_y_pos, widgetParamFloat::PERCENT );
  // param_f->setMax(100);
  // param_f->setValueDefault(80);  
  // param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setSustain,
  //           reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
  //           std::placeholders::_1));

  // m_params.push_back( param_f );  
  // m_param_list->pushWidget(param_f); 

  // //sustain
  // param_f = new widgetParamFloat( m_display, l_device,
  //                                 F("Release"), F("SRC_Release"), 
  //                                 m_param_y_pos, widgetParamFloat::SECONDS );
  // param_f->setMax(9999);
  // param_f->setValueDefault(250);  
  // param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setRelease,
  //           reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
  //           std::placeholders::_1));

  // m_params.push_back( param_f );  
  // m_param_list->pushWidget(param_f); 

  //start
  param_f = new widgetParamFloat( m_display, l_device, 
                                  F("Start"), F("SRC_Start"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(95);
  param_f->setValueDefault(0);
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setStart,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice( i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f);


  // //length
  // param_f = new widgetParamFloat( m_display, l_device,
  //                                 F("Length"), F("SRC_Length"), 
  //                                 m_param_y_pos, widgetParamFloat::PERCENT );
  // param_f->setMax(100);
  // param_f->setValueDefault(100);  
  // param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setLength,
  //           reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
  //           std::placeholders::_1));

  // m_params.push_back( param_f );  
  //m_param_list->pushWidget(param_f); 


  //-------------------{ Player Param }-----------------------
  //Pitch
  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Pitch"), F("SRC_Pitch"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(80.0);
  param_f->setMin(-80.0);  
  param_f->setValueDefault(0);  
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setPitch,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f); 



  //-------------------{ Channel Vol }-----------------------
  //CH 1
  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Vol_ch1"), F("SRC_Vol_ch1"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(100);
  param_f->setValueDefault(100);  
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setVolCH1,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f); 

  //CH 2
  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Vol_ch2"), F("SRC_Vol_ch2"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(100);
  param_f->setValueDefault(100);  
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setVolCH2,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f); 





  //-------------------{ Browser }-----------------------

  widgetParamBrowser *param_b = new widgetParamBrowser( m_display, l_device,
                                                        F("Sample"), F("SRC_SampleName"), 
                                                        PARAM_VALUE_POS_BROWSER);

  param_b->setDir(F("/samples"));
  param_b->setUpdateCallback(std::bind(&audioDeviceSampler::openSample,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice( i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_b  );  
  m_param_list->pushWidget( param_b );   



  //-------------------{ Midi }-----------------------

  //CH
  widgetParamInt *param_i = new widgetParamInt( m_display, l_device,
                                                F("Midi CH"), F("SRC_MidiCH"), 
                                                m_param_y_pos );

  param_i->setMax(16);
  param_i->setMin(1);
  param_i->setValueDefault((i_engine_sampler+1));

  param_i->setUpdateCallback(std::bind(&audioDeviceSampler::setMidiCh,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_i  );  
  m_param_list->pushWidget( param_i );   

  //note min 
  widgetParamList *param_l = new widgetParamList( m_display, l_device,
                                                  F("Note Min"), F("SRC_MidiNoteMin"), 
                                                  m_param_y_pos, midi_index_list_with_number, MIDI_INDEX_LIST_NUMBER_LEN );

  param_l->setValueDefault(0);

  param_l->setUpdateCallback(std::bind(&audioDeviceSampler::setNoteMin,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_l );  
  m_param_list->pushWidget( param_l );     

  //note max
  param_l =                new widgetParamList( m_display, l_device,
                                                F("Note Max"), F("SRC_MidiNoteMax"), 
                                                m_param_y_pos, midi_index_list_with_number, MIDI_INDEX_LIST_NUMBER_LEN );

  param_l->setValueDefault(0);

  param_l->setUpdateCallback(std::bind(&audioDeviceSampler::setNoteMin,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_l );  
  m_param_list->pushWidget( param_l );     


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

      //udate guie
      p->triggerCallback();
    }

    else if (pa->isParamType(widgetParam::INT)){
      widgetParamInt *p = reinterpret_cast<widgetParamInt *>(pa); 
      float val;
      if(!m_handler->getParamValue(p->getLDevice(), p->getLParam(), val)){
        p->useDefaultVal();
      }
      else{
        p->setValue((int)val);
      }

      //udate guie
      p->triggerCallback();
    }

    else if (pa->isParamType(widgetParam::LIST)){
      widgetParamList *p = reinterpret_cast<widgetParamList *>(pa); 
      float val;
      if(!m_handler->getParamValue(p->getLDevice(), p->getLParam(), val)){
        p->useDefaultVal();
      }
      else{
        p->setValue((int)val);
      }

      //udate guie
      p->triggerCallback();
    }    

    else if(pa->isParamType(widgetParam::BROWSER)){
      widgetParamBrowser * p = reinterpret_cast<widgetParamBrowser *>(pa); 
      String val;
      if(m_handler->getParamValue(p->getLDevice(), p->getLParam(), val)){
        p->setValue(val);
      }

      p->triggerCallback();
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

    else if (pa->isParamType(widgetParam::INT)){
      widgetParamInt *p = reinterpret_cast<widgetParamInt *>(pa); 
      m_handler->saveParamValue(p->getLDevice(), p->getLParam(), (float)p->getValue());
    }

    else if (pa->isParamType(widgetParam::LIST)){
      widgetParamList *p = reinterpret_cast<widgetParamList *>(pa); 
      m_handler->saveParamValue(p->getLDevice(), p->getLParam(), (float)p->getValue());
    }

    else if(pa->isParamType(widgetParam::BROWSER)){
      widgetParamBrowser * p = reinterpret_cast<widgetParamBrowser *>(pa); 
      String val = p->getValue();
      m_handler->saveParamValue(p->getLDevice(), p->getLParam(), val);
    }
  }  
}
