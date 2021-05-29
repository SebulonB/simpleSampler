#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <Bounce.h>

#include "gui.h"
#include "widgets.h"
#include "encoderKnobs.h"



#define GUI_DEBUG


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define BUTTON            34
#define BUTTON_ENCODER    32
#define KNOB_CNT          1

#define OLED_DC     37
#define OLED_CS     10
#define OLED_RESET  38
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                          &SPI, OLED_DC, OLED_RESET, OLED_CS);



//Buttons and Encoders
Bounce button_bouncer = Bounce( BUTTON, 20); 
Bounce button_bouncer_encoder = Bounce( BUTTON_ENCODER , 20); 

bool   button_read = false;
bool   button_encoder_read = false;

uint8_t knob_pins[2*KNOB_CNT] = {
  31, 30, // row 1
};
EncoderKnopGroup *p_knobs; //      = new EncoderKnopGroup(KNOB_CNT, knob_pins);



enum UI_KNOBS{
  UI_KNOB_BUTTON1 = 0,
  UI_KNOB_BUTTON2,
  UI_KNOB_ROTARY,
  UI_KNOB_NUM
};


//gui elements
enum GUI_STATES
{
  GUI_STATE_SAMP1 = 0,
  GUI_STATE_SAMP2,
  GUI_STATE_SAMP3,
  GUI_STATE_SAMP4,
  GUI_STATE_SAMP5,
  GUI_STATE_SAMP6,
  GUI_STATE_MAIN,
  GUI_STATE_LFO,
  // GUI_STATE_DELAY,
  // GUI_STATE_REVERB,
  GUI_STATE_CLOCK,  
  GUI_STATE_NUM,
};

const char w_index_list[GUI_STATE_NUM] PROGMEM = {'1', '2', '3', '4', '5', '6', 'M', 'L', 'C'};
const char w_param_l[]  PROGMEM = "paraM";
widgetIndicator w_index(&display, w_index_list, GUI_STATE_NUM);
widgetButton m_widbut(&display, w_param_l, 32, 0); 

enum GUI_STATES gui_state = GUI_STATE_SAMP1;
bool gui_indexing = true;


void gui_init(void)
{
   //SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setRotation(2);
  display.clearDisplay();  

  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(BUTTON_ENCODER,INPUT_PULLUP);
  p_knobs = new EncoderKnopGroup(KNOB_CNT, knob_pins);  


  w_index.forceDraw();
  w_index.setActive(true);
  // w_index.setIndex(6);

  // display.setTextColor(SSD1306_WHITE);  
  // display.setCursor(32, 0);             // Start at top-left corner
  // display.setTextSize(1);
  // display.print("params | map");    
  m_widbut.forceDraw();
  m_widbut.setActive(true);

  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(32, 12);             // Start at top-left corner
  display.setTextSize(1);
  display.print("file: KICK16");

  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(32, 24);             // Start at top-left corner
  display.setTextSize(1);
  display.print("midi: CH2 E3");  

  display.display();

}



void gui_display_bpm(float bpm)
{
}




void gui_change_state(enum UI_KNOBS knob, unsigned int val)
{
#ifdef GUI_DEBUG
  char str_[100];
  sprintf(str_, "gui set state: %d %d\n", (int)knob, val);
  Serial.print(str_);
#endif


if(gui_indexing){
  if(knob == UI_KNOB_ROTARY)
  {
    if(val>=GUI_STATE_NUM){
      val = (GUI_STATE_NUM-1);
      p_knobs->writeSingle(val, 0); 
    }
    w_index.setIndex(val);    
    gui_state = (enum GUI_STATES)val;
  }
  else if(knob == UI_KNOB_BUTTON2){
    gui_indexing = false;
    w_index.setActive(false);
    m_widbut.setActive(true);
  }
}

if(knob == UI_KNOB_BUTTON1){
  gui_indexing = true;
  w_index.setActive(true);
  m_widbut.setActive(false);  
}


//  switch(gui_state){
//   case GUI_STATE_SAMP1:
//     if()

//   break;

//   case GUI_STATE_SAMP2:

//   break;

//   case GUI_STATE_SAMP3:

//   break;

//   case GUI_STATE_SAMP4:

//   break;

//   case GUI_STATE_SAMP5:

//   break;

//   case GUI_STATE_SAMP6:

//   break;

//   case GUI_STATE_MAIN:

//   break;

//   case GUI_STATE_LFO:

//   break;

//   case GUI_STATE_DELAY:

//   break;

//   case GUI_STATE_REVERB:

//   break;

//   case GUI_STATE_CLOCK:

//   break;
  
//   default: 
//     gui_state = GUI_STATE_SAMP1;
//   break;
 
//  }
}


void gui_state_machine(void)
{

  button_bouncer.update();
  button_bouncer_encoder.update();
 
  if(      button_bouncer.read() == LOW 
        && button_read == false) 
  {
    gui_change_state(UI_KNOB_BUTTON1, 1);
    button_read = true;
  } 
  else if( button_bouncer.read() == HIGH )
  {
    button_read = false;
  }

  if(      button_bouncer_encoder.read() == LOW
        && button_encoder_read == false )
  {
    gui_change_state(UI_KNOB_BUTTON2, 1);
    button_encoder_read = true;
  }
  else if(button_bouncer_encoder.read() == HIGH )
  {  
    button_encoder_read = false;
  }
  
  uint8_t n=p_knobs->getNumber();
  for(int i=0; i<n; i++)
  {
    //rotary
    if(p_knobs->updated(i))
    { 
      gui_change_state(UI_KNOB_ROTARY, p_knobs->readSteps(i)); 
    }
  }
}



