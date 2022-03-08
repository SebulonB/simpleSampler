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
#include "widget_param_list.h"



//------------ widgetParamList ---------------
// widgetParamList::widgetParamList(Adafruit_SSD1306 *disp, const char *label, uint16_t val_x_pos, enum UNIT u) : 
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

widgetParamList::widgetParamList( Adafruit_SSD1306 *disp, 
                      const char *device,
                      const __FlashStringHelper *label, 
                      const __FlashStringHelper *param,
                      uint16_t val_x_pos, const char *s[], uint32_t n ) : widgetParam()
{
  m_display      = disp;
  m_value_x_pos  = val_x_pos;
 
  for(uint32_t i=0; i<n; i++){
    m_str_l.push_back(s[i]);
  }
  
  m_max          = (m_str_l.size() - 1);

  m_widget_type  = widget::PARAM;  
  m_param_type   = LIST;
  m_label        = new widgetLabel(disp, label); 
  l_device       = device; 
  l_param        = reinterpret_cast<const char *>(param); 
  init();
}

widgetParamList::widgetParamList(  Adafruit_SSD1306 *disp, 
                                    const char *device,
                                    const __FlashStringHelper *label, 
                                    const __FlashStringHelper *param,
                                    uint16_t val_x_pos)
{
  m_display      = disp;
  m_value_x_pos  = val_x_pos;
 
  m_str_l.push_back(str_unknown);
  m_max          = (m_str_l.size() - 1);

  m_widget_type  = widget::PARAM;  
  m_param_type   = LIST;
  m_label        = new widgetLabel(disp, label); 
  l_device       = device; 
  l_param        = reinterpret_cast<const char *>(param); 
  init();   
}      

void widgetParamList::init()
{
  //ger height from label
  uint16_t w,h;
  m_label->getSize(w,h);
  m_height = h;

  m_width = m_value_x_pos + m_value_width;
}

void  widgetParamList::pushString(const char *str)
{
  //clear on init
  if(!m_init_str_l){
    m_str_l.clear();
    m_init_str_l = true;
  }

  m_str_l.push_back(str);
  m_max          = (m_str_l.size() - 1);
}

void widgetParamList::setUpdateCallback(std::function <void (int)> funcp)
{
  p_udpateEngine_callback = funcp;
}

void widgetParamList::callback()
{
  if(p_udpateEngine_callback == nullptr) {return;}
  p_udpateEngine_callback(m_value+m_offset);
}   

void widgetParamList::setValue(uint32_t m)
{
  if(m >= m_max){m=m_max;}
  if(m <= m_min){m=m_min;}

  //clear last digit
  if (m >=  999){
    m           = (uint32_t)std::round(m);
    uint32_t ml = (uint32_t)m%10;
    m  -= ml;
  }
  //round
  else if( m >= 99){
    m = (uint32_t)std::round(m);     
  }

  m_value = m;

  callback();
}

void  widgetParamList::setValueDefault(uint32_t m)
{
    m_value_default = m;
}

void  widgetParamList::setOffset(uint32_t m){
    m_offset = m;
}

void  widgetParamList::useDefaultVal(){
    setValue(m_value_default);
}

uint32_t   widgetParamList::getValue(void){
    return m_value;
}

uint32_t   widgetParamList::getMax(void){
    return m_max;
}

void widgetParamList::inc_value(bool inc)
{
  int iv=0;

  iv = 1;
  if(m_millis_diff <= 25){
    iv = 10;
  }  

  if(!inc){
    iv = -iv;
  }

  int val = (int)m_value;
  val += iv;
  if(val < 0){return;}

  setValue(val);
}

void widgetParamList::draw()
{

  m_label->setPos(m_pos_x, m_pos_y);

  //redraw label
  m_label->setActive(m_active && !m_edit);
  m_label->forceDraw();

  if(m_value >= m_str_l.size()){return;}

  const char *str = m_str_l.at(m_value);

  m_value_text_size = m_label->getTextSize();
  m_value_width     = 5 * m_value_text_size * strlen(str) + strlen(str) + 1;
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


