#ifndef AUDIO_ENGINE_H_
#define AUDIO_ENGINE_H_

#include <Arduino.h>
#include <String.h>
#include <vector>
#include "audio_device.h"
#include "audio_device_sampler.h"


#define DEBUG_AUDIO_ENGINE


class audioEngine
{
  public:
    enum AUDIO_ENGINE_DEVICES
    {
      DEVICE_SAMPLER_1 = 0,
      DEVICE_SAMPLER_2,
      DEVICE_SAMPLER_3,
      DEVICE_SAMPLER_4,
      DEVICE_SAMPLER_5,   
      DEVICE_SAMPLER_6,   
      DEVICE_NUM                            
    };

    audioEngine();
    
    //API
    audioDevice *getDevice(enum AUDIO_ENGINE_DEVICES d){return get_device(d);}    
    uint32_t     getRealDeviceCNT(){return (uint32_t)DEVICE_NUM;}

    void midiNoteOn(byte channel, byte note, byte velocity);
    void midiNoteOff(byte channel, byte note, byte velocity);

  private:
    audioDevice * m_devices[DEVICE_NUM];
    audioDevice * get_device(enum AUDIO_ENGINE_DEVICES d);

#ifdef DEBUG_AUDIO_ENGINE
    char str_[100];
#endif

};



#endif /* AUDIO_ENGINE_H_ */
