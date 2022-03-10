#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "String.h"

#include "device_param.h"




deviceParam::deviceParam()
{
  for(int i=0; i<max_mod; i++){
    m_mod[i] = 0.0;
  }
}


uint8_t deviceParam::getModIndex()
{
  //find free space
  int i=0;
  for(i=0; i<max_mod; i++){
    if( !(m_free & (1<<i)) ){
       break;
    }
  }
  
  m_free |= (1<<i);

#ifdef DEBUG_DEVICE_PARAM
  sprintf(str_, "device getIndex (%d) free(%2x)\n", i, m_free);
  Serial.print(str_);
#endif

  return i;
}

void    deviceParam::removeModIndex(uint8_t i)
{
  if(i >= max_mod){return;}
  m_free &= ~(1<<i);
  
  //reset to 1.0
  m_mod[i] = 0.0;

#ifdef DEBUG_DEVICE_PARAM
  sprintf(str_, "device removeIndex (%d) free(%2x)\n", i, m_free);
  Serial.print(str_);
#endif  
}

float   deviceParam::getVal(void)
{
  float v = m_param;
  for(int i=0; i<max_mod; i++){
    v += m_mod[i];
  }

#ifdef DEBUG_DEVICE_PARAM
  sprintf(str_, "device param getVal (%6.3f) p(%6.3f) m1(%6.3f) m2(%6.3f) m3(%6.3f) m4(%6.3f) m5(%6.3f) m6(%6.3f)\n", v,
   m_param, m_mod[0], m_mod[1], m_mod[2], m_mod[3], m_mod[4], m_mod[5] );
  Serial.print(str_);
#endif

  return v;
}

void    deviceParam::setParam(float v)
{
  m_param = v;
}

void    deviceParam::setMod(uint8_t i, float v)
{
  if(i >= max_mod){return;}
  if(!(m_free & (1<<i))){return;}

#ifdef DEBUG_DEVICE_PARAM
  sprintf(str_, "device param update mod i(%d) val(%6.3f)\n", i, v);
  Serial.print(str_);
#endif

  m_mod[i] = v;
}
