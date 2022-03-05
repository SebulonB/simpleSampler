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

#include "audio_engine/audio_engine.h"
#include "midi_handler.h"
#include "device.h"
#include "led.h"
#include "clock.h"
#include "gui.h"
#include "encoderKnobs.h"
#include "tinyalloc/tinyalloc.h"


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

audioEngine engine;


//****************************************************************
//                       | External RAM |
//
EXTMEM uint8_t ext_ram[1]; //Just to get EXTMEM pointer
extern uint8_t external_psram_size; //in MB. Set in startup.c

void init_memory()
{
    uint32_t psram_bytes = 1024 * 1024 * external_psram_size;
    void * m = (void *)(ext_ram);
    
    if(psram_bytes <= 0){
      psram_bytes = 1024*128; //128 kb
      m = (void *)malloc(psram_bytes);
    }

    ta_init((void *)(m),               //Base of heap
            (void *)(m + psram_bytes), //End of heap
            psram_bytes / 32768,             //Number of memory chunks (32k/per chunk)
            16,                              //Smaller chunks than this won't split
            32);                             //32 word size alignment
}



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
  
  //
  init_memory();

  //Midi
  MidiHandlerSetAudioEngine(&engine);   
  
  delay(1000);
  gui_init(&engine);  

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
