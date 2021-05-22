
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "clock.h"
#include "led.h"


//#define DEBUG_CLOCK
//#define CLOCK_PRINT_BPM

enum CLOCK_STATE clock_state        = CLOCK_STATE_NONE;
enum CLOCK_STATE clock_state_next   = CLOCK_STATE_NONE;

uint32_t clock_divider_cnt = 0;
float    clock_bpm         = 0;
uint32_t previousClockTime = 0;


void clock_init(void)
{
   pinMode(CLOCK_PIN,  OUTPUT);
   clock_state = CLOCK_STATE_MIDI_1;
}


bool change_state( enum CLOCK_USER user, enum CLOCK_SIGNAL signal, 
                   enum CLOCK_STATE &state, enum CLOCK_STATE new_state )
{
  if(     user   == CLOCK_USER_BUTTON 
      &&  signal == CLOCK_SIGNAL_BUTTON_ON )
  {
    state = new_state;
    return true;
  } 
  return false;
}

uint8_t check_beat_clock_division(enum CLOCK_USER user, enum CLOCK_SIGNAL signal, uint8_t div)
{
  if(    user   == CLOCK_USER_MIDI_CLOCK
      && signal == CLOCK_SIGNAL_MIDI_CLOCK )
  {
    if( (clock_divider_cnt%div) == 0 || clock_divider_cnt == 1 )
    {
      return 1; //first of beat
    }
    else if( clock_divider_cnt >= 1 && clock_divider_cnt <= 5 )
    {
      return 2;
    }
  }
  return 0;
}

void clock_worker(enum CLOCK_USER user, enum CLOCK_SIGNAL signal)
{

#ifdef DEBUG_CLOCK
  char str_[100];
  sprintf(str_, "clock: user(%d) signal(%d) state_in(%d)\n", (uint8_t)user, (uint8_t)signal, (uint8_t)clock_state);
  Serial.print(str_);
#endif  
  uint8_t clock_out = 0;
   
  switch(clock_state)
  {
    case CLOCK_STATE_MIDI_16:
      if (change_state(user, signal, clock_state, CLOCK_STATE_MIDI_1)){
        led_worker(LED_USER_CLOCK, LED_SIGNAL_SET_1);
      }
      
      clock_out = check_beat_clock_division(user, signal, 24);
    break;

    case CLOCK_STATE_MIDI_8:
      if (change_state(user, signal, clock_state, CLOCK_STATE_MIDI_16)){
        led_worker(LED_USER_CLOCK, LED_SIGNAL_SET_16);
      }

      clock_out = check_beat_clock_division(user, signal, 24);
    break;

    case CLOCK_STATE_MIDI_4:
      if (change_state(user, signal, clock_state, CLOCK_STATE_MIDI_8)){
        led_worker(LED_USER_CLOCK, LED_SIGNAL_SET_8);
      } 
      
      clock_out = check_beat_clock_division(user, signal, 24);   
    break;

    case CLOCK_STATE_MIDI_2:
      if (change_state(user, signal, clock_state, CLOCK_STATE_MIDI_4)){
        led_worker(LED_USER_CLOCK, LED_SIGNAL_SET_4);
      }   
      clock_out = check_beat_clock_division(user, signal, 24);
    break;

    case CLOCK_STATE_MIDI_1:
      if (change_state(user, signal, clock_state, CLOCK_STATE_MIDI_2)){
        led_worker(LED_USER_CLOCK, LED_SIGNAL_SET_2);
      }  
      clock_out = check_beat_clock_division(user, signal, 24);
    break;

    case CLOCK_STATE_MIDI_8T:
      if (check_beat_clock_division(user, signal, 6)){
        clock_out = 0;
      }
    break;

    case CLOCK_STATE_TRIGGER_PRI:
      if (change_state(user, signal, clock_state, CLOCK_STATE_MIDI_1)){
        led_worker(LED_USER_CLOCK, LED_SIGNAL_SET_1);
      }   
    break;

    default: //NONE
      if (change_state(user, signal, clock_state, CLOCK_STATE_MIDI_1)){
        led_worker(LED_USER_CLOCK, LED_SIGNAL_SET_1);
      }   
    break;
   }

  // clock output
  if(clock_out == 1 || clock_out == 2){
    led_worker(LED_USER_CLOCK, LED_SIGNAL_CLOCK_IN);
  }
  else{
    led_worker(LED_USER_CLOCK, LED_SIGNAL_CLOCK_OUT);
  }
#ifdef DEBUG_CLOCK
  sprintf(str_, "clock: user(%d) signal(%d) state_out(%d)\n", (uint8_t)user, (uint8_t)signal, (uint8_t)clock_state);
  Serial.print(str_);
#endif     
}


//
//Midi Stuff
#define MIDI_CLOCK_MAX_CNT 97

void calc_bpm(void)
{
  clock_bpm = (float)(120000. / (millis() - previousClockTime)); 
#ifdef CLOCK_PRINT_BPM
  Serial.print(clock_bpm);
  Serial.println(" BPM");
#endif
  previousClockTime = millis();
}

void clock_midi_beat(void)
{
  clock_divider_cnt++;
  if(clock_divider_cnt >= MIDI_CLOCK_MAX_CNT){clock_divider_cnt=1;}

  if((clock_divider_cnt % 48) == 0){
    calc_bpm();
  }

  //clock_worker(CLOCK_USER_MIDI_CLOCK, CLOCK_USER_MIDI_CLOCK);
  uint8_t c = check_beat_clock_division(CLOCK_USER_MIDI_CLOCK, CLOCK_SIGNAL_MIDI_CLOCK, 24);
  if(c == 1){
    digitalWrite(CLOCK_PIN, HIGH);
    led_on();
  }
  else if(c == 0){
    digitalWrite(CLOCK_PIN, LOW);
    led_off();
  }
}


//midi clock cmds
void clock_midi_start(void)
{
  clock_divider_cnt = 0;
  previousClockTime = millis();
}

void clock_midi_continue(void)
{
  previousClockTime = millis();
}

void clock_midi_stop(void)
{

}


float clock_get_bpm(void)
{
  return clock_bpm;
}
