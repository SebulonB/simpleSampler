/*

███████╗ █████╗ ███╗   ███╗██████╗ ██╗     ███████╗██████╗    
██╔════╝██╔══██╗████╗ ████║██╔══██╗██║     ██╔════╝██╔══██╗   
███████╗███████║██╔████╔██║██████╔╝██║     █████╗  ██████╔╝   
╚════██║██╔══██║██║╚██╔╝██║██╔═══╝ ██║     ██╔══╝  ██╔══██╗   
███████║██║  ██║██║ ╚═╝ ██║██║     ███████╗███████╗██║  ██║██╗
╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝     ╚══════╝╚══════╝╚═╝  ╚═╝╚═╝
                                                       
 */                                                        

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <MIDI.h>
#include <Bounce.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "USBHost_t36.h"

#include <math.h>
#include <string.h>

#include "src/sampler.h"
#include "src/midi_handler.h"
#include "src/device.h"
#include "src/led.h"
#include "src/clock.h"
#include "src/gui.h"
#include "src/encoderKnobs.h"
#include "src/wav2mem.h"

//****************************************************************
//                           | Config |
//
// Use these with the Teensy 3.5 & 3.6 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD

#define BUTTON            34
#define BUTTON_ENCODER    30
#define KNOB_CNT          1

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);





//****************************************************************
//                         | Globals |
//
char str_[100];
Bounce button_bouncer = Bounce( BUTTON, 20); 
Bounce button_bouncer_encoder = Bounce( BUTTON_ENCODER , 20); 

bool   button_read = false;
bool   button_encoder_read = false;

uint8_t knob_pins[2*KNOB_CNT] = {
  31, 32, // row 1
};
EncoderKnopGroup *p_knobs; //      = new EncoderKnopGroup(KNOB_CNT, knob_pins);

uint32_t ui_timer = millis();

USBHost myusb;
USBHub hub1(myusb);
MIDIDevice midi_host(myusb);

//****************************************************************
//                         | INIT |
//
void setup() {
  Serial.begin(115200);

  //Init Audio
  AudioMemory(512);

  //Init Midi USB
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleControlChange(myControlChange);
  usbMIDI.setHandleClock(myClock);
  usbMIDI.setHandleStart(myStart);  
  usbMIDI.setHandleStop(myStop);
  usbMIDI.setHandleContinue(myContinue);  

  //init MIDI HW
  MIDI.setHandleNoteOn(myNoteOn);
  MIDI.setHandleNoteOff(myNoteOff);
  MIDI.setHandleControlChange(myControlChange);
  MIDI.setHandleClock(myClock);
  MIDI.setHandleStart(myStart);  
  MIDI.setHandleStop(myStop);
  MIDI.setHandleContinue(myContinue);  
  MIDI.begin(MIDI_CHANNEL_OMNI);

  //Init Midi Host
  myusb.begin();
  midi_host.setHandleNoteOff(hostNoteOn);
  midi_host.setHandleNoteOn(hostNoteOff);
  midi_host.setHandleControlChange(hostControlChange);

  //Init SD-Card
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }  
  }

  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(BUTTON_ENCODER,INPUT_PULLUP);

  p_knobs = new EncoderKnopGroup(KNOB_CNT, knob_pins);
  
  delay(1000);
  gui_init();  

  samplerInit();
  led_init();
  clock_init();


}


void loop() {

  usbMIDI.read();
  MIDI.read();

  //host usb
  myusb.Task();
  midi_host.read();  

  button_bouncer.update();
  button_bouncer_encoder.update();
  if(      button_bouncer.read() == LOW 
        && button_read == false) 
  {
    clock_worker(CLOCK_USER_BUTTON, CLOCK_SIGNAL_BUTTON_ON);
    button_read = true;
  } 
  else if( button_bouncer.read() == HIGH )
  {
    button_read = false;
  }

  if(      button_bouncer_encoder.read() == LOW
        && button_encoder_read == false )
  {
    Serial.print("Hi Sebi\n");
    button_encoder_read = true;
  }
  else if(button_bouncer_encoder.read() == HIGH )
  {
    button_encoder_read = false;
  }

  if(millis() >= ui_timer)
  {
    ui_timer = millis() + 50;

    uint8_t n=p_knobs->getNumber();
    for(int i=0; i<n; i++)
    {
      //rotary
      if(p_knobs->updated(i))
      { 
        float  val = p_knobs->read(i);
        sprintf(str_, "Knob: %6.3f\n", val);
        
        Serial.print(str_); 
      }
    }

    //update display
    gui_display_bpm(clock_get_bpm());
  }     

  //led_worker(LED_USER_MAIN, LED_SIGNAL_BANG);
}
