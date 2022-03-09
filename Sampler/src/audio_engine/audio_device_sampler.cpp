#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <MIDI.h>

#include "audio_device.h"
#include "audio_device_sampler.h"
#include "tinyalloc/tinyalloc.h"


extern uint8_t external_psram_size; //in MB. Set in startup.c


audioDeviceSampler::audioDeviceSampler(const __FlashStringHelper *device) : audioDevice()
{

  l_device = reinterpret_cast<const char *>(device); 
  m_device_type = AUDIO_DEVICE_SAMPLER;
  //patch
  playMem      = new  AudioPlayMemoryC();      
  envelope     = new  AudioEffectEnvelope();
  outMixerCH2  = new  AudioMixer4();  
  outMixerCH1  = new  AudioMixer4();  
  patchCord1   = new  AudioConnection( *playMem,  0, *envelope, 0);  
  patchCord2   = new  AudioConnection( *envelope, 0,  *outMixerCH2, 0);    
  patchCord3   = new  AudioConnection( *envelope, 0,  *outMixerCH1, 0);   


  //init
  envelope->delay(0);
  envelope->sustain(1.0);
  //envelope->release(0.0);
  //envelope->releaseNoteOn(0);

  //reset mod
  for(int i=0; i<MOD_CC_A; i++){
    m_cc_src[i] = 0xff; //turn off
  }

  //reg CC0 -> CC_A -> Volume
  m_cc_src[MOD_CC_A]      = 1; //CC1
  m_cc_dest[MOD_CC_A]     = MOD_DEST_VOLUME;
  m_cc_mod_slot[MOD_CC_A] = m_master_vol.getModIndex();


#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Indit Audio Device Sampler (%s)\n", l_device);
  Serial.print(str_);
#endif

}

void audioDeviceSampler::midiNoteOn(byte channel, byte note, byte velocity)
{

  if(channel != m_midi_ch){return;}
  if(note < m_note_min || note > m_note_max){return;}
  if(m_mem == NULL){return;}

  m_velocity = velocity/127.;

  AudioNoInterrupts(); 
  set_volume();

  unsigned int * p = (unsigned int *)m_mem;  
  envelope->releaseNoteOn(0);  
  envelope->noteOn();
  playMem->play(p); 
  
  AudioInterrupts();

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) noteOn ch(%d) note(%d) vel(%d) min(%d) max(%d) mem(%p)\n", 
    l_device, channel, note, velocity, m_note_min, m_note_max, p);
  Serial.print(str_);
#endif



}

void audioDeviceSampler::midiNoteOff(byte channel, byte note, byte velocity)
{

  if(channel != m_midi_ch){return;}
  if(note < m_note_min || note > m_note_max){return;}

  if(m_mem == NULL){return;}
  
  if(m_sustain <= 0.0) {return;}

  AudioNoInterrupts();  
  envelope->noteOff();  
  //playMem->stop(); 
  AudioInterrupts(); 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) noteOff ch(%d) note(%d) vel(%d)\n", 
    l_device, channel, note, velocity);
  Serial.print(str_);
#endif
}    


void audioDeviceSampler::midiCC(byte channel, byte control, byte value)
{

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) midiCC ch(%d) cc(%d) val(%d)\n", 
    l_device, channel, control, value);
  Serial.print(str_);
#endif


  if(channel != m_midi_ch){return;}

  int i=0;
  for(i=0; i<MOD_CC_NUM; i++){
    if(m_cc_src[i] == control){
      break;
    }
  }

  if(i>=MOD_CC_NUM){
    return;
  }

  switch(m_cc_dest[i]){
    case MOD_DEST_VOLUME:
      m_master_vol.setMod(m_cc_mod_slot[i], value/127.);
      set_volume();
    break;

    default:
    break;
  }
  
}


void audioDeviceSampler::openSample(const char *s)
{
  if(s == nullptr || s == NULL){return;}
  if(external_psram_size <= 0){
    return;
  }

  File dataFile = SD.open(s); 
  if(m_mem != NULL){  
    ta_free(m_mem);
    m_mem = NULL;
  }
  //get multible of 32
  m_mem = (uint8_t *)ta_alloc(dataFile.size());
  if(m_mem == NULL){return;}

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) openSample(%s) size(%lld) p_mem(%p)\n", l_device, s, dataFile.size(), m_mem);
  Serial.print(str_);
#endif

 wav2m(&dataFile, (unsigned int *)m_mem);
 
 dataFile.close();

}

void audioDeviceSampler::set_volume()
{
  float m  = m_master_vol.getVal();
  float v1 = m_ch1_vol * m * m_velocity;
  float v2 = m_ch2_vol * m * m_velocity;


  AudioNoInterrupts();  
  
  for(int i=0; i<4; i++){
    outMixerCH2->gain(i, v1);
    outMixerCH1->gain(i, v2);
  }

  AudioInterrupts(); 
}

void audioDeviceSampler::setVolume(float v)
{

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setVolume(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif

  //normalize
  m_master_vol.setParam(v/100.);
  set_volume();
}

void audioDeviceSampler::setVolCH1(float v)
{

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setVolCh1(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif
  //normalize
  m_ch1_vol = v/100.;
  set_volume();
}

void audioDeviceSampler::setVolCH2(float v)
{

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setVolCh2(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif

  //normalize
  m_ch2_vol = v/100.;;
  set_volume();
}

void audioDeviceSampler::setStart(float v)
{

  AudioNoInterrupts();  
  //gui use 100%
  playMem->setStart(v/100.);
  AudioInterrupts(); 

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


void audioDeviceSampler::setPitch(float v)
{
  AudioNoInterrupts();  
  //gui use 100%
  playMem->setPitch(v/100.);
  AudioInterrupts(); 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setPitch(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif  
}



void audioDeviceSampler::setAttack(float v)
{
  AudioNoInterrupts();  
  envelope->attack(v);
  AudioInterrupts(); 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setAttack(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif
}

void audioDeviceSampler::setHold(float v)
{
  AudioNoInterrupts();  
  envelope->hold(v);
  AudioInterrupts(); 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setHold(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif

}

void audioDeviceSampler::setDecay(float v)
{
  AudioNoInterrupts();  
  envelope->decay(v);
  AudioInterrupts(); 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setDecay(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif  
}

void audioDeviceSampler::setSustain(float v)
{
  m_sustain = v/100.;
  AudioNoInterrupts();  
  envelope->sustain(m_sustain);
  AudioInterrupts(); 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setSustain(%8.2f)\n", l_device, m_sustain);
  Serial.print(str_);
#endif  
}

void audioDeviceSampler::setRelease(float v)
{
  AudioNoInterrupts();  
  envelope->release(v);
  AudioInterrupts(); 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setRelease(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif  
}



void audioDeviceSampler::setNoteMin(uint8_t m){
  if(m>MIDI_VAL_MAX){m = MIDI_VAL_MAX;}

 #ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setMidiNoteMin(%d)\n", l_device, m);
  Serial.print(str_);
#endif    
  m_note_min=m;
}

void audioDeviceSampler::setNoteMax(uint8_t m)
{
  if(m>MIDI_VAL_MAX){m = MIDI_VAL_MAX;}

 #ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setMidiNoteMax(%d)\n", l_device, m);
  Serial.print(str_);
#endif    

  m_note_max=m;
}

void audioDeviceSampler::setMidiCh(uint8_t m)
{
  if(m>MIDI_CH_MAX){return;} 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setMidiNoteMax(%d)\n", l_device, m);
  Serial.print(str_);
#endif    
  
  m_midi_ch=m;
}
