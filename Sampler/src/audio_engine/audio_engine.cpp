#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <MIDI.h>

#include "audio_engine.h"
#include "audio_device.h"
#include "audio_device_sampler.h"


void audioEngine::midiNoteOn(byte channel, byte note, byte velocity)
{
  for(int i=0; i<DEVICE_NUM; i++){
    m_devices[i]->midiNoteOn(channel, note, velocity);
  }
}

void audioEngine::midiNoteOff(byte channel, byte note, byte velocity)
{
  for(int i=0; i<DEVICE_NUM; i++){
    m_devices[i]->midiNoteOff(channel, note, velocity);
  }
}


audioEngine::audioEngine()
{

#ifdef DEBUG_AUDIO_ENGINE
  sprintf(str_, "init audio engine\n");
  Serial.print(str_);
#endif

  //Create Samplers
  m_devices[DEVICE_SAMPLER_1] = new audioDeviceSampler(F("Sampler_1"));
  m_devices[DEVICE_SAMPLER_2] = new audioDeviceSampler(F("Sampler_2"));
  m_devices[DEVICE_SAMPLER_3] = new audioDeviceSampler(F("Sampler_3"));
  m_devices[DEVICE_SAMPLER_4] = new audioDeviceSampler(F("Sampler_4"));
  m_devices[DEVICE_SAMPLER_5] = new audioDeviceSampler(F("Sampler_5"));
  m_devices[DEVICE_SAMPLER_6] = new audioDeviceSampler(F("Sampler_6"));      

}


audioDevice * audioEngine::get_device(enum AUDIO_ENGINE_DEVICES d)
{
  uint32_t i = static_cast<uint32_t>(d);
  if(i >= DEVICE_NUM){return nullptr;}

  return m_devices[(i)];
}

