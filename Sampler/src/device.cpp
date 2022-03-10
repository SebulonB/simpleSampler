#include <math.h>
#include <string.h>

#include "device.h"
#include "misc.h"


static float val[3];
static float val_sum[3];
static bool  updated[3] = {true};
static uint32_t mean_cnt = 0;

#define DIFF_VAL_UPDATED 0.008
#define POT_MEAN_VAL     1000

float getPot(byte ch)
{
  float vol = 0.0;
  
  if      (ch == 0)
  {
    vol = analogRead(14);
    vol = map(vol, 0, 1024, 0., 1.);
  }
  else if (ch == 1)
  {
    vol = analogRead(17);
    vol = map(vol, 0, 1024, 1., 0.);
  }
  else if (ch == 2)
  {
    vol = analogRead(16);
    vol = map(vol, 0, 1024, 0., 1.);
  }

  return vol;
}

void checkPotVal()
{
  for(int i=0; i<3; i++){
    val_sum[i] += getPot(i);
  }

  if(mean_cnt++ <= POT_MEAN_VAL ){return;}
  mean_cnt = 0;

  for(int i=0; i<3; i++){
    float v = val_sum[i] / POT_MEAN_VAL;
    val_sum[i] = 0;
    float d = (v-val[i]);
    d = (d>0.0) ? d : -d;
    if(d > DIFF_VAL_UPDATED){
      val[i] = v;
      updated[i] = true;
    } 
  } 
}

float getPotVal(byte ch){
  if(ch>=3){return 0.0;}
  return val[ch];
}

bool getPotUpdated(byte ch){
  if(ch>=3){return false;}
  bool u=updated[ch];
  updated[ch]=false;
  return u;
}
