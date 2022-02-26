#ifndef AUDIO_DEVICE_H_
#define AUDIO_DEVICE_H_


class audioDevice
{
  public:
    enum AUDIO_DEVICE_TYPE
    {
      AUDIO_DEVICE_UNKNOWN,
      AUDIO_DEVICE_SAMPLER
    }; 

    bool isDeviceType(enum AUDIO_DEVICE_TYPE t){
      return (t == m_device_type ? true : false);
    }

  protected:
    enum AUDIO_DEVICE_TYPE m_device_type{AUDIO_DEVICE_UNKNOWN};
    const char * l_device;

};



#endif /*AUDIO_DEVICE_H_*/
