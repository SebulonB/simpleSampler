#ifndef widget_param_h_
#define widget_param_h_
#include "widget.h"
#include "widget_label.h"


class widgetParam : public widget {
  
  public:
    enum PARAM_TYPE{
      FLOAT,
      INT,
      BROWSER,
      LIST,
    };
  
    void setEdit(bool e);
    void incValue(bool inc);

    const char * getLDevice();
    const char * getLParam();
    void triggerCallback();

    bool isParamType(enum PARAM_TYPE t);

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

#endif /*widget_param_h_*/

