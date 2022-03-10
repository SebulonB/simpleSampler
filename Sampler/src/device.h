#ifndef DEVICE_H_
#define DEVICE_H_
#include "WProgram.h"

float getPot(byte ch);

void checkPotVal();
float getPotVal(byte ch);
bool getPotUpdated(byte ch);

#endif /*DEVICE_H_*/
