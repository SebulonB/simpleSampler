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

  Serial.println("Inc Index ");
  Serial.print(index);
  Serial.println("\n");

  if(inc){
    index+=1;
  }
  else{
    index-=1;
  }
  
  return setIndex(index);  

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

  m_widget_type  = widget::LIST;  

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

#ifdef DEBUG_WIDGET_LIST
  sprintf(str_, "widget inc index index(%d)\n", m_index );
  Serial.print(str_);    
#endif

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



//------------ widgetParamInt ---------------
// widgetParamInt::widgetParamInt(Adafruit_SSD1306 *disp, const char *label, uint16_t val_x_pos, enum UNIT u) : 
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

widgetParamInt::widgetParamInt( Adafruit_SSD1306 *disp, 
                      const char *device,
                      const __FlashStringHelper *label, 
                      const __FlashStringHelper *param,
                      uint16_t val_x_pos, enum UNIT u ) : widgetParam()
{
  m_display      = disp;
  m_value_x_pos  = val_x_pos;
  m_unit         = u;
  m_widget_type  = widget::PARAM;  
  m_param_type   = INT;
  m_label        = new widgetLabel(disp, label); 
  l_device       = device; 
  l_param        = reinterpret_cast<const char *>(param); 
  init();
}

void widgetParamInt::init()
{
  //ger height from label
  uint16_t w,h;
  m_label->getSize(w,h);
  m_height = h;

  char str[10];
  cal_val_str(str);

  m_width = m_value_x_pos + m_value_width;
}

void widgetParamInt::setUpdateCallback(std::function <void (int)> funcp)
{
  p_udpateEngine_callback = funcp;
}

void widgetParamInt::callback()
{
  if(p_udpateEngine_callback == nullptr) {return;}
  p_udpateEngine_callback(m_value);
}   

void widgetParamInt::setValue(int m)
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

void widgetParamInt::inc_value(bool inc)
{
  int iv=0;

  iv = 1;
  if(m_millis_diff <= 25){
    iv = 10;
  }  

  if(!inc){
    iv = -iv;
  }

  int val = m_value;
  val += iv;

  setValue(val);
}

uint16_t widgetParamInt::cal_val_str(char *str)
{
  //generate value string
  uint16_t l=0; 
  sprintf(str, "%d", m_value);
  l = strlen(str);
  m_value_text_size = m_label->getTextSize();
  m_value_width     = 5 * m_value_text_size * strlen(str) + strlen(str) + 1;

  return l;
}

void widgetParamInt::draw()
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

//------------ widgetParamBrowser---------------
widgetParamBrowser::widgetParamBrowser( Adafruit_SSD1306 *disp, 
                                        const char *device,
                                        const __FlashStringHelper *label, 
                                        const __FlashStringHelper *param,
                                        uint16_t val_x_pos ) : widgetParam()
{
  m_display      = disp;
  m_value_x_pos  = val_x_pos;  
  m_widget_type  = widget::PARAM;  
  m_param_type   = BROWSER;
  m_label        = new widgetLabel(disp, label); 
  l_device       = device; 
  l_param        = reinterpret_cast<const char *>(param); 
  init();
}


void widgetParamBrowser::init()
{
  //ger height from label
  uint16_t w,h;
  m_label->getSize(w,h);
  m_height = h;

  memset(m_value, 0, MAX_VALUE_STRING_SIZE*sizeof(char));
  strcpy(m_value, str_unknown);

  //m_width;
}

void widgetParamBrowser::setUpdateCallback(std::function <void (const char *)> funcp)
{
  p_udpateEngine_callback = funcp;
}

void widgetParamBrowser::callback()
{
  if(p_udpateEngine_callback == nullptr) {return;}
  String s=m_dir;//[1]; //remove '/'
  s.append('/');
  s.append(m_value);
  s.append(F(".wav"));
  p_udpateEngine_callback(s.c_str());
}   

void widgetParamBrowser::set_edit(bool e){
  m_edit = e;

  if(m_edit == false){
    //ok load callback
    callback();
  }
}

void widgetParamBrowser::inc_value(bool inc)
{
  int16_t iv = 1;

  if(m_millis_diff <= 25){
    iv = 5;
  }  

  if(!inc){
    iv = -iv;
  }

  int16_t val = static_cast<int16_t>(m_index);
  val += iv;

  //clip
  if(val < 0){
    val = 0;
  }
  if(val >= MAX_BROWSER_VALUES){
    val = MAX_BROWSER_VALUES;
  }

  m_index = val;
  
  browse();
}

void widgetParamBrowser::draw()
{
  m_label->setPos(m_pos_x, m_pos_y);

  //redraw label
  m_label->setActive(m_active && !m_edit);
  m_label->forceDraw();

  m_value_text_size = m_label->getTextSize();
  m_value_width     = 5 * m_value_text_size * strlen(m_value) + strlen(m_value) + 1;  
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
  m_display->print(m_value);
  m_display->display();     

}

void widgetParamBrowser::browse()
{
#ifdef DEBUG_WIDGET_PARAM_BROWSER
  sprintf(str_, "Inc Browser: Device(%s) Param(%s) Value(%s) Index(%d) IndexOld(%d)\n",
                l_device, l_param, m_value, m_index, m_index_old);
  Serial.print(str_);
#endif


  if(m_index < m_index_old)
  { 
    if(m_folder){
      m_folder.close();
    }
  }

  if(!m_folder){    
    m_folder    = SD.open(m_dir);
    m_index_old = 0;    
  }

  //Find File to Index
  File m_file;

  while(true){
    m_file = m_folder.openNextFile();

    if(!m_file){
      m_index = m_index_old;
      break;
    }

    if(!m_file.isDirectory())
    {
  #ifdef DEBUG_WIDGET_PARAM_BROWSER
        sprintf( str_, "Browser Open next File(%s) Size(%lld)\n", 
                m_file.name(), m_file.size());
        Serial.print(str_);
  #endif 

      String file_s = m_file.name();   

      //check for unknown files (starting with .   | unix)
      if(file_s.startsWith('.')){
        continue;
      }

      uint16_t len = file_s.lastIndexOf('.');
      if(len > MAX_VALUE_STRING_SIZE){
        len = MAX_VALUE_STRING_SIZE;
      }

      file_s =file_s.toUpperCase();

      //*.wav file?
      if(!file_s.endsWith("WAV")){
        continue;
      }      
      
      if(++m_index_old >= m_index){
        memset(m_value, 0, MAX_VALUE_STRING_SIZE*sizeof(char));
        memcpy(m_value, m_file.name(), len*sizeof(char));        
        break;
      }
    }
  }//end while

}

