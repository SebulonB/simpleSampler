#include <stdint.h>
#include <math.h>
#include <string.h>

#include "led.h"

//#define DEBUG_LED


enum LED_STATE led_state          = LED_STATE_STARTUP;
enum LED_STATE led_state_finished = LED_STATE_NONE;


//
//blink sequences
#define MAX_LED_DURATION 16
#define LED_STD_FREQ   200 //ms
#define LED_CLOCK_FREQ  50 //0ms

#define LED_STARTUP_FREQ 100
uint8_t led_seq_startup[MAX_LED_DURATION] = {12,   35,  80, 255,     0, 255,   0, 255,   0, 255,   0,   0,   0, 255,   0,   255};
uint8_t led_seq_1[MAX_LED_DURATION]       = { 35,  80,  35,   0,   255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,     0};
uint8_t led_seq_2[MAX_LED_DURATION]       = { 35,  80,  35,   0,   255,   0, 255,   0,   0,   0,   0,   0,   0,   0,   0,     0};
uint8_t led_seq_4[MAX_LED_DURATION]       = { 35,  80,  35,   0,   255, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,     0};
uint8_t led_seq_8[MAX_LED_DURATION]       = { 35,  80,  35,   0,   255, 255,   0, 255, 255,   0,   0,   0,   0,   0,   0,     0};
uint8_t led_seq_16[MAX_LED_DURATION]      = { 35,  80,  35,   0,   255, 255,   0, 255, 255,   0, 255, 255,   0, 255, 255,     0};

//globals
uint8_t  led_count = 0;
uint32_t led_next_timer = 0;

void change_state_clock_user(enum LED_USER user, enum LED_SIGNAL signal, enum LED_STATE &state)
{
  if(user != LED_USER_CLOCK) {return;}
  
  switch( signal )
  {
    case LED_SIGNAL_SET_16: state = LED_STATE_CONFIRM_16; break;
    case LED_SIGNAL_SET_8:  state = LED_STATE_CONFIRM_8;  break;
    case LED_SIGNAL_SET_4:  state = LED_STATE_CONFIRM_4;  break;
    case LED_SIGNAL_SET_2:  state = LED_STATE_CONFIRM_2;  break;
    case LED_SIGNAL_SET_1:  state = LED_STATE_CONFIRM_1;  break;    
    default: break;    
  }
}

void next_state_to_finish(enum LED_STATE finish)
{
  led_state_finished = finish;
  led_next_timer = millis() + LED_STD_FREQ; 
}

void led_init(){
  pinMode(LED_PIN,  OUTPUT);
  led_next_timer = millis();
}

void led_on(void){
  analogWrite(LED_PIN, 255);
}

void led_off(void){
  analogWrite(LED_PIN, 0);
}

void led_worker(enum LED_USER user, enum LED_SIGNAL signal)
{
  
#ifdef DEBUG_LED
  char str_[100];
  if(  !(   user   == LED_USER_MAIN 
         && signal == LED_SIGNAL_BANG))
  {
    sprintf(str_, "led state in: %d\n", led_state);
    Serial.print(str_);
  }
#endif  


  //user Main:Bang ==> led sequencer
  if(     led_next_timer > millis()
       && user   == LED_USER_MAIN 
       && signal == LED_SIGNAL_BANG )  {return;}

  //midi clock should not influence led seq
  if(    led_state != LED_STATE_CLOCK
       &&   user   == LED_USER_CLOCK
       && ( signal == LED_SIGNAL_CLOCK_IN || signal == LED_SIGNAL_CLOCK_OUT)) {return;}

  led_next_timer = millis() + LED_STARTUP_FREQ; 

  uint8_t brightness = 0;
  enum LED_STATE state_next = LED_STATE_NONE;  

  switch(led_state)
  {
    case LED_STATE_STARTUP:
      brightness = led_seq_startup[led_count];
      led_state_finished = LED_STATE_NONE;
      led_next_timer = millis() + LED_STARTUP_FREQ; 
    break;

    case LED_STATE_CLOCK:
      if(signal != LED_SIGNAL_CLOCK_IN ){
        change_state_clock_user(user, signal, state_next);
      }
      if (    user   == LED_USER_CLOCK
           && signal == LED_SIGNAL_CLOCK_IN )
      {
        brightness = 255;  
      }
      else if (    user   == LED_USER_CLOCK
                && signal == LED_SIGNAL_CLOCK_OUT )
      {
        brightness = 0;  
      }
      led_next_timer = millis() + LED_STD_FREQ; 
  
    break;

    case LED_STATE_CONFIRM_16:
      change_state_clock_user(user, signal, state_next);
      next_state_to_finish(LED_STATE_CLOCK);
      brightness = led_seq_16[led_count];
    break;

    case LED_STATE_CONFIRM_8:
      change_state_clock_user(user, signal, state_next);
      next_state_to_finish(LED_STATE_CLOCK);
      brightness = led_seq_8[led_count]; 
    break;

    case LED_STATE_CONFIRM_4:
      change_state_clock_user(user, signal, state_next);
      next_state_to_finish(LED_STATE_CLOCK);
      brightness = led_seq_4[led_count]; 
    break;

    case LED_STATE_CONFIRM_2:
      change_state_clock_user(user, signal, state_next);
      next_state_to_finish(LED_STATE_CLOCK);
      brightness = led_seq_2[led_count];
    break;

    case LED_STATE_CONFIRM_1:
      change_state_clock_user(user, signal, state_next);
      next_state_to_finish(LED_STATE_CLOCK);
      brightness = led_seq_1[led_count]; 
    break;

    default:// LED_STATE_NONE,    
      change_state_clock_user(user, signal, state_next);
      brightness = 0;
    break;
  }

  if(     state_next != LED_STATE_NONE
      &&  state_next != led_state )
  {
    led_count=0; 
    led_state = state_next;
#ifdef DEBUG_LED
  sprintf(str_, "led changed state: %d\n", led_state);
  Serial.print(str_);
#endif    
  }

  else if(led_count++ >= MAX_LED_DURATION){
    led_count=0;    
    //brightness=0;
#ifdef DEBUG_LED
    sprintf(str_, "led end of seq: %d\n", led_state);
    Serial.print(str_);
#endif   

    led_state = led_state_finished;
  }

  analogWrite(LED_PIN, brightness);

#ifdef DEBUG_LED
  if(  !(   user   == LED_USER_MAIN 
         && signal == LED_SIGNAL_BANG))
  {
    sprintf(str_, "led state out: %d\n", led_state);
    Serial.print(str_);
  }
#endif

}




