#ifndef device_param_h_
#define device_param_h_

#define DEBUG_DEVICE_PARAM

#define MAX_MOD 6

class deviceParam 
{
  public:
    deviceParam();

    uint8_t getModIndex();
    void    removeModIndex(uint8_t i);

    float   getVal(void);
    void    setParam(float v);
    void    setMod(uint8_t i, float v);

  private:
    const uint8_t max_mod{MAX_MOD};
    float m_param{0.0};
    float m_mod[MAX_MOD];

    uint8_t m_free{0};

#ifdef DEBUG_DEVICE_PARAM
    char str_[100];
#endif

};

#endif /*device_param_h_*/
