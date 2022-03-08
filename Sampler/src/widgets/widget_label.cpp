#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <vector>
#include <cmath>

#include <SD.h>

#include "widget.h"
#include "widget_label.h"





//------------ widgetLabel ---------------
widgetLabel::widgetLabel(Adafruit_SSD1306 *disp, const char *label) :
                         widget(), 
                         m_label(label)
{
  m_display = disp;  
  init();
};

widgetLabel::widgetLabel(Adafruit_SSD1306 *disp, const __FlashStringHelper *label) :
                         widget()
{
  m_display = disp; 
  m_label    = reinterpret_cast<const char *>(label);
  init();
};

widgetLabel::widgetLabel(Adafruit_SSD1306 *disp, const __FlashStringHelper *label, uint16_t x, uint16_t y) :
                         widget()
{

  m_display = disp;
  m_label    = reinterpret_cast<const char *>(label);    
  m_pos_x = x;
  m_pos_y = y;
  m_widget_type  = widget::LABEL;  

  init();
};

widgetLabel::widgetLabel(Adafruit_SSD1306 *disp, const char *label, uint16_t x, uint16_t y) :
                         widget(), 
                         m_label(label)
{

  m_display = disp;  
  m_pos_x = x;
  m_pos_y = y;
  m_widget_type  = widget::LABEL;    

  init();
};

uint8_t widgetLabel::getTextSize(){
    return m_text_size;
}

void widgetLabel::setText(const char *label){
  m_label = label;
  init();
}

const char * widgetLabel::getText()
{
    if(m_label == NULL){return str_unknown;}
    return m_label;
}

void widgetLabel::init(void){
  m_width = 5 * m_text_size * strlen(m_label) + strlen(m_label) + 1;
  m_height = 8 * m_text_size + 1;
#ifdef DEBUG_WIDGET_LABEL
  sprintf(str_, "widgetLabel Init active(%s) label(%s) size(%3d %3d)\n", 
      (m_active?"true":"false"), m_label, m_width, m_height);
  Serial.print(str_);    
#endif      
}

void widgetLabel::draw(void){
  if(m_display == NULL){return;}

#ifdef DEBUG_WIDGET_LABEL
sprintf(str_, "widgetLabel draw active(%s) label(%s) size(%3d %3d) pos(%3d %3d)\n", 
    (m_active?"true":"false"), m_label, m_width, m_height, m_pos_x, m_pos_y);
Serial.print(str_);     
#endif      
  //set 
  uint16_t x = m_pos_x;
  //if(x>0){x-=1;}
  
  if(m_active){
    m_display->fillRect(x, m_pos_y, m_width, m_height, SSD1306_WHITE);
    m_display->setTextColor(SSD1306_INVERSE);        // Draw white text
  }
  else{
    m_display->fillRect(x, m_pos_y, m_width, m_height, SSD1306_BLACK);
    m_display->setTextColor(SSD1306_WHITE);        // Draw white text       
  }

  m_display->setTextSize(m_text_size);      
  m_display->setCursor(m_pos_x+1, m_pos_y+1);             // Start at top-left corner
  m_display->print(m_label);
  m_display->display();   
}    

