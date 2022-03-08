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

    enum MIDI_BOUNDRY
    {
      MIDI_CH_MAX = 16,
      MIDI_VAL_MAX = 127,
    };

    virtual AudioStream *getOutputStream(uint8_t audio_ch);

    //API
    virtual void midiNoteOn(byte channel, byte note, byte velocity);
    virtual void midiNoteOff(byte channel, byte note, byte velocity);    
    virtual void midiCC(byte channel, byte control, byte value);    

    bool isDeviceType(enum AUDIO_DEVICE_TYPE t){
      return (t == m_device_type ? true : false);
    }

  protected:
    enum AUDIO_DEVICE_TYPE m_device_type{AUDIO_DEVICE_UNKNOWN};
    const char * l_device;

};



#endif /*AUDIO_DEVICE_H_*/
