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
  playMem      = new  AudioPlayMemory();      
  envelope     = new  AudioEffectEnvelope();
  outMixerCH2  = new  AudioMixer4();  
  outMixerCH1  = new  AudioMixer4();  
  //patchCord1   = new  AudioConnection( *playMem,  0, *envelope, 0);  
  patchCord2   = new  AudioConnection( *playMem, 0, *outMixerCH2, 0);    
  patchCord3   = new  AudioConnection( *playMem, 0, *outMixerCH1, 0);   


  //init
  envelope->delay(0);
  envelope->sustain(1.0);
  //envelope->release(0.0);
  //envelope->releaseNoteOn(0);

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

  AudioNoInterrupts(); 
  unsigned int * p = (unsigned int *)m_mem;  
  //envelope->releaseNoteOn(5);  
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

  AudioNoInterrupts();  
  envelope->noteOff();  
  playMem->stop(); 
  AudioInterrupts(); 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) noteOff ch(%d) note(%d) vel(%d)\n", 
    l_device, channel, note, velocity);
  Serial.print(str_);
#endif


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
  sprintf(str_, "Sampler(%s) openSample(%s) size(%ld) p_mem(%p)\n", l_device, s, size, m_mem);
  Serial.print(str_);
#endif

 wav2m(&dataFile, (unsigned int *)m_mem);
 
 dataFile.close();

}

void audioDeviceSampler::set_volume()
{
  float v1 = m_ch1_vol * m_master_vol;
  float v2 = m_ch2_vol * m_master_vol;

  for(int i=0; i<4; i++){
    outMixerCH2->gain(i, v1);
    outMixerCH1->gain(i, v2);
  }

}

void audioDeviceSampler::setVolume(float v)
{

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setVolume(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif

  //normalize
  m_master_vol = v/100.;
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
  AudioNoInterrupts();  
  envelope->sustain(v/100.);
  AudioInterrupts(); 

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setSustain(%8.2f)\n", l_device, v);
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

