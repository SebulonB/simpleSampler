#ifndef SAMPLER_H_
#define SAMPLER_H_
#include "WProgram.h"

enum SAMPLER_EFFEKT
{
  SAMPLER_EFFEKT_DELAY = 0,
  SAMPLER_EFFEKT_NUM
};



void samplerInit(void);

void playSec(const char *filename, byte velocity);
void playSecMem(uint8_t ch, uint16_t val );

void playDrum(const char *filename, byte velocity);

void samplerSetParam(enum SAMPLER_EFFEKT effekt, uint8_t param, uint16_t val);



#endif
