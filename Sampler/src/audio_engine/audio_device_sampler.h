#ifndef AUDIO_DEVICE_SAMPLER_H_
#define AUDIO_DEVICE_SAMPLER_H_

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include "audio_device.h"
#include "wav2mem.h"
#include "playMemC.h"


/*
// GUItool: begin automatically generated code
AudioPlayMemory          playMem;        //xy=625.7406463623047,438.8889331817627
AudioEffectEnvelope      envelope;      //xy=793.3332824707031,437.7777500152588
AudioMixer4              outMixerCH2;         //xy=1023.3331756591797,541.1110954284668
AudioMixer4              outMixerCH1;       //xy=1046.6296768188477,384.55560302734375
AudioOutputI2S           i2s1;           //xy=1260.8518295288086,475.44446182250977
AudioConnection          patchCord1(playMem, envelope);
AudioConnection          patchCord2(envelope, 0, outMixerCH1, 0);
AudioConnection          patchCord3(envelope, 0, outMixerCH2, 0);
// GUItool: end automatically generated code
*/


//#define DEBUG_AUDIO_DEVICE_SAMPLER


class audioDeviceSampler : public audioDevice
{
  public:
    audioDeviceSampler(const __FlashStringHelper *device);
    AudioStream *getOutputStream(uint8_t audio_ch){
      if(audio_ch == 0){return outMixerCH1;}
      else             {return outMixerCH2;}
    }


    //------{ API }---------

    //Sampler
    void openSample(const char *s);

    void setVolume(float v);
    void setVolCH1(float v);
    void setVolCH2(float v);

    void setStart(float v);
    void setLength(float v);

    void setAttack(float v);
    void setHold(float v);
    void setDecay(float v);
    void setSustain(float v);
    void setRelease(float v);    

    void setPitch(float v);


    //MIDI
    void setNoteMin(uint8_t m){if(m>MIDI_VAL_MAX){return;} m_note_min=m;}
    void setNoteMax(uint8_t m){if(m>MIDI_VAL_MAX){return;} m_note_max=m;}
    void setMidiCh(uint8_t m){if(m>MIDI_CH_MAX){return;} m_midi_ch=m;}

    void midiNoteOn(byte channel, byte note, byte velocity);
    void midiNoteOff(byte channel, byte note, byte velocity);    

    void midiCC(byte channel, byte control, byte value);

  private:
    uint8_t *m_mem{NULL};

    float       m_master_vol{0.0};
    float       m_velocity{1.0};
    float       m_ch1_vol{0.0};
    float       m_ch2_vol{0.0};    

    uint8_t     m_note_min{0};
    uint8_t     m_note_max{MIDI_VAL_MAX};
    uint8_t     m_midi_ch{1};

    void set_volume();

    //patch
    AudioPlayMemoryC         *playMem;  
    AudioEffectEnvelope      *envelope;            
    AudioMixer4              *outMixerCH1;   
    AudioMixer4              *outMixerCH2;          
    AudioConnection          *patchCord1;     
    AudioConnection          *patchCord2;      
    AudioConnection          *patchCord3; 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
    char str_[100];
#endif

};



#endif /*AUDIO_DEVICE_SAMPLER_H_*/
