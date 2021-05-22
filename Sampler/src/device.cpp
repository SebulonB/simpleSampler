#include <math.h>
#include <string.h>

#include "device.h"


float getPot(byte ch)
{
  float vol = 0.0;
  
  if      (ch == 0)
  {
    vol = analogRead(15);
    vol = map(vol, 0, 1024, 0., 1.);
  }
  else if (ch == 1)
  {
    vol = analogRead(14);
    vol = map(vol, 0, 1024, 0., 1.);
  }
  else if (ch == 2)
  {
    vol = analogRead(16);
    vol = map(vol, 0, 1024, 0., 1.);
  }
  else if (ch == 3)
  {
    vol = analogRead(17);
    vol = map(vol, 0, 1024, 1., 0.);
  }

  return vol;
}
