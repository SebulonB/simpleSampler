#ifndef widget_list_h_
#define widget_list_h_
#include "widget.h"


//#define DEBUG_WIDGET_LIST

class widgetList : public widget {

  public:
    widgetList( Adafruit_SSD1306 *disp, 
                const uint16_t x, const uint16_t y, 
                const uint16_t width, const uint16_t height);

    void pushWidget(widget *w);

    bool setIndex(u_int16_t index);
    bool incIndex(bool inc);

    uint16_t getIndex(void)   {return m_index;};
    uint16_t getIndexMax(void){return m_widgets.size();}
    widget   *getActiveWidget();


  private:
    std::vector<widget *> m_widgets;

    uint16_t m_seperator{2};
    uint16_t m_index{0};
    char     m_value[10];

    void draw();

#ifdef DEBUG_WIDGET_LIST
    char str_[100];
#endif


};

#endif /*widget_list_h_*/

