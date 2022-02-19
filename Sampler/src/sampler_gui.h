#ifndef SAMPLER_GUI_
#define SAMPLER_GUI_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <vector>

#include "widgets.h"

const char w_param_l[]  PROGMEM = "params";
const char w_map_l[]    PROGMEM = "map";
const char w_file_l[]   PROGMEM = "file";
const char w_midi_l[]   PROGMEM = "midi";


#define DEBUG_WIDGET_SAMPLER_SELECTION


class widgetSamplerSelection
{
  public:
    widgetSamplerSelection(Adafruit_SSD1306 *disp, uint16_t index): 
                          m_display(disp), m_index(index){
                            init();
                          };

    void forceDraw(){
      draw();
    }

    bool setIndex(uint16_t id, bool d){
      if(id>=m_buttons.size()){return false;}
      m_index=id;
      
      if(d){
        m_active=true;
        draw();
      }
      return true;
    }  

    unsigned int getButtonCnt(void){return m_buttons.size();}             
     
    void setActive(bool b){m_active=b;}
    
    void reset(){
      for(auto w : m_buttons){
        w->setActive(false);
      }      
    }

  private:
    Adafruit_SSD1306 *m_display{NULL};  
    uint16_t m_index;
    bool     m_active{false};
    
    std::vector<widgetLabel *> m_buttons;

#ifdef DEBUG_WIDGET_SAMPLER_SELECTION
    char str_[100];
#endif

    void init(){

#ifdef DEBUG_WIDGET_SAMPLER_SELECTION
    sprintf(str_, "widgetSamplerSelection Init active(%s) index(%d)\n", 
        (m_active?"true":"false"), m_index);
    Serial.print(str_);    
#endif   

      widgetLabel* param = new widgetLabel(m_display, w_param_l, 32, 0);
      widgetLabel* map   = new widgetLabel(m_display, w_map_l,   75, 0);
      widgetLabel* file  = new widgetLabel(m_display, w_file_l,  32, 12);
      widgetLabel* midi  = new widgetLabel(m_display, w_midi_l,  32, 24);

      m_buttons.push_back(param);
      m_buttons.push_back(map);
      m_buttons.push_back(file);
      m_buttons.push_back(midi);
    }

   void draw(void){
#ifdef DEBUG_WIDGET_SAMPLER_SELECTION
    sprintf(str_, "widgetSamplerSelection draw active(%s) index(%d)\n", 
        (m_active?"true":"false"), m_index);
    Serial.print(str_);    
#endif   
    
    if(!m_active){return;}

    //reset
    for(auto w : m_buttons){
      w->setActive(false);
    }
    
    if(m_index>=m_buttons.size()){m_index=0;}
    if(m_index< m_buttons.size()){
      m_buttons.at(m_index)->setActive(true);
    }
   }


};

#endif /*SAMPLER_GUI_*/
