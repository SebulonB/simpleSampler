#ifndef AUDIO_DEVICE_SAMPLER_H_
#define AUDIO_DEVICE_SAMPLER_H_

#include "audio_device.h"



#define DEBUG_AUDIO_DEVICE_SAMPLER


class audioDeviceSampler : public audioDevice
{
  public:
    audioDeviceSampler(const __FlashStringHelper *device);

    //API
    void setVolume(float v);
    void openSample(const char *s);
    void setStart(float v);
    void setLength(float v);

  private:
    const char *m_sample;
    float       m_volume{0.0};
    float       m_start{0.0};
    float       m_length{0.0};

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
    char str_[100];
#endif

};



#endif /*AUDIO_DEVICE_SAMPLER_H_*/
