#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <MIDI.h>

#include "misc.h"
#include "audio_device_strings.h"
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

  //reset mod midi
  for(int i=0; i<MOD_CC_NUM; i++){
    m_cc_dest[i] = MOD_DEST_NONE; //turn off
    m_cc_mod_slot[i] = 0xff; //reset
  }

  //reset mod pot
  for(int i=0; i<MOD_POT_NUM; i++){
    m_pot_dest[i] = MOD_DEST_NONE; //turn off
    m_pot_mod_slot[i] = 0xff; //reset
  }


  //set label names
  m_param_labels[MOD_DEST_NONE]   = str_device_param_none;
  m_param_labels[MOD_DEST_VOLUME] = str_device_param_volume;  
  m_param_labels[MOD_DEST_PITCH]  = str_device_param_start;    
  m_param_labels[MOD_DEST_START]  = str_device_param_pitch;  

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Indit Audio Device Sampler (%s)\n", l_device);
  Serial.print(str_);
#endif

}


//
//   ---------{ Midi Parser }------------
//
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

  if( m_cc_dest[i] == MOD_DEST_NONE ){return;}

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) midiCC ch(%d) cc(%d) val(%d)\n", 
    l_device, channel, control, value);
  Serial.print(str_);
#endif
   
  float val = mapf(value, 0, MIDI_VAL_MAX, m_cc_mod_min[i], m_cc_mod_max[i]); 
  m_params[m_cc_dest[i]].setMod(m_cc_mod_slot[i], val);

  switch(m_cc_dest[i]){
    case MOD_DEST_VOLUME:
      set_volume();
    break;
  }
  
}

//
//   ---------{ Midi Config }------------
//
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

//
//   ---------{ Modulation Midi }--------------
//
const char *audioDeviceSampler::getParamString(mod_dest_t d){
  return m_param_labels[d];
}

void audioDeviceSampler::modCCregParam( uint8_t index, uint8_t d)
{
  
  if(index >=  MOD_CC_NUM){return;}
  if(d >= MOD_DEST_NUM){return;}

  mod_dest_t dest = static_cast<mod_dest_t>(d);

  if(m_cc_dest[index] >= MOD_DEST_NUM){
    m_cc_dest[index] = MOD_DEST_NONE;
  }

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) CCx(%d) dest(%d) regCCParam(%s) unreg(%s)\n", 
    l_device, index, dest, m_param_labels[dest], m_param_labels[m_cc_dest[index]]); //] );
  Serial.print(str_);
#endif

  //unreg
  m_params[m_cc_dest[index]].removeModIndex(m_cc_mod_slot[index]);      

  // //reg
  m_cc_dest[index] = dest; 
  m_cc_mod_slot[index] = m_params[dest].getModIndex();      

}

void audioDeviceSampler::modCCsetCH(uint8_t index, uint8_t ch)
{
  if(index >=  MOD_CC_NUM){return;}
  if(ch >= MIDI_VAL_MAX){return;}

  m_cc_src[index] = ch;

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  if(m_cc_dest[index]>=MOD_DEST_NUM){return;}
  sprintf(str_, "Sampler(%s) CCx(%d) setCHParam(%s) ch(%d)\n", 
    l_device, index, m_param_labels[m_cc_dest[index]], ch);
  Serial.print(str_);
#endif


}

void audioDeviceSampler::modCCsetMax(uint8_t index, float v)
{
  if(index >=  MOD_CC_NUM){return;}
  v /= 100.;
  if     (v >  2.0){v = 2.0;}
  else if(v < -2.0){v = 2.0;}

  m_cc_mod_max[index] = v;  

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  if(m_cc_dest[index]>=MOD_DEST_NUM){return;}
  sprintf(str_, "Sampler(%s) CCx(%d) setMax(%s) val(%6.3f)\n", 
    l_device, index, m_param_labels[m_cc_dest[index]], v);
  Serial.print(str_);
#endif
}

void audioDeviceSampler::modCCsetMin(uint8_t index, float v)
{
  if(index >=  MOD_CC_NUM){return;}
  v /= 100.;
  if     (v >  2.0){v = 2.0;}
  else if(v < -2.0){v = 2.0;}

  m_cc_mod_min[index] = v;  

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  if(m_cc_dest[index]>=MOD_DEST_NUM){return;}
  sprintf(str_, "Sampler(%s) CCx(%d) setMin(%s) val(%6.3f)\n", 
    l_device, index, m_param_labels[m_cc_dest[index]], v);
  Serial.print(str_);
#endif
}


