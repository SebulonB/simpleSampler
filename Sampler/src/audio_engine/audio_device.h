#ifndef AUDIO_DEVICE_H_
#define AUDIO_DEVICE_H_

#include "widgets/widgets.h"


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

    virtual void setPot(byte channel, float v);

    bool isDeviceType(enum AUDIO_DEVICE_TYPE t){
      return (t == m_device_type ? true : false);
    }

    void setGui(widgetIndicator * gui){
      m_gui = gui;
    }



  protected:
    enum AUDIO_DEVICE_TYPE m_device_type{AUDIO_DEVICE_UNKNOWN};
    const char * l_device;

    widgetIndicator * m_gui{nullptr};  
    uint32_t          m_id{0};


};



#endif /*AUDIO_DEVICE_H_*/
