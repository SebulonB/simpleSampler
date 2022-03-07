#ifndef widget_param_list_h_
#define widget_param_list_h_
#include "widget.h"
#include "widget_param.h"
#include "widget_list.h"


class widgetParamList : public widgetParam {

  public:
    enum UNIT{
      UNKNOWN,
    };

    //widgetParamInt(Adafruit_SSD1306 *disp, const char *label, uint16_t val_x_pos, enum UNIT u);
    widgetParamList(  Adafruit_SSD1306 *disp, 
                      const char *device,
                      const __FlashStringHelper *label, 
                      const __FlashStringHelper *param,
                      uint16_t val_x_pos, enum UNIT u=UNKNOWN );    
  
    //API
    void  setUpdateCallback(std::function <void (int)> funcp);
    enum UNIT getUnit(){return m_unit;}
    void  setValueDefault(int m);
    void  useDefaultVal();
    void  setValue(int m);
    int   getValue(void);
    int   getMax(void);
    void  pushWidget(widget *w);

  private:
    std::vector<widgetList *> m_list;

    int m_min{0};
    int m_max{0};
    int m_value{0};
    int m_value_default{0};

    enum UNIT m_unit{UNKNOWN};
    uint16_t  m_value_text_size{0};

    uint16_t  m_value_width{0};
    uint16_t  m_value_width_max{0}; //used to clear txt

    //Input Stream Expansion
    std::function <void (int val)>p_udpateEngine_callback {nullptr};    
    void callback();    

    void set_edit(bool e){m_edit = e;}
    void init();
    uint16_t cal_val_str(char* str);
    void inc_value(bool inc);
    void draw();



#ifdef DEBUG_WIDGET_PARAM_INT
    char str_[100];
#endif    

};


#endif /*widget_param_h_*/
