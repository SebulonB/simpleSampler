#ifndef GUI_DEVICE_H_
#define GUI_DEVICE_H_
#include "widgets.h"
#include <vector>

#define DEBUG_GUI_DEVICE_SAMPLER



class guiDevice : public widget
{
  public:
    void incRotary(bool inc){
      inc_rotary(inc);
    }
    void setEdit(){
      m_edit = !m_edit;
      set_edit();
    }
    void setEdit(bool en){
      m_edit = en;
      set_edit();
    }

  protected:
    bool m_edit{false};
    u_int16_t m_param_y_pos{0};

    widgetList *m_param_list{NULL};
    std::vector<widgetParam *> m_params{NULL}; 

    void inc_rotary(bool inc);
    void set_edit();
    void draw();

#ifdef DEBUG_GUI_DEVICE_SAMPLER
    char str_[100];
#endif

};







//------ guiDeviceSampler -------
class guiDeviceSampler : public guiDevice
{
  public:
    guiDeviceSampler( Adafruit_SSD1306 *disp, 
                      const uint16_t x, const uint16_t y, 
                      const uint16_t width, const uint16_t height);

};

#endif /*GUI_DEVICE_H_*/
