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
#include "widget_param_browser.h"


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


