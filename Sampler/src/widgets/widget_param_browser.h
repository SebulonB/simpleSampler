#ifndef widget_param_browser_h_
#define widget_param_browser_h_
#include "widget.h"
#include "widget_param.h"

//#define DEBUG_WIDGET_PARAM_BROWSER

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

#endif /*widget_param_h_*/

