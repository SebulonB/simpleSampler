
#include <math.h>
#include <string.h>

#include "audio_engine/audio_engine.h"
#include "midi_handler.h"
#include "clock.h"

//#define DEBUG_MIDI


audioEngine* m_engine = nullptr;



void MidiHandlerSetAudioEngine(audioEngine *e)
{
  m_engine = e;
}

//****************************************************************
//                         | Midi Calbacks |
//
void myControlChange(byte channel, byte control, byte value) {
  if(m_engine == nullptr){return;}
  m_engine->midiCC(channel, control, value);
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

  if(m_engine == nullptr){return;}
  m_engine->midiNoteOn(channel, note, velocity);

}

void myNoteOff(byte channel, byte note, byte velocity) {
#ifdef DEBUG_MIDI  
  char str_[100];
  sprintf(str_, "Note Off,   ch(%2d) note(%3d) veloctiy(%3d)\n", channel, note, velocity);
  Serial.print(str_);
#endif

  if(m_engine == nullptr){return;}
  m_engine->midiNoteOff(channel, note, velocity);

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

