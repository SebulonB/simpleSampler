#ifndef WIDGETS_H_
#define WIDGETS_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>

#define WIDGET_INDICATOR_MAX_ENTRIES 20
// #define DEBUG_WIDGET_INDICATOR 
// #define DEBUG_WIDGET_BUTTON

class widgetIndicator{
  
  public:
    widgetIndicator(Adafruit_SSD1306 *disp, const char *l, unsigned int max){
      m_display = disp;
      if(max > WIDGET_INDICATOR_MAX_ENTRIES){max = WIDGET_INDICATOR_MAX_ENTRIES;}
      
      m_max = max;
      for(unsigned int i=0; i<max; i++){m_index_list[i]=l[i];}
    };
    ~widgetIndicator(){};

    void forceDraw(void){draw();}

    void setActive(bool a){
      m_active = a;
      draw();
    }

    void setIndex(unsigned int x){
      if(x > WIDGET_INDICATOR_MAX_ENTRIES){x = WIDGET_INDICATOR_MAX_ENTRIES;}   
      m_index = x;
      draw();
    }

    void setXY(unsigned int x, unsigned int y){
      m_pos_x = x;
      m_pos_y = y;
    }
    
    void setSize(unsigned int w, unsigned int h, unsigned int text){
      m_width = w;
      m_hight = h;
      m_test_s = text;
    }

  private:
    Adafruit_SSD1306 *m_display{NULL};
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

    void draw(void){
      if(m_display == NULL){return;}
      
#ifdef DEBUG_WIDGET_INDICATOR     
      sprintf(str_, "widgetIdikator draw: active(%s) index(%d | %c)\n", 
              (m_active?"true":"false"), m_index, m_index_list[m_index]);
      Serial.print(str_);      
#endif            

      if(m_active){
        m_display->fillRect(m_pos_x, m_pos_y, m_width, m_hight, SSD1306_WHITE);
        m_display->setTextColor(SSD1306_INVERSE);        // Draw white text
      }
      else{
        m_display->fillRect(m_pos_x, m_pos_y, m_width, m_hight, SSD1306_BLACK);
        m_display->drawRect(m_pos_x, m_pos_y, m_width, m_hight, SSD1306_WHITE);
        m_display->setTextColor(SSD1306_WHITE);        // Draw white text       
      }

      m_display->setTextSize(3);      
      m_display->setCursor(5,6);             // Start at top-left corner
      m_display->print(m_index_list[m_index]);
      m_display->display();      
    }
};



class widgetButton {

  public:
    widgetButton(Adafruit_SSD1306 *disp, const char *label, uint16_t x, uint16_t y) :
                 m_display(disp), m_label(label), m_pos_x(x), m_pos_y(y){
                   init();
                 };

    void forceDraw(void){draw();}

    void setActive(bool a){
      m_active = a;
      draw();
    }


  private:
    Adafruit_SSD1306 *m_display{NULL};
    
    const char *m_label{NULL};
    uint8_t     m_text_size{1};
    bool        m_active{false};

    uint16_t    m_pos_x;
    uint32_t    m_pos_y;

    uint16_t    m_width;
    uint16_t    m_hight;

#ifdef DEBUG_WIDGET_BUTTON
    char str_[100];
#endif

    void init(void){
      m_width = 5 * m_text_size * strlen(m_label) + strlen(m_label) + 1;
      m_hight = 8 * m_text_size;
#ifdef DEBUG_WIDGET_BUTTON
    sprintf(str_, "widgetButton Init active(%s) label(%s) size(%3d %3d)\n", 
        (m_active?"true":"false"), m_label, m_width, m_hight);
    Serial.print(str_);    
#endif      
    }

    void draw(void){
      if(m_display == NULL){return;}

#ifdef DEBUG_WIDGET_BUTTON
    sprintf(str_, "widgetButton draw active(%s) label(%s) size(%3d %3d) pos(%3d %3d)\n", 
        (m_active?"true":"false"), m_label, m_width, m_hight, m_pos_x, m_pos_y);
    Serial.print(str_);     
#endif      
      //set 
      uint16_t x = m_pos_x;
      if(x>0){x-=1;}
      
      if(m_active){
        m_display->fillRect(x, m_pos_y, m_width, m_hight, SSD1306_WHITE);
        m_display->setTextColor(SSD1306_INVERSE);        // Draw white text
      }
      else{
        m_display->fillRect(x, m_pos_y, m_width, m_hight, SSD1306_BLACK);
        m_display->setTextColor(SSD1306_WHITE);        // Draw white text       
      }

      m_display->setTextSize(m_text_size);      
      m_display->setCursor(m_pos_x, m_pos_y);             // Start at top-left corner
      m_display->print(m_label);
      m_display->display();   
    }             

};


#endif /* WIDGETS_H_ */