//
//   ---------{ Modulation Midi }--------------
//
void audioDeviceSampler::setPot(byte channel, float v)
{

  byte i = channel;
  if(i>=MOD_POT_NUM){
    return;
  }

  if( m_pot_dest[i] == MOD_DEST_NONE ){return;}


   
  float val = mapf(v, 0.0, 1.0, m_pot_mod_min[i], m_pot_mod_max[i]); 
  m_params[m_pot_dest[i]].setMod(m_pot_mod_slot[i], val);

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) Pot(%d) val(%6.3f) mapped(%6.3f) min(%6.3f) max(%6.3f)\n", 
    l_device, channel, v, val, m_pot_mod_min[i], m_pot_mod_max[i]);
  Serial.print(str_);
#endif

  switch(m_pot_dest[i]){
    case MOD_DEST_VOLUME:
      set_volume();
    break;
  }
  
}

void audioDeviceSampler::modPotregParam( uint8_t index, uint8_t d)
{
  
  if(index >=  MOD_POT_NUM){return;}
  if(d >= MOD_DEST_NUM){return;}

  mod_dest_t dest = static_cast<mod_dest_t>(d);

  if(m_pot_dest[index] >= MOD_DEST_NUM){
    m_pot_dest[index] = MOD_DEST_NONE;
  }

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) Pot(%d) dest(%d) regPotParam(%s) unreg(%s)\n", 
    l_device, index, dest, m_param_labels[dest], m_param_labels[m_pot_dest[index]]); //] );
  Serial.print(str_);
#endif

  //unreg
  m_params[m_pot_dest[index]].removeModIndex(m_pot_mod_slot[index]);      

  // //reg
  m_pot_dest[index] = dest; 
  m_pot_mod_slot[index] = m_params[dest].getModIndex();      

}

void audioDeviceSampler::modPotsetMax(uint8_t index, float v)
{
  if(index >=  MOD_CC_NUM){return;}
  v /= 100.;
  if     (v >  2.0){v = 2.0;}
  else if(v < -2.0){v = 2.0;}

  m_pot_mod_max[index] = v;  

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  if(m_pot_dest[index]>=MOD_DEST_NUM){return;}
  sprintf(str_, "Sampler(%s) Pot(%d) setMax(%s) val(%6.3f)\n", 
    l_device, index, m_param_labels[m_pot_dest[index]], v);
  Serial.print(str_);
#endif
}

void audioDeviceSampler::modPotsetMin(uint8_t index, float v)
{
  if(index >=  MOD_POT_NUM){return;}
  v /= 100.;
  if     (v >  2.0){v = 2.0;}
  else if(v < -2.0){v = 2.0;}

  m_pot_mod_min[index] = v;  

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  if(m_pot_dest[index]>=MOD_DEST_NUM){return;}
  sprintf(str_, "Sampler(%s) Pot(%d) setMin(%s) val(%6.3f)\n", 
    l_device, index, m_param_labels[m_pot_dest[index]], v);
  Serial.print(str_);
#endif
}


//
//   ---------{ FileHandling }--------------
//
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



//
//   ---------{ Param Volume }--------------
//
void audioDeviceSampler::setVolume(float v)
{

#ifdef DEBUG_AUDIO_DEVICE_SAMPLER
  sprintf(str_, "Sampler(%s) setVolume(%8.2f)\n", l_device, v);
  Serial.print(str_);
#endif

  //normalize
  m_params[MOD_DEST_VOLUME].setParam(v/100.);
  set_volume();
}

void audioDeviceSampler::set_volume()
{
  float m  = m_params[MOD_DEST_VOLUME].getVal();
  float v1 = m_ch1_vol * m * m_velocity;
  float v2 = m_ch2_vol * m * m_velocity;


  AudioNoInterrupts();  
  for(int i=0; i<4; i++){
    outMixerCH2->gain(i, v1);
    outMixerCH1->gain(i, v2);
  }
  AudioInterrupts(); 
}


//
//   ---------{ Param Paning }--------------
//
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


//
//   ---------{ Params Main }--------------
//
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


//
//   ---------{ Envelop }--------------
//
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




