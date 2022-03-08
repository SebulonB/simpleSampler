#ifndef widget_label_h_
#define widget_label_h_
#include "widget.h"


// #define DEBUG_WIDGET_LABEL


class widgetLabel : public widget{

  public:
    widgetLabel(Adafruit_SSD1306 *disp, const char *label);
    widgetLabel(Adafruit_SSD1306 *disp, const __FlashStringHelper *label);
    widgetLabel(Adafruit_SSD1306 *disp, const char *label, uint16_t x, uint16_t y);
    widgetLabel(Adafruit_SSD1306 *disp, const __FlashStringHelper *label, uint16_t x, uint16_t y);

    void setText(const char *label);
    uint8_t getTextSize();

    const char * getText();

  private:   
    const char * m_label{NULL};
    uint8_t      m_text_size{1};

#ifdef DEBUG_WIDGET_LABEL
    char str_[100];
#endif

    void init(void);
    void draw(void);        

};

#endif /*widget_label_h_*/
