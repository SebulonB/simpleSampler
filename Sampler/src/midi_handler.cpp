
#include <math.h>
#include <string.h>

#include "sampler.h"
#include "midi_handler.h"
#include "clock.h"

#define DEBUG_MIDI

//****************************************************************
//                         | Midi Calbacks |
//

void myNoteOff(byte channel, byte note, byte velocity) {
//  sprintf(str_, "Note Off,  ch(%2d) note(%3d) veloctiy(%3d)\n", channel, note, velocity);
//  Serial.print(str_);
}

void myControlChange(byte channel, byte control, byte value) {
//  sprintf(str_, "Control Change,  ch(%2d) control(%3d) value(%3d)\n", channel, control, value);
//  Serial.print(str_);
}

void myClock() {
  //Serial.println("Clock");
  clock_midi_beat();
}

void myStart() {
  Serial.println("Start");
  clock_midi_start();

}

void myStop() {
  Serial.println("Stop");
  clock_midi_stop();     
}

void myContinue() {
  Serial.println("Continue");
  clock_midi_continue();  
}




void myNoteOn(byte channel, byte note, byte velocity) {

#ifdef DEBUG_MIDI  
  char str_[100];
  sprintf(str_, "Note On,   ch(%2d) note(%3d) veloctiy(%3d)\n", channel, note, velocity);
  Serial.print(str_);
#endif

  if(channel == 2)
  {
    //play file
    switch(note)
    {
      case 52:
        playDrum("PRI/1.WAV", velocity);
      break;
  
      case 53:
        playDrum("PRI/2.WAV", velocity);
      break;
  
      case 54:
        playDrum("PRI/3.WAV", velocity);
      break;
  
      case 55:
        playDrum("PRI/4.WAV", velocity);
      break;  
  
      case 56:
        playDrum("PRI/5.WAV", velocity);
      break;   
  
      case 57:
        playDrum("PRI/6.WAV", velocity);
      break;   
  
      case 58:
        playDrum("PRI/7.WAV", velocity);
      break;             
  
      case 59:
        playDrum("PRI/8.WAV", velocity);
      break;             
  
      case 60:
        playDrum("PRI/9.WAV", velocity);
      break;   
  
      case 61:
        playDrum("PRI/10.WAV", velocity);
      break;   
  
      case 62:
        playDrum("PRI/11.WAV", velocity);
      break;   
  
      case 63:
        playDrum("PRI/12.WAV", velocity);
      break;  
  
      case 64:
        playDrum("PRI/13.WAV", velocity);
      break;   
  
      case 65:
        playDrum("PRI/14.WAV", velocity);
      break;  
  
      case 66:
        playDrum("PRI/15.WAV", velocity);
      break;   
  
      case 67:
        playDrum("PRI/16.WAV", velocity);
      break;   
                  
    }
  }

  else if( channel == 3)
  {
     //play file
    if(note >= 48 && note < 52 )
    {
      samplerSetParam(SAMPLER_EFFEKT_DELAY, (note-48), velocity);
    }
    switch(note)
    {
      case 52:
        playSec("SEC/1.WAV", velocity);
      break;
  
      case 53:
        playSec("SEC/2.WAV", velocity);
      break;
  
      case 54:
        playSec("SEC/3.WAV", velocity);
      break;
  
      case 55:
        playSec("SEC/4.WAV", velocity);
      break;  
  
      case 56:
        playSec("SEC/5.WAV", velocity);
      break;   
  
      case 57:
        playSec("SEC/6.WAV", velocity);
      break;   
  
      case 58:
        playSec("SEC/7.WAV", velocity);
      break;             
  
      case 59:
        playSec("SEC/8.WAV", velocity);
      break;             
  
      case 60:
        playSec("SEC/9.WAV", velocity);
      break;   
  
      case 61:
        playSec("SEC/10.WAV", velocity);
      break;   
  
      case 62:
        playSec("SEC/11.WAV", velocity);
      break;   
  
      case 63:
        playSec("SEC/12.WAV", velocity);
      break;  
  
      case 64:
        playSec("SEC/13.WAV", velocity);
      break;   
  
      case 65:
        playSec("SEC/14.WAV", velocity);
      break;  
  
      case 66:
        playSec("SEC/15.WAV", velocity);
      break;   
  
      case 67:
        playSecMem(0, velocity);
      break;     

      case 68:
        playSecMem(1, velocity);
      break;     
    }   
  }
  
}



void hostNoteOn(byte channel, byte note, byte velocity)
{
  Serial.print("Note On, ch=");
  Serial.print(channel);
  Serial.print(", note=");
  Serial.print(note);
  Serial.print(", velocity=");
  Serial.print(velocity);
  Serial.println();
}

void hostNoteOff(byte channel, byte note, byte velocity)
{
  Serial.print("Note Off, ch=");
  Serial.print(channel);
  Serial.print(", note=");
  Serial.print(note);
  //Serial.print(", velocity=");
  //Serial.print(velocity);
  Serial.println();
}

void hostControlChange(byte channel, byte control, byte value)
{
  Serial.print("Control Change, ch=");
  Serial.print(channel);
  Serial.print(", control=");
  Serial.print(control);
  Serial.print(", value=");
  Serial.print(value);
  Serial.println();
}

