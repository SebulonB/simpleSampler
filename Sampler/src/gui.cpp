#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>

#include "gui.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define OLED_DC     37
#define OLED_CS     10
#define OLED_RESET  38
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                          &SPI, OLED_DC, OLED_RESET, OLED_CS);

  

void gui_init(void)
{
   //SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setRotation(2);
  display.clearDisplay();  

}



void gui_display_bpm(float bpm)
{
  char str_[30];
  display.clearDisplay(); 
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  sprintf(str_, "bpm: %7.3f", bpm);
  display.print(str_);
  display.display();
}