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

#include "sampler.h"
#include "midi_handler.h"
#include "device.h"
#include "led.h"
#include "clock.h"
#include "gui.h"
#include "encoderKnobs.h"
#include "wav2mem.h"

//****************************************************************
//                           | Config |
//
// Use these with the Teensy 3.5 & 3.6 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);





//****************************************************************
//                         | Globals |
//
char str_[100];

uint32_t ui_timer = millis();
uint32_t ui_timer500 = millis();

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

  gui_state_machine();   
  
  //
  //50 millisecond 
  if(millis() >= ui_timer)
  {
    ui_timer = millis() + 10;
 
  }     

  //
  //500ms 
  if(millis() >= ui_timer500){
    ui_timer500 = millis() + 500;

  }


}
