#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <vector>
#include <cmath>

#include <SD.h>

#include "widget.h"
#include "widgets.h"
#include "widget_label.h"


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
