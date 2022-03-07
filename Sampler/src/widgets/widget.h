#ifndef WIDGET_H_
#define WIDGET_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

#include <String.h>
#include <functional>
#include <vector>


//global strings
const char str_unknown[] PROGMEM = "unknown";


class widget {

  public:
    enum WIDGET_TYPE{
      UNKNOWN,
      LIST,
      LABEL,
      PARAM,
      DEVICE,
    };

    void getPos(uint16_t &x, uint16_t &y);
    void setPos(uint16_t x,  uint16_t y);

    void getSize(uint16_t &w, uint16_t &h);
    void setSize(uint16_t w,  uint16_t h);

    void forceDraw(void);//
    void setActive(bool b);

    bool isWidgetType(enum WIDGET_TYPE t);
    
  protected:
    Adafruit_SSD1306 *m_display{NULL};
    uint16_t    m_pos_x{0};
    uint16_t    m_pos_y{0};
    uint16_t    m_width{0};
    uint16_t    m_height{0};    

    enum  WIDGET_TYPE m_widget_type{UNKNOWN};      

    bool        m_active{false};

    virtual void draw();

};




#endif /* WIDGETS_H_ */
