#ifndef widget_param_float_h_
#define widget_param_float_h_
#include "widget.h"
#include "widget_param.h"


class widgetParamFloat : public widgetParam {

  public:
    enum UNIT{
      SECONDS,
      PERCENT
    };

    //widgetParamFloat(Adafruit_SSD1306 *disp, const char *label, uint16_t val_x_pos, enum UNIT u);
    widgetParamFloat( Adafruit_SSD1306 *disp, 
                      const char *device,
                      const __FlashStringHelper *label, 
                      const __FlashStringHelper *param,
                      uint16_t val_x_pos, enum UNIT u );    
  
    //API
    void  setUpdateCallback(std::function <void (float)> funcp);
    enum UNIT getUnit(){return m_unit;}
    void  setValueDefault(float m){m_value_default = m;}
    void  useDefaultVal(){setValue(m_value_default);}
    void  setValue(float m);
    float getValue(void){return m_value;}
    void  setMin(float m){m_min = m;}
    float getMin(void){return m_min;}
    void  setMax(float m){m_max = m;}
    float getMax(void){return m_max;}

  private:
    float m_min{0.};
    float m_max{0.};
    float m_value{0.};
    float m_value_default{0.};

    enum UNIT m_unit{PERCENT};
    uint16_t  m_value_text_size{0};

    uint16_t  m_value_width{0};
    uint16_t  m_value_width_max{0}; //used to clear txt

    //Input Stream Expansion
    std::function <void (float val)>p_udpateEngine_callback {nullptr};    
    void callback();    

    void set_edit(bool e){m_edit = e;}
    void init();
    uint16_t cal_val_str(char* str);
    void inc_value(bool inc);
    void draw();



#ifdef DEBUG_WIDGET_PARAM_FLOAT
    char str_[100];
#endif    

};

#endif /*widget_param_h_*/
