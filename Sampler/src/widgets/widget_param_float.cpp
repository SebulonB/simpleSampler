#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <vector>
#include <cmath>

#include <SD.h>

#include "widget.h"
#include "widget_param.h"
#include "widget_param_float.h"


// #define DEBUG_WIDGET_PARAM_FLOAT




//------------ widgetParamFloat ---------------
// widgetParamFloat::widgetParamFloat(Adafruit_SSD1306 *disp, const char *label, uint16_t val_x_pos, enum UNIT u) : 
//                      widgetParam()
// {
//   m_display      = disp;
//   m_value_x_pos  = val_x_pos;
//   m_unit         = u;
//   m_widget_type  = widget::PARAM;
//   m_param_type   = FLOAT;
//   m_label        = new widgetLabel(disp, label); 
//   init();
// }

widgetParamFloat::widgetParamFloat( Adafruit_SSD1306 *disp, 
                      const char *device,
                      const __FlashStringHelper *label, 
                      const __FlashStringHelper *param,
                      uint16_t val_x_pos, enum UNIT u ) : widgetParam()
{
  m_display      = disp;
  m_value_x_pos  = val_x_pos;
  m_unit         = u;
  m_widget_type  = widget::PARAM;  
  m_param_type   = FLOAT;
  m_label        = new widgetLabel(disp, label); 
  l_device       = device; 
  l_param        = reinterpret_cast<const char *>(param); 
  init();
}

void widgetParamFloat::init()
{
  //ger height from label
  uint16_t w,h;
  m_label->getSize(w,h);
  m_height = h;

  char str[10];
  cal_val_str(str);

  m_width = m_value_x_pos + m_value_width;
}

void widgetParamFloat::setUpdateCallback(std::function <void (float)> funcp)
{
  p_udpateEngine_callback = funcp;
}

void widgetParamFloat::callback()
{
  if(p_udpateEngine_callback == nullptr) {return;}
  p_udpateEngine_callback(m_value);
}   

void widgetParamFloat::setValue(float m)
{
  if(m >= m_max){m=m_max;}
  if(m <= m_min){m=m_min;}

  //clear last digit
  if (m >=  999.9){
    m           = (uint32_t)std::round(m);
    uint32_t ml = (uint32_t)m%10;
    m  -= ml;
  }
  //round
  else if( m >= 99.9){
    m = (uint32_t)std::round(m);     
  }

  m_value = m;

  callback();
}

void widgetParamFloat::inc_value(bool inc)
{
  float iv=0;

  iv = 0.1;
  if(m_millis_diff <= 25){
    iv = 1;
  }  

  if(m_unit == SECONDS){
    if(m_value >= 1000.){
      iv *=100;
    }
    else if(m_value >= 100.){
      iv *=10;
    }
  }

  if(!inc){
    iv = -iv;
  }

  float val = m_value;
  val += iv;

  setValue(val);
}

uint16_t widgetParamFloat::cal_val_str(char *str)
{
  //generate value string
  uint16_t l=0; 
  if(     m_unit == PERCENT){
    sprintf(str, "%5.1f", m_value);
    l = strlen(str);
    sprintf(str+l, "%%");
  }
  else if(m_unit == SECONDS){
    if(     m_value < 100.0){
      sprintf(str, "%4.1f", m_value);
    }
    else if(m_value >= 100.0){
      sprintf(str, "%0.0f", m_value);      
    }
    l = strlen(str);
    sprintf(str+l, "ms");
  }

  l = strlen(str);
  m_value_text_size = m_label->getTextSize();
  m_value_width     = 5 * m_value_text_size * strlen(str) + strlen(str) + 1;

  return l;
}

void widgetParamFloat::draw()
{

  m_label->setPos(m_pos_x, m_pos_y);

  //redraw label
  m_label->setActive(m_active && !m_edit);
  m_label->forceDraw();

  char str[10];
  memset(str, 0, 10*sizeof(char));
  cal_val_str(str);
  uint16_t x=m_pos_x+m_value_x_pos;
  
  //clear Value on Screen
  if(m_value_width_max < m_value_width){
    m_value_width_max = m_value_width;
  }
  m_display->fillRect(x, m_pos_y, m_value_width_max, m_height, SSD1306_BLACK);

  if(m_active && m_edit){
    m_display->fillRect(x, m_pos_y, m_value_width, m_height, SSD1306_WHITE);    
    m_display->setTextColor(SSD1306_INVERSE);        // Draw white text
  }
  else{
    m_display->setTextColor(SSD1306_WHITE);        // Draw white text       
  }  

  m_display->setTextSize(m_value_text_size);      
  m_display->setCursor(x+1, m_pos_y+1);             // Start at top-left corner
  m_display->print(str);
  m_display->display();     
}


