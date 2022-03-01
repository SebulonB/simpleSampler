#ifndef WIDGETS_H_
#define WIDGETS_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

#include <String.h>
#include <functional>

#include "encoderKnobs.h"

//global strings
const char str_unknown[] PROGMEM = "unknown";


#define WIDGET_INDICATOR_MAX_ENTRIES 20


// #define DEBUG_WIDGET_INDICATOR 
// #define DEBUG_WIDGET_LABEL
//#define DEBUG_WIDGET_LIST
// #define DEBUG_WIDGET_PARAM_FLOAT
//#define DEBUG_WIDGET_PARAM_BROWSER




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

    void forceDraw(void){draw();}    
    void setActive(bool b){m_active=b;} 

    bool isWidgetType(enum WIDGET_TYPE t){
      return (t == m_widget_type) ? true : false;
    }

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


class widgetIndicator : public widget{
  
  public:
    widgetIndicator(Adafruit_SSD1306 *disp, const char *l, unsigned int max);
    ~widgetIndicator(){};

    void setIndex(unsigned int x);
    unsigned int getIndex(){return m_index;}

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
    uint8_t getTextSize(){return m_text_size;}

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

class widgetParam : public widget {
  
  public:
    enum PARAM_TYPE{
      FLOAT,
      BROWSER
    };
  
    void setEdit(bool e){set_edit(e);}
    void incValue(bool inc);

    const char * getLDevice(){return l_device;}
    const char * getLParam(){return l_param;}
    void triggerCallback(){callback();}

    bool isParamType(enum PARAM_TYPE t){
      return (t == m_param_type) ? true : false;
    }

  protected:
    bool     m_edit{false};  
    widgetLabel *m_label;
    
    const char * l_device{NULL};
    const char * l_param{NULL};
    
    uint16_t  m_value_x_pos{0};

    uint32_t m_millis_old;
    uint32_t m_millis_diff;
    uint16_t m_millis_diff_shure;
    
    enum    PARAM_TYPE m_param_type{FLOAT};

    virtual void inc_value(bool inc);
    virtual void set_edit(bool e);

    virtual void callback();

};

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


class widgetParamBrowser: public widgetParam{

  public:
    enum DEFINES{
      MAX_BROWSER_VALUES    = 3000,
      MAX_VALUE_STRING_SIZE = 10,
    };

    widgetParamBrowser( Adafruit_SSD1306 *disp, 
                        const char *device,
                        const __FlashStringHelper *label, 
                        const __FlashStringHelper *param,
                        uint16_t val_x_pos );  

    void  setUpdateCallback(std::function <void (const char*)> funcp);

    char *  getValue(){return m_value;}
    void    setValue(String &m){
      strcpy(m_value, m.c_str());
    }

    void    setDir(const __FlashStringHelper *dir){
      m_dir = reinterpret_cast<const char *>(dir); 
    }
    void    setDir(const char * dir)
    {
      m_dir = dir;
    }

  private:
    uint16_t m_index{0};
    uint16_t m_index_old{0};
    char     m_value[MAX_VALUE_STRING_SIZE];
    const char * m_dir;
    File     m_folder;

    uint16_t  m_value_width{0};
    uint16_t  m_value_width_max{0}; //used to clear txt    
    uint16_t  m_value_text_size{0};    

    //Input Stream Expansion
    std::function <void (const char *val)>p_udpateEngine_callback {nullptr};    
    void callback();    

    void set_edit(bool e);

    void init();
    void inc_value(bool inc);
    void draw();

    void browse();
    


#ifdef DEBUG_WIDGET_PARAM_BROWSER
    char str_[100];
#endif    
    
};



#endif /* WIDGETS_H_ */
