#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>
#include <Bounce.h>
#include <vector>

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
  // GUI_STATE_MAIN,
  // GUI_STATE_LFO,
  // GUI_STATE_CLOCK,  
  GUI_STATE_NUM,
};

enum GUI_STATES gui_state = GUI_STATE_SAMP1;
static bool gui_indexing = true;
static bool gui_edit_param = false;

const char w_index_list[] PROGMEM = {'1', '2', '3', '4', '5', '6', 'M', 'L', 'C'};
const char *w_labels[]  = {      "Attack", 
                                 "Decay", 
                                 "Sustain", 
                                 "Release", 
                                 "Volume", 
                                 "Start", 
                                 "flub(8)",  
                                 "flub(9)",
                                 "flub(10)"};

widgetIndicator w_index(&display, w_index_list, GUI_STATE_NUM);
widgetList list(&display, 32, 0, 128-32, 32);



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
  pinMode(knob_pins[0], INPUT_PULLUP);
  pinMode(knob_pins[1], INPUT_PULLUP);  
  p_knobs = new EncoderKnopGroup(KNOB_CNT, knob_pins);  

  w_index.setActive(true);
  w_index.forceDraw();


}



void gui_display_bpm(float bpm)
{
}





void gui_change_state(enum UI_KNOBS knob, int32_t val)
{
// #ifdef GUI_DEBUG
//   char str_[100];
//   sprintf(str_, "gui set state: %d %d\n", (int)knob, val);
//   Serial.print(str_);
// #endif

if(gui_indexing){
  if(knob == UI_KNOB_ROTARY)
  {
    if(val>=GUI_STATE_NUM){
      val = (GUI_STATE_NUM-1);
      p_knobs->writeSingle(val, 0); 
    }
    if(val<0){
       val = 0;
      p_knobs->writeSingle(val, 0);      
    }
    w_index.setIndex(val);    
    gui_state = (enum GUI_STATES)val;
  }
  else if(knob == UI_KNOB_BUTTON2){
    gui_indexing = false;
    w_index.setActive(false);
    w_index.forceDraw();
    //reset
    display.fillRect(32, 0, 128-32, 32, SSD1306_BLACK);

    for(int i=0; i<9; i++){
      widgetParamFloat *la= new widgetParamFloat (&display, w_labels[i], 55, widgetParamFloat::SECONDS);
      la->setMax(9000);
      la->setValue(i*10 + 25);
      list.pushWidget(la);
    }

    list.setIndex(0);  
    list.setActive(true);  
    list.forceDraw();

  }
}
else {
  if(knob == UI_KNOB_ROTARY){

    if(gui_edit_param){
      widgetParam* w = reinterpret_cast<widgetParam* >(list.getActiveWidget());
      if(w==NULL){return;}
      if(val==0){return;}
      w->incValue((val>0 ? true : false));
      w->forceDraw();
      p_knobs->writeSingle(0, 0); 
    }
    else
    {
      if(val == 0){return;}
      if(list.incIndex((val>0 ? true : false))){
        list.forceDraw();
      }
      p_knobs->writeSingle(0, 0); 
    }
  }
  else if(knob == UI_KNOB_BUTTON2)
  {
    gui_edit_param = !gui_edit_param;
    widgetParam* w = reinterpret_cast<widgetParam* >(list.getActiveWidget());
    if(w!=NULL){
      w->setEdit(gui_edit_param);
      p_knobs->writeSingle(0, 0); 
      w->forceDraw();
    }
  }
}

if(knob == UI_KNOB_BUTTON1){
  gui_indexing = true;
  w_index.setActive(true);
  w_index.forceDraw();  

  gui_edit_param = false;
  widgetParam* w = reinterpret_cast<widgetParam* >(list.getActiveWidget());
  if(w!=NULL){
    w->setEdit(gui_edit_param);
    w->forceDraw();
  }

  list.setActive(false);
  list.forceDraw();

}


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



