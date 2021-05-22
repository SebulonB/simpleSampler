
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
   
  }

  else if( channel == 3)
  {
    //play file
    playSecMem(note - 52, velocity);
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

