#ifndef GUI_DEVICE_H_
#define GUI_DEVICE_H_
#include "widgets.h"
#include <vector>
#include "handler.h"

#include "audio_engine/audio_engine.h"


//#define DEBUG_GUI_DEVICE_SAMPLER
//#define DEBUG_GUI_DEVICE



class guiDevice : public widget
{
  public:
    void incRotary(bool inc){
      inc_rotary(inc);
    }
    void setEdit(){
      m_edit = !m_edit;
      set_edit();
    }
    void setEdit(bool en){
      m_edit = en;
      set_edit();
    }
    void rstIndex();
    uint16_t getIndex(){
      if(m_param_list == NULL){return 0;}
      return m_param_list->getIndex();
    };
    void setIndex(uint16_t m){
      if(m_param_list == NULL){return;}
      m_param_list->setIndex(m);
    }

    void readHandler(patchHandler *m_handler){read_handler(m_handler);}
    void saveHandler(patchHandler *m_handler){save_handler(m_handler);}

  protected:
    uint16_t m_id{0};
    const char * l_device{NULL};

    bool m_edit{false};
    u_int16_t m_param_y_pos{0};

    widgetList *m_param_list{NULL};
    std::vector<widgetParam *> m_params{NULL}; 
    
    void inc_rotary(bool inc);
    void set_edit();
    void draw();

    void read_handler(patchHandler *m_handler);
    void save_handler(patchHandler *m_handler);

#ifdef DEBUG_GUI_DEVICE
    char str_[100];
#endif    

};







//------ guiDeviceSampler -------
class guiDeviceSampler : public guiDevice
{
  public:
    guiDeviceSampler( Adafruit_SSD1306 *disp, 
                      audioEngine * engine,
                      enum audioEngine::AUDIO_ENGINE_DEVICES i_engine_sampler,
                      const __FlashStringHelper *device, 
                      const uint16_t x, const uint16_t y, 
                      const uint16_t width, const uint16_t height) ;

};

#endif /*GUI_DEVICE_H_*/
