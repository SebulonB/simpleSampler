#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <MIDI.h>

#include "static_malloc/static_malloc.h"
#include "audio_device.h"
#include "audio_device_sampler.h"






audioDeviceSampler::audioDeviceSampler(const __FlashStringHelper *device) : audioDevice()
{

  l_device = reinterpret_cast<const char *>(device); 
  m_device_type = AUDIO_DEVICE_SAMPLER;
  //patch
  playMem    = new  AudioPlayMemory();        
  outMixer   = new  AudioMixer4();       
  patchCord1 = new  AudioConnection( *playMem, 0, *outMixer, 0);      


#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Indit Audio Device Sampler (%s)\n", l_device);
  Serial.print(str_);
#endif

}



void audioDeviceSampler::openSample(const char *s)
{

  if(s == nullptr || s == NULL){return;}

  File dataFile = SD.open(s); 
  if(m_mem != NULL){
    sm_free(m_mem);
  }
  m_mem = (uint8_t *)sm_malloc(dataFile.size());  

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) openSample(%s) size(%lld) p_mem(%p)\n", l_device, s, dataFile.size(), m_mem);
  Serial.print(str_);
#endif

wav2m(&dataFile, (unsigned int *)m_mem);


}

void audioDeviceSampler::setVolume(float v)
{

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setVolume(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif
}

void audioDeviceSampler::setStart(float v)
{

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setStart(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif
}

void audioDeviceSampler::setLength(float v)
{

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setLength(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif
}


