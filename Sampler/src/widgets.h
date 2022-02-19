#ifndef WIDGETS_H_
#define WIDGETS_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>

#include "encoderKnobs.h"




#define WIDGET_INDICATOR_MAX_ENTRIES 20
#define DEBUG_WIDGET_INDICATOR 
#define DEBUG_WIDGET_LABEL
#define DEBUG_WIDGET_LIST



class widget {

  public:
    void getPos(uint16_t &x, uint16_t &y);
    void setPos(uint16_t x,  uint16_t y);

    void getSize(uint16_t &w, uint16_t &h);
    void setSize(uint16_t w,  uint16_t h);

    void forceDraw(void){draw();}    
    void setActive(bool b){m_active=b;} 

  protected:
    Adafruit_SSD1306 *m_display{NULL};
    uint16_t    m_pos_x{0};
    uint16_t    m_pos_y{0};
    uint16_t    m_width{0};
    uint16_t    m_height{0};    

    bool        m_active{false};

    virtual void draw();

};


class widgetIndicator : public widget{
  
  public:
    widgetIndicator(Adafruit_SSD1306 *disp, const char *l, unsigned int max);
    ~widgetIndicator(){};

    void setIndex(unsigned int x);

  private:
    char m_index_list[WIDGET_INDICATOR_MAX_ENTRIES];
    unsigned int m_test_s{3};
    unsigned int m_max{0};
    unsigned int m_index{0};

#ifdef DEBUG_WIDGET_INDICATOR
    char str_[100];
#endif

    void draw(void);
};



class widgetLabel : public widget{

  public:
    widgetLabel(Adafruit_SSD1306 *disp, const char *label);
    widgetLabel(Adafruit_SSD1306 *disp, const __FlashStringHelper *label);
    widgetLabel(Adafruit_SSD1306 *disp, const char *label, uint16_t x, uint16_t y);
    widgetLabel(Adafruit_SSD1306 *disp, const __FlashStringHelper *label, uint16_t x, uint16_t y);

    void setText(const char *label);

  private:   
    const char * m_label{NULL};
    uint8_t      m_text_size{1};

#ifdef DEBUG_WIDGET_LABEL
    char str_[100];
#endif

    void init(void);
    void draw(void);        

};




class widgetList : public widget {

  public:
    widgetList( Adafruit_SSD1306 *disp, 
                const uint16_t x, const uint16_t y, 
                const uint16_t width, const uint16_t height);

    void pushWidget(widget *w);

    bool setIndex(u_int16_t index);
    uint16_t getIndex(void)   {return m_index;};
    uint16_t getIndexMax(void){return m_widgets.size();}


  private:
    std::vector<widget *> m_widgets;

    uint16_t m_seperator{3};
    uint16_t m_index{0};

    void draw();

#ifdef DEBUG_WIDGET_LIST
    char str_[100];
#endif


};


#endif /* WIDGETS_H_ */
