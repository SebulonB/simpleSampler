#ifndef widget_index_h_
#define widget_index_h_
#include "widgets.h"

//#define DEBUG_WIDGET_INDICATOR 


#define WIDGET_INDICATOR_MAX_ENTRIES 20

class widgetIndicator : public widget{
  
  public:
    widgetIndicator(Adafruit_SSD1306 *disp, const char *l, unsigned int max);
    ~widgetIndicator(){};

    bool setIndex(unsigned int x);
    unsigned int getIndex(){return m_index;}
    bool incIndex(bool inc);
    void setMax(unsigned int m){m_max = m;}
    void noteOff(uint32_t id, uint8_t note, uint8_t vel);
    void noteOn(uint32_t id, uint8_t note, uint8_t vel);

  private:
    char m_index_list[WIDGET_INDICATOR_MAX_ENTRIES];
    unsigned int m_test_s{3};
    unsigned int m_max{0};
    unsigned int m_index{0};
    bool         m_note_gate[WIDGET_INDICATOR_MAX_ENTRIES];

#ifdef DEBUG_WIDGET_INDICATOR
    char str_[100];
#endif

    void draw(void);
    void draw_note(uint8_t ch);
};


#endif /*widget_index_h_*/
