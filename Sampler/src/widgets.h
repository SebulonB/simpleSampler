#ifndef WIDGETS_H_
#define WIDGETS_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>

#include "encoderKnobs.h"

class widget {
  protected:
    Adafruit_SSD1306 *m_display{NULL};

};



#define WIDGET_INDICATOR_MAX_ENTRIES 20
// #define DEBUG_WIDGET_INDICATOR 
// #define DEBUG_WIDGET_LABEL

class widgetIndicator : widget{
  
  public:
    widgetIndicator(Adafruit_SSD1306 *disp, const char *l, unsigned int max);
    ~widgetIndicator(){};

    void forceDraw(void){draw();}
    void setActive(bool a);
    void setIndex(unsigned int x);
    void setXY(unsigned int x, unsigned int y);  
    void setSize(unsigned int w, unsigned int h, unsigned int text);

  private:
    char m_index_list[WIDGET_INDICATOR_MAX_ENTRIES];

    int          m_width{25};
    int          m_hight{32}; 
    unsigned int m_pos_x{0};
    unsigned int m_pos_y{0};
    unsigned int m_test_s{3};

    unsigned int m_max{0};
    unsigned int m_index{0};
    bool m_active{false};

#ifdef DEBUG_WIDGET_INDICATOR
    char str_[100];
#endif

    void draw(void);
};



class widgetLabel : widget{

  public:
    widgetLabel(Adafruit_SSD1306 *disp, const char *label, uint16_t x, uint16_t y);

    void forceDraw(void){draw();}
    void setActive(bool a);
    void setText(const char *label);


  private:   
    const char *m_label{NULL};
    uint8_t     m_text_size{1};
    bool        m_active{false};

    uint16_t    m_pos_x;
    uint32_t    m_pos_y;

    uint16_t    m_width;
    uint16_t    m_hight;

#ifdef DEBUG_WIDGET_LABEL
    char str_[100];
#endif

    void init(void);
    void draw(void);        

};


class widgetPage : widget {
  public:
    widgetPage(Adafruit_SSD1306 *disp, const char *label);

    void forceDraw(void){draw();}
    void setActive(bool b){m_active=b;}

    void setButtonIndex(uint16_t t);
    uint16_t getButtonIndex(void){return m_button_index;}
    
    void pushButton(widgetLabel *p){m_buttons.push_back(p);}
    void pushLabel(widgetLabel *p) {m_labels.push_back(p);}

    void signalRotary(EncoderKnopGroup *p_knobs);
    void signalButton(uint8_t button, bool pressed);
    

  private:
    const char *m_name{NULL};

    uint16_t m_button_index;
    std::vector<widgetLabel *> m_buttons;
    std::vector<widgetLabel *> m_labels;    

    bool     m_active{false};   
    bool     m_active_display{false};

#ifdef DEBUG_WIDGET_PAGE
    char str_[100];
#endif

    void init();
    void draw();
    
};


#endif /* WIDGETS_H_ */
