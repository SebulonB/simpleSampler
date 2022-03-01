#ifndef AUDIO_DEVICE_SAMPLER_H_
#define AUDIO_DEVICE_SAMPLER_H_

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include "audio_device.h"
#include "wav2mem.h"


/*
// GUItool: begin automatically generated code
AudioPlayMemory          playMem;       //xy=134.75,1329
AudioMixer4              outMixer;         //xy=356.5898132324219,1348.974365234375
AudioOutputI2S           i2s1;           //xy=658.8833312988281,1420.8833312988281
AudioConnection          patchCord1(playMem, 0, outMixer, 0);
// GUItool: end automatically generated code
*/


#define DEBUG_AUDIO_DEVICE_SAMPLER


class audioDeviceSampler : public audioDevice
{
  public:
    audioDeviceSampler(const __FlashStringHelper *device);
    AudioStream *getAudioConnectionOut(){return outMixer;}

    //------{ API }---------

    //Sampler
    void openSample(const char *s);

    void setVolume(float v);
    void setStart(float v);
    void setLength(float v);

    //MIDI
    void setNoteMin(uint8_t m){if(m>MIDI_VAL_MAX){return;} m_note_min=m;}
    void setNoteMax(uint8_t m){if(m>MIDI_VAL_MAX){return;} m_note_max=m;}
    void setMidiCh(uint8_t m){if(m>MIDI_CH_MAX){return;} m_midi_ch=m;}

    void midiNoteOn(byte channel, byte note, byte velocity);
    void midiNoteOff(byte channel, byte note, byte velocity);    

  private:
    uint8_t    *m_mem{nullptr};
    float       m_volume{0.0};
    float       m_start{0.0};
    float       m_length{0.0};

    uint8_t     m_note_min{0};
    uint8_t     m_note_max{MIDI_VAL_MAX};
    uint8_t     m_midi_ch{1};

    //patch
    AudioPlayMemory          *playMem;        //xy=134.75,1329
    AudioMixer4              *outMixer;       //xy=356.5898132324219,1348.974365234375
    AudioConnection          *patchCord1;     //(playMem, 0, outMixer, 0);
 



#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
    char str_[100];
#endif

};



#endif /*AUDIO_DEVICE_SAMPLER_H_*/
