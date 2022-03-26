#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <vector>
#include <cmath>

#include <SD.h>

#include "widget.h"
#include "widget_index.h"




//------------ widgetIndicator ---------------
widgetIndicator::widgetIndicator(Adafruit_SSD1306 *disp, const char *l, unsigned int max) : widget()
{
  m_display = disp;
  if(max > WIDGET_INDICATOR_MAX_ENTRIES){max = WIDGET_INDICATOR_MAX_ENTRIES;}
  
  m_max = max;
  for(unsigned int i=0; i<max; i++){m_index_list[i]=l[i];}

  //default init
  m_width  = 25;
  m_height = 32;   
};


bool widgetIndicator::setIndex(unsigned int x){
  if(x >= WIDGET_INDICATOR_MAX_ENTRIES){return false;}   
  if(x >= m_max){return false;}   
  m_index = x;
  draw();
  return true;
}

bool widgetIndicator::incIndex(bool inc)
{
  unsigned int index = m_index;

  if(inc){
    index+=1;
  }
  else{
    index-=1;
  }
  
  return setIndex(index);  

}


void widgetIndicator::noteOn(uint32_t id, uint8_t /*note*/, uint8_t /*vel*/)
{
  if(id >= WIDGET_INDICATOR_MAX_ENTRIES){return;}

  m_note_gate[id] = true;
  draw_note(id);
}

void widgetIndicator::noteOff(uint32_t id, uint8_t /*note*/, uint8_t /*vel*/)
{
  if(id >= WIDGET_INDICATOR_MAX_ENTRIES){return;}

  m_note_gate[id] = false;
  draw_note(id);
}

void widgetIndicator::draw_note(uint8_t ch)
{
  if(m_display == NULL){return;}
  if(ch >= WIDGET_INDICATOR_MAX_ENTRIES){return;}

  if(m_index != ch){return;}

  if      (!m_active && m_note_gate[ch]){
    m_display->fillRect(m_pos_x, m_pos_y, 6, 6, SSD1306_WHITE);
  }
  else if (!m_active && !m_note_gate[ch]){
    m_display->fillRect(m_pos_x, m_pos_y, 6, 6, SSD1306_BLACK);
    m_display->drawRect(m_pos_x, m_pos_y, m_width, m_height, SSD1306_WHITE);    
  }  
  else if (m_active && m_note_gate[ch]){
    m_display->fillRect(m_pos_x, m_pos_y, 6, 6, SSD1306_BLACK);
    m_display->drawRect(m_pos_x, m_pos_y, m_width, m_height, SSD1306_WHITE);      
  }  
  else if (m_active && !m_note_gate[ch]){
    m_display->fillRect(m_pos_x, m_pos_y, 6, 6, SSD1306_WHITE);
  }    

  m_display->display();       
  
#ifdef DEBUG_WIDGET_INDICATOR     
  sprintf(str_, "widgetIdikator draw note gate: active(%s) index(%d | %d) gate(%s)\n", 
          (m_active?"true":"false"), m_index, ch, (m_note_gate[ch]?"on":"off") );
  Serial.print(str_);      
#endif       
}


void widgetIndicator::draw(void){
  if(m_display == NULL){return;}
  
#ifdef DEBUG_WIDGET_INDICATOR     
  sprintf(str_, "widgetIdikator draw: active(%s) index(%d | %c)\n", 
          (m_active?"true":"false"), m_index, m_index_list[m_index]);
  Serial.print(str_);      
#endif            

  if(m_active){
    m_display->fillRect(m_pos_x, m_pos_y, m_width, m_height, SSD1306_WHITE);
    m_display->setTextColor(SSD1306_INVERSE);        // Draw white text
  }
  else{
    m_display->fillRect(m_pos_x, m_pos_y, m_width, m_height, SSD1306_BLACK);
    m_display->drawRect(m_pos_x, m_pos_y, m_width, m_height, SSD1306_WHITE);
    m_display->setTextColor(SSD1306_WHITE);        // Draw white text       
  }

  m_display->setTextSize(3);      
  m_display->setCursor(5,6);             // Start at top-left corner
  m_display->print(m_index_list[m_index]);
  m_display->display();      
}
