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


void widgetParam::setEdit(bool e){
    set_edit(e);
}

const char * widgetParam::getLDevice(){
    return l_device;
}

const char * widgetParam::getLParam(){
    return l_param;
}

void widgetParam::triggerCallback(){
    callback();
}

bool widgetParam::isParamType(enum PARAM_TYPE t){
    return (t == m_param_type) ? true : false;
}


