#ifndef SAMPLER_GUI_
#define SAMPLER_GUI_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>

#include "widgets.h"

const char w_param_l[]  PROGMEM = "param";
const char w_map_l[]    PROGMEM = "map";
const char w_file_l[]   PROGMEM = "file";
const char w_midi_l[]   PROGMEM = "midi";


#define DEBUG_WIDGET_SAMPLER_SELECTION


class widgetSamperSelection
{
  public:
    widgetSamperSelection(Adafruit_SSD1306 *disp, uint16_t index): 
                          m_display(disp), m_index(index){
                            init();
                          };

                          

  private:
    Adafruit_SSD1306 *m_display{NULL};  
    uint16_t m_index;
    bool     m_active{false};

    widgetButton *m_button_param{NULL};
    widgetButton *m_button_map{NULL};
    widgetButton *m_button_file{NULL};
    widgetButton *m_button_midi{NULL};

#ifdef DEBUG_WIDGET_SAMPLER_SELECTION
    char str_[100];
#endif

    void init(){

#ifdef DEBUG_WIDGET_BUTTON
    sprintf(str_, "widgetSamplerSelection Init active(%s) index(%d)\n", 
        (m_active?"true":"false"), m_index);
    Serial.print(str_);    
#endif   

      m_button_param = new widgetButton(m_display, w_param_l, 32, 0);
      m_button_map   = new widgetButton(m_display, w_map_l,   70, 0);
      m_button_file  = new widgetButton(m_display, w_file_l,  32, 12);
      m_button_midi  = new widgetButton(m_display, w_midi_l,  32, 24);


    }
  

}

#endif /*SAMPLER_GUI_*/
