#ifndef widget_param_list_h_
#define widget_param_list_h_
#include "widget.h"
#include "widget_param.h"
#include "widget_list.h"


class widgetParamList : public widgetParam {

  public:
    //widgetParamInt(Adafruit_SSD1306 *disp, const char *label, uint16_t val_x_pos, enum UNIT u);
    widgetParamList(  Adafruit_SSD1306 *disp, 
                      const char *device,
                      const __FlashStringHelper *label, 
                      const __FlashStringHelper *param,
                      uint16_t val_x_pos, const char * s[], uint32_t n); 

    widgetParamList(  Adafruit_SSD1306 *disp, 
                  const char *device,
                  const __FlashStringHelper *label, 
                  const __FlashStringHelper *param,
                  uint16_t val_x_pos);                         

    //API
    void  setUpdateCallback(std::function <void (int)> funcp);

    void  setValueDefault(uint32_t m);
    void  useDefaultVal();
    void  setOffset(uint32_t m);

    void  pushString(const char *str);

    void       setValue(uint32_t m);
    uint32_t   getValue(void);
    uint32_t   getMax(void);

  private:

    uint32_t m_min{0};
    uint32_t m_max{0};
    uint32_t m_value{0};
    uint32_t m_value_default{0};
    uint32_t m_offset{0};

    std::vector<const char *> m_str_l;
    bool     m_init_str_l{false};

    uint16_t  m_value_text_size{0};

    uint16_t  m_value_width{0};
    uint16_t  m_value_width_max{0}; //used to clear txt

    //Input Stream Expansion
    std::function <void (int val)>p_udpateEngine_callback {nullptr};    
    void callback();    

    void set_edit(bool e){m_edit = e;}
    void init();

    void inc_value(bool inc);
    void draw();



#ifdef DEBUG_WIDGET_PARAM_INT
    char str_[100];
#endif    

};


#endif /*widget_param_h_*/
