#ifndef CLOCK_LED_
#define CLOCK_LED_
#include "WProgram.h"

#define CLOCK_PIN 35

enum CLOCK_STATE
{
  CLOCK_STATE_NONE = 0,

  CLOCK_STATE_MIDI_16,
  CLOCK_STATE_MIDI_8,
  CLOCK_STATE_MIDI_4,
  CLOCK_STATE_MIDI_2,
  CLOCK_STATE_MIDI_1,
  CLOCK_STATE_MIDI_8T,

  CLOCK_STATE_TRIGGER_PRI,

  CLOCK_STATE_NUM,
};

enum CLOCK_USER
{
  CLOCK_USER_MAIN = 0,
  CLOCK_USER_BUTTON,
  CLOCK_USER_MIDI_CLOCK,
  CLOCK_USER_NUM,
};

enum CLOCK_SIGNAL
{
  CLOCK_SIGNAL_BANG=0,
  CLOCK_SIGNAL_BUTTON_ON,
  CLOCK_SIGNAL_MIDI_CLOCK,
  CLOCK_SIGNAL_MIDI_STOP,
  CLOCK_SIGNAL_NUM,
};

void clock_init(void);
void clock_worker(enum CLOCK_USER user, enum CLOCK_SIGNAL signal);

void clock_midi_beat(void);
void clock_midi_start(void);
void clock_midi_continue(void);
void clock_midi_stop(void);

float clock_get_bpm(void);
#endif
