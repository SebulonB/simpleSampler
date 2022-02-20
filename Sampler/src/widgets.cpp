#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <vector>

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


void widgetIndicator::setIndex(unsigned int x){
  if(x > WIDGET_INDICATOR_MAX_ENTRIES){x = WIDGET_INDICATOR_MAX_ENTRIES;}   
  m_index = x;
  draw();
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
  init();
};

widgetLabel::widgetLabel(Adafruit_SSD1306 *disp, const char *label, uint16_t x, uint16_t y) :
                         widget(), 
                         m_label(label)
{

  m_display = disp;  
  m_pos_x = x;
  m_pos_y = y;

  init();
};


void widgetLabel::setText(const char *label){
  m_label = label;
  init();
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


//------------ widgetList ---------------
widgetList::widgetList( Adafruit_SSD1306 *disp, 
                        const uint16_t x, const uint16_t y, 
                        const uint16_t width, const uint16_t height) : 
                        widget()
{

  m_display = disp;

  m_pos_x = x;
  m_pos_y = y;

  m_width  = width;
  m_height = height;

#ifdef DEBUG_WIDGET_LIST
  sprintf(str_, "create widget list: x(%d), y(%d), width(%d), height(%d)\n",
         m_pos_x, m_pos_y, m_width, m_height );
  Serial.print(str_);    
#endif
}

widget *widgetList::getActiveWidget()
{
  if(   m_widgets.size() == 0 
     || m_widgets.size() <= m_index){
    return NULL;
  }
  
  return m_widgets.at(m_index);
}

void widgetList::pushWidget(widget *w)
{
  //First
  m_widgets.push_back(w);
}

bool widgetList::incIndex(bool inc)
{
  uint16_t index = m_index;
  if(inc){
    index+=1;
  }
  else{
    index-=1;
  }
  return setIndex(index);
}


bool widgetList::setIndex(u_int16_t index)
{
  if(index >= m_widgets.size()){return false;}

  m_index = index;

  //set list pos
  uint16_t w, h;
  for(unsigned i=index; i<m_widgets.size(); i++){
    m_widgets.at(i)->getSize(w,h);
    m_widgets.at(i)->setPos(m_pos_x, m_pos_y + (i-index)*(h + m_seperator));
    
    if(i==index){
      m_widgets.at(i)->setActive(true);
    }
    else{
      m_widgets.at(i)->setActive(false);      
    }
  }

#ifdef DEBUG_WIDGET_LIST
  sprintf(str_, "widget list set index(%d)\n", m_index );
  Serial.print(str_);    
#endif

 return true;
}


void widgetList::draw()
{
  //first blank
  m_display->fillRect(m_pos_x, m_pos_y, m_width, m_height, SSD1306_BLACK);

  uint16_t x, y, w, h;
  for(unsigned int i=m_index; i<m_widgets.size(); i++)
  {
    m_widgets.at(i)->getPos(x, y);
    m_widgets.at(i)->getSize(w, h);
    
    if(i==m_index && m_active){
      m_widgets.at(i)->setActive(true);
    }
    else{
      m_widgets.at(i)->setActive(false);
    }

    if(y+h <= m_height){
      m_widgets.at(i)->forceDraw();
    }
  }
}

//------------ widgetParam ---------------
void widgetParam::incValue(bool inc){

  m_millis_diff = millis() - m_millis_old;
 
  //be shure we are below 30ms
  if(m_millis_diff > 100){
    m_millis_diff_shure = 0;
  }
  else if(m_millis_diff <= 25){
    if(m_millis_diff_shure++ < 3){
      m_millis_diff = 100;
    }
  }

  m_millis_old  = millis();

  inc_value(inc);
}



//------------ widgetParamFloat ---------------
widgetParamFloat::widgetParamFloat(Adafruit_SSD1306 *disp, const char *label, uint16_t val_x_pos, enum UNIT u) : 
                     widgetParam()
{
  m_display      = disp;
  m_value_x_pos  = val_x_pos;
  m_unit         = u;
  m_param_type   = FLOAT;
  m_label        = new widgetLabel(disp, label); 
  init();
}

widgetParamFloat::widgetParamFloat(Adafruit_SSD1306 *disp, const __FlashStringHelper *label, uint16_t val_x_pos, enum UNIT u) : 
                     widgetParam()
{
  m_display      = disp;
  m_value_x_pos  = val_x_pos;
  m_unit         = u;
  m_param_type   = FLOAT;
  m_label        = new widgetLabel(disp, label); 
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
  m_value_last_width = m_value_width;
}

void widgetParamFloat::setValue(float m)
{
  if(m >= m_max){m=m_max;}
  if(m <= m_min){m=m_min;}

  //clear last digit
  if        (m >= 1000){
    m           = (uint32_t)m;
    uint32_t ml = (uint32_t)m%10;
    m  -= ml;
  }
  //round
  else if( m >= 100){
    m = (uint32_t)m;     
  }

  m_value = m;
}

void widgetParamFloat::inc_value(bool inc)
{
  float iv=0;

  iv = 0.1;
  if(m_millis_diff <= 25){
    iv = 1;
  }  

  if(m_unit == SECONDS){
    if(m_value >= 1000){
      iv *=100;
    }
    else if(m_value >= 100){
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
    if(     m_value < 100.){
      sprintf(str, "%4.1f", m_value);
    }
    else if(m_value >= 100){
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
  cal_val_str(str);
  uint16_t x=m_pos_x+m_value_x_pos;
  
  //clear
  m_display->fillRect(x, m_pos_y, m_value_last_width, m_height, SSD1306_WHITE);

  if(m_active && m_edit){
    m_display->setTextColor(SSD1306_INVERSE);        // Draw white text
  }
  else{
    m_display->fillRect(x, m_pos_y, m_value_width, m_height, SSD1306_BLACK);
    m_display->setTextColor(SSD1306_WHITE);        // Draw white text       
  }  
  m_value_last_width = m_value_width;

  m_display->setTextSize(m_value_text_size);      
  m_display->setCursor(x+1, m_pos_y+1);             // Start at top-left corner
  m_display->print(str);
  m_display->display();     
}