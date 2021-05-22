#ifndef MIDI_HANDLER_H_
#define MIDI_HANDLER_H_
#include "WProgram.h"

void myNoteOn(byte channel, byte note, byte velocity);
void myNoteOff(byte channel, byte note, byte velocity);
void myControlChange(byte channel, byte control, byte value);

void hostNoteOn(byte channel, byte note, byte velocity);
void hostNoteOff(byte channel, byte note, byte velocity);
void hostControlChange(byte channel, byte control, byte value);

void myClock(); 
void myStart();
void myContinue(); 
void myStop();

#endif /*MIDI_HANDER_H_*/
