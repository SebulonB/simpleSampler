#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <Bounce.h>
#include <vector>

#include "widgets.h"


//------------ widgetIndicator ---------------
widgetIndicator::widgetIndicator(Adafruit_SSD1306 *disp, const char *l, unsigned int max) : widget()
{
  m_display = disp;
  if(max > WIDGET_INDICATOR_MAX_ENTRIES){max = WIDGET_INDICATOR_MAX_ENTRIES;}
  
  m_max = max;
  for(unsigned int i=0; i<max; i++){m_index_list[i]=l[i];}
};


void widgetIndicator::setActive(bool a){
  m_active = a;
  draw();
}

void widgetIndicator::setIndex(unsigned int x){
  if(x > WIDGET_INDICATOR_MAX_ENTRIES){x = WIDGET_INDICATOR_MAX_ENTRIES;}   
  m_index = x;
  draw();
}

void widgetIndicator::setXY(unsigned int x, unsigned int y){
  m_pos_x = x;
  m_pos_y = y;
}

void widgetIndicator::setSize(unsigned int w, unsigned int h, unsigned int text){
  m_width = w;
  m_hight = h;
  m_test_s = text;
}

void widgetIndicator::draw(void){
  if(m_display == NULL){return;}
  
#ifdef DEBUG_WIDGET_INDICATOR     
  sprintf(str_, "widgetIdikator draw: active(%s) index(%d | %c)\n", 
          (m_active?"true":"false"), m_index, m_index_list[m_index]);
  Serial.print(str_);      
#endif            

  if(m_active){
    m_display->fillRect(m_pos_x, m_pos_y, m_width, m_hight, SSD1306_WHITE);
    m_display->setTextColor(SSD1306_INVERSE);        // Draw white text
  }
  else{
    m_display->fillRect(m_pos_x, m_pos_y, m_width, m_hight, SSD1306_BLACK);
    m_display->drawRect(m_pos_x, m_pos_y, m_width, m_hight, SSD1306_WHITE);
    m_display->setTextColor(SSD1306_WHITE);        // Draw white text       
  }

  m_display->setTextSize(3);      
  m_display->setCursor(5,6);             // Start at top-left corner
  m_display->print(m_index_list[m_index]);
  m_display->display();      
}



//------------ widgetLabel ---------------
widgetLabel::widgetLabel(Adafruit_SSD1306 *disp, const char *label, uint16_t x, uint16_t y) :
                         widget(), 
                         m_label(label), m_pos_x(x), m_pos_y(y)
{
  m_display = disp;  
  init();
};

void widgetLabel::setActive(bool a){
  m_active = a;
  draw();
}

void widgetLabel::setText(const char *label){
  m_label = label;
  init();
}

void widgetLabel::init(void){
  m_width = 5 * m_text_size * strlen(m_label) + strlen(m_label) + 1;
  m_hight = 8 * m_text_size;
#ifdef DEBUG_WIDGET_LABEL
  sprintf(str_, "widgetLabel Init active(%s) label(%s) size(%3d %3d)\n", 
      (m_active?"true":"false"), m_label, m_width, m_hight);
  Serial.print(str_);    
#endif      
}

void widgetLabel::draw(void){
  if(m_display == NULL){return;}

#ifdef DEBUG_WIDGET_LABEL
sprintf(str_, "widgetLabel draw active(%s) label(%s) size(%3d %3d) pos(%3d %3d)\n", 
    (m_active?"true":"false"), m_label, m_width, m_hight, m_pos_x, m_pos_y);
Serial.print(str_);     
#endif      
  //set 
  uint16_t x = m_pos_x;
  if(x>0){x-=1;}
  
  if(m_active){
    m_display->fillRect(x, m_pos_y, m_width, m_hight, SSD1306_WHITE);
    m_display->setTextColor(SSD1306_INVERSE);        // Draw white text
  }
  else{
    m_display->fillRect(x, m_pos_y, m_width, m_hight, SSD1306_BLACK);
    m_display->setTextColor(SSD1306_WHITE);        // Draw white text       
  }

  m_display->setTextSize(m_text_size);      
  m_display->setCursor(m_pos_x, m_pos_y);             // Start at top-left corner
  m_display->print(m_label);
  m_display->display();   
}    


//------------ widgetPage ---------------
widgetPage::widgetPage(Adafruit_SSD1306 *disp, const char *label) :
              widget(), m_name(label)
{
  m_display = disp;
  init();
}

void widgetPage::setButtonIndex(uint16_t t){
  if(t>=m_buttons.size()){return;}
  m_button_index=t;
}

void widgetPage::signalRotary(EncoderKnopGroup *p_knobs){
#ifdef DEBUG_WIDGET_PAGE
  sprintf(str_, "widgetPage: %s | signalRotary: %d\n", p_knobs->readSteps(0));
  Serial.print(str_);    
#endif   
}

void widgetPage::signalButton(uint8_t button, bool pressed){
#ifdef DEBUG_WIDGET_PAGE
  sprintf(str_, "widgetPage: %s | signalButton: %d %s\n", 
                  p_knobs->readSteps(0), (pressed?"pressed":"released"));
  Serial.print(str_);    
#endif
}

void widgetPage::init(){
#ifdef DEBUG_WIDGET_PAGE
  sprintf(str_, "widgetPage: %s | init\n", m_name);
  Serial.print(str_);    
#endif   
}

void widgetPage::draw(void){
#ifdef DEBUG_WIDGET_PAGE
  sprintf(str_, "widgetPage: %s | draw active(%s) index(%d)\n", 
      m_name, (m_active?"true":"false"), m_index);
  Serial.print(str_);    
#endif   

  if(!m_active){return;}
  if( m_button_index>=m_buttons.size()){return;}

  //reset
  for(auto w : m_buttons){
    w->setActive(false);
  }
  //
  if(m_button_index < m_buttons.size()){
    m_buttons.at(m_button_index)->setActive(true);
  }
}
