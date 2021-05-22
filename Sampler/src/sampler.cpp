#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <MIDI.h>
#include <Bounce.h>

#include <math.h>
#include <string.h>
#include <stdint.h>

#include "sampler.h"
#include "midi_handler.h"
#include "device.h"

#include "handler.h"
#include "samples/samples.h"
#include "wav2mem.h"
#include "bufferManager.h"


//
// Buffer Manager
#define EXTMEM_SIZE 4000000
EXTMEM unsigned int samples_mem[EXTMEM_SIZE]; //4M Words
bufferManager ramManager(samples_mem, EXTMEM_SIZE);
patchHandler  configHandler;
char str_to_char_p[100];


//****************************************************************
//                         | Patch GUI |
//

// GUItool: begin automatically generated code
AudioPlayMemory          playMem2;       //xy=568.8833312988281,1493.8833312988281
AudioPlayMemory          playMem1;       //xy=571.88330078125,1450.88330078125
AudioPlaySdWav           playSdSec;      //xy=584.88330078125,851.9999923706055
AudioPlaySdWav           playSdDrum;     //xy=589.88330078125,464.99999237060547
AudioMixer4              mixerSec;       //xy=768.88330078125,863.9999923706055
AudioMixer4              mixerDrum;      //xy=772.88330078125,479.99999237060547
AudioAmplifier           amp_mem_2;           //xy=915.88330078125,1490.88330078125
AudioAmplifier           amp_mem_1;           //xy=917.88330078125,1449.88330078125
AudioMixer4              delay_feedback; //xy=930.88330078125,959.9999923706055
AudioEffectDelay         delaySec;       //xy=950.88330078125,1137.9999923706055
AudioEffectEnvelope      envelopeDrum;   //xy=985.88330078125,479.99999237060547
AudioMixer4              delaySecDryWet; //xy=1208.88330078125,875.9999923706055
AudioMixer4              mixerSecondary;         //xy=1434.88330078125,894.88330078125
AudioMixer4              MainLeft;         //xy=1596,529
AudioMixer4              MainRight;         //xy=1597,692
AudioOutputI2S           i2s1;           //xy=1770.88330078125,586.0000305175781
AudioConnection          patchCord1(playMem2, amp_mem_2);
AudioConnection          patchCord2(playMem1, amp_mem_1);
AudioConnection          patchCord3(playSdSec, 0, mixerSec, 0);
AudioConnection          patchCord4(playSdSec, 1, mixerSec, 1);
AudioConnection          patchCord5(playSdDrum, 0, mixerDrum, 0);
AudioConnection          patchCord6(playSdDrum, 1, mixerDrum, 1);
AudioConnection          patchCord7(mixerSec, 0, delaySecDryWet, 0);
AudioConnection          patchCord8(mixerSec, 0, delay_feedback, 0);
AudioConnection          patchCord9(mixerDrum, envelopeDrum);
AudioConnection          patchCord10(amp_mem_2, 0, mixerSecondary, 2);
AudioConnection          patchCord11(amp_mem_1, 0, mixerSecondary, 1);
AudioConnection          patchCord12(delay_feedback, delaySec);
AudioConnection          patchCord13(delaySec, 0, delay_feedback, 1);
AudioConnection          patchCord14(delaySec, 0, delaySecDryWet, 1);
AudioConnection          patchCord15(envelopeDrum, 0, MainLeft, 0);
AudioConnection          patchCord16(envelopeDrum, 0, MainRight, 0);
AudioConnection          patchCord17(delaySecDryWet, 0, mixerSecondary, 0);
AudioConnection          patchCord18(mixerSecondary, 0, MainLeft, 1);
AudioConnection          patchCord19(mixerSecondary, 0, MainRight, 1);
AudioConnection          patchCord20(MainLeft, 0, i2s1, 0);
AudioConnection          patchCord21(MainRight, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=652.88330078125,273.99999237060547
// GUItool: end automatically generated code





//#define DEBUG_SAMPLER
// #define DEBUG_SAMPLER_NOTE
// #define DEBUG_SAMPLER_EFFEKT

void debug_mem(unsigned int *p, unsigned int size){
  char str_[20];  
  for(int i=0; i<size; i++){
    if(i%8 == 0 && i!=0){Serial.print("\n");}
    sprintf(str_, "0x%08X,", p[i]);
    Serial.print(str_);     
  }
  Serial.print("\n");  
}

void samplerInit(void)
{
  sgtl5000_1.enable();  //enable Audio Shield
  sgtl5000_1.volume(0.65);
  sgtl5000_1.lineOutLevel(14); //set 2.98V pp

  //Init Mixers
  for(int i=0; i<4; i++){
    mixerDrum.gain(i, 0.5);
    mixerSec.gain(i, 0.5);
    mixerSecondary.gain(i, 1.0);
    
    MainLeft.gain(i, 1.0);
    MainRight.gain(i, 1.0);
  }

  //Init delay
  delaySec.delay(0, 60);
  delay_feedback.gain(0, 1.0);   //no feedback
  delay_feedback.gain(1, 0.0);
  delaySecDryWet.gain(0, 1.0);   //full dry
  delaySecDryWet.gain(1, 0.0);       

  
  //Fill Buffer
  unsigned int *mem = ramManager.getNextPointer();
  if(mem != NULL){
    for(int i=0; i<10689; i++){
      mem[i] = AudioSampleKick16_b[i];    
    }
    ramManager.allocate(10689);
  }
  
  
  String param;
  if(configHandler.getParamValue( "sampler_1",  "file", param ))
  {
    param.toCharArray(str_to_char_p, param.length() + 1);
    File dataFile = SD.open(str_to_char_p); 
    ramManager.allocate( wav2m(&dataFile, ramManager.getNextPointer() ));
  }

 
#ifdef DEBUG_SAMPLER  
  Serial.print("Init Sampler\n");
#endif  
}


void playSec(const char *filename, byte velocity)
{
 
  float vol  = velocity/127. * 0.635; // 1.0 at 100velocity value (stereo ==> 0.5)
  mixerSec.gain(0, vol);
  mixerSec.gain(1, vol);
    
  playSdSec.stop();
  playSdSec.play(filename);

#if defined(DEBUG_SAMPLER) && defined(DEBUG_SAMPLER_NOTE)
  Serial.print("Playing Sec file: ");
  Serial.println(filename);
#endif  

}

void playDrum(const char *filename, byte velocity)
{
#if defined(DEBUG_SAMPLER) && defined(DEBUG_SAMPLER_NOTE)
  char str_[100];
#endif  
  
  AudioNoInterrupts(); 
  mixerDrum.gain(0, 0);
  mixerDrum.gain(1, 0);  

  playSdDrum.stop(); 
  envelopeDrum.noteOff();
  //set level/gain/velocity
  float vol  = velocity/127. * 0.635;
  mixerDrum.gain(0, vol);
  mixerDrum.gain(1, vol);  

  //attack
  float att  = getPot(1);
  att = att * att * att * 3000.;
  envelopeDrum.attack(att);

  //hold
  float hol  = getPot(3);
  hol = hol * hol * hol * 3000;
  envelopeDrum.hold(hol);

  //decay
  float de   = getPot(2);
  de = de * de * de * 6000;

  envelopeDrum.decay(de);
  if(de >= 5500){
    envelopeDrum.sustain(0.9);  
  }
  else{
    envelopeDrum.sustain(0.0); 
  }

  envelopeDrum.noteOn(); 
  AudioInterrupts();   
  //play file

  playSdDrum.play(filename);

#if defined(DEBUG_SAMPLER) && defined(DEBUG_SAMPLER_NOTE)
  sprintf(str_, "Play Drum: velocity(%5.3f)  attack(%6.1f)  hold(%6.1f)  release(%6.1f) | ", vol*2., att, hol, de);
  Serial.print(str_);  
  Serial.println(filename);
#endif

}


void samplerSetParam(enum SAMPLER_EFFEKT effekt, uint8_t param, uint16_t val)
{
#if defined(DEBUG_SAMPLER) && defined(DEBUG_SAMPLER_EFFEKT)
  char str_[100];
  sprintf(str_, "sampler set param:  effekt(%d) | param(%2d) value(%3d) \n", effekt, param, val);
  Serial.print(str_);    
#endif  
  
  float val_n = val/127.;
  if(val == 1){val_n = 0.0;}

  if(effekt == SAMPLER_EFFEKT_DELAY)
  {
    if(     param == 0){
      delaySec.delay(0, val_n*val_n*1000);
    }
    else if(param == 1)
    {
      delay_feedback.gain(0, 1.0);
      delay_feedback.gain(1, val_n*0.75);
    }
    else if(param == 2)
    {
      delaySecDryWet.gain(0, 1.0-val_n);
      delaySecDryWet.gain(1, val_n);
    }       
  }
}


void playSecMem(int ch, uint16_t val )
{

  float val_n = val/127.;
  if(val == 1){val_n = 0.0;}

  AudioNoInterrupts(); 
  unsigned int * p = ramManager.getPointerFromIndex(ch);  
  if(p != NULL){
    amp_mem_1.gain(val_n);
    playMem1.play(p); 
  }
  AudioInterrupts(); 
}

