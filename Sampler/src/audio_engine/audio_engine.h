#ifndef AUDIO_ENGINE_H_
#define AUDIO_ENGINE_H_

#include <Arduino.h>
#include <String.h>
#include <vector>
#include "audio_device.h"
#include "audio_device_sampler.h"


/*
// GUItool: begin automatically generated code
AudioMixer4              outMixerCH2;         //xy=915.6480178833008,516.7592353820801
AudioMixer4              outMixerCH1;         //xy=919.9233093261719,425.08539962768555
AudioOutputI2S           i2s1;           //xy=1228.8834228515625,455.8832702636719
AudioConnection          patchCord1(outMixerCH2, 0, i2s1, 1);
AudioConnection          patchCord2(outMixerCH1, 0, i2s1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=826.2166061401367,606.2168235778809
// GUItool: end automatically generated code
*/

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
  

    //audio Engine --Main--
    AudioMixer4              *outMixerCH2;    //xy=915.6480178833008,516.7592353820801
    AudioMixer4              *outMixerCH1;    //xy=919.9233093261719,425.08539962768555
    AudioOutputI2S           *i2s1;           //xy=1228.8834228515625,455.8832702636719
    AudioConnection          *patchCord1;     //(outMixerCH2, 0, i2s1, 1);
    AudioConnection          *patchCord2;     //(outMixerCH1, 0, i2s1, 0);
    AudioControlSGTL5000     *sgtl5000_1;     //xy=826.2166061401367,606.2168235778809    
    std::vector<AudioConnection *> m_out_connections;
    

#ifdef DEBUG_AUDIO_ENGINE
    char str_[100];
#endif

};



#endif /* AUDIO_ENGINE_H_ */
