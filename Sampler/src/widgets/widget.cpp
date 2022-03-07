#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <vector>
#include <cmath>

#include <SD.h>

#include "widgets.h"



//------------ widget---------------
void widget::getPos(uint16_t &x, uint16_t &y){
    x = m_pos_x;
    y = m_pos_y;
}
void widget::setPos(uint16_t x, uint16_t y)
{
  m_pos_x = x;
  m_pos_y = y;
}

void widget::getSize(uint16_t &w, uint16_t &h){
  w = m_width;
  h = m_height;
}
void widget::setSize(uint16_t w, uint16_t h)
{
  m_width  = w;
  m_height = h; 
}


void widget::forceDraw(void){draw();}    
void widget::setActive(bool b){m_active=b;} 

bool widget::isWidgetType(enum WIDGET_TYPE t){
  return (t == m_widget_type) ? true : false;
}



