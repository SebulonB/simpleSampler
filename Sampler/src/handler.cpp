#include <string.h>
#include <string>
#include <vector>

#include <SD.h>

#include <Audio.h>
#include <Wire.h>
#include <Bounce.h>
#include <ArduinoJson.h>

#include "handler.h"


//#define FILE_STR "SET1.TXT"
#define FILE_STR "SET_2.TXT"


void patchHandler::init(void)
{
#ifdef DEBUG_PATCH_HANDLER
  Serial.print("patchHandler Init\n");
#endif
  
  m_file_read = SD.open(FILE_STR, FILE_READ);

  if(!m_file_read){
#ifdef DEBUG_PATCH_HANDLER      
    Serial.print(F("could not open file\n"));
#endif
  }

  DeserializationError err = deserializeJson(m_doc_read, m_file_read);
  if (err) {
#ifdef DEBUG_PATCH_HANDLER      
    sprintf(str_, "deserializeJson() failed with code: %s\n", err.c_str());
    Serial.print(str_);
#endif    
  }  

}


bool patchHandler::getParamValue(const char *l_device, const char *l_param, float &val){

  if(!m_file_read){
#ifdef DEBUG_PATCH_HANDLER  
    sprintf(str_, "file not opened: %s | %s \n", l_device, l_param);    
    Serial.print(str_);
#endif
    return false;
  }

  float v = m_doc_read[l_device][l_param];
  
  if(v == 0.0){
    val = 0.0;

#ifdef DEBUG_PATCH_HANDLER   
    sprintf(str_, "ph: getValue (%s | %-8s): fail or 0.0\n", l_device, l_param);   
    Serial.print(str_);
#endif
    return true;
  }

  val = v;
#ifdef DEBUG_PATCH_HANDLER   
  sprintf(str_, "ph: getValue (%s | %-8s): %3.3f\n", l_device, l_param, val);   
  Serial.print(str_);
#endif
  return true;
}

bool patchHandler::getParamValue(const char *l_device, const char *l_param, String &val)
{
  if(!m_file_read){
#ifdef DEBUG_PATCH_HANDLER  
    sprintf(str_, "file not opened: %s | %s \n", l_device, l_param);    
    Serial.print(str_);
#endif
    return false;
  }

  String v = m_doc_read[l_device][l_param];
  
  if(    v.c_str() == NULL 
      || v.c_str() == nullptr 
      || (strcmp (v.c_str(), "null") == 0) )
  
  {
    return false;
  }

  val = v;
#ifdef DEBUG_PATCH_HANDLER   
  sprintf(str_, "ph: getValue (%s | %-8s): %s\n", l_device, l_param, val.c_str());   
  Serial.print(str_);
#endif
  return true;  
}

bool patchHandler::saveParamValue(const char *l_device, const char *l_param, float val)
{ 
  m_doc_write[l_device][l_param] = val;

#ifdef DEBUG_PATCH_HANDLER   
  sprintf(str_, "ph: savedParam (%s | %-8s): %3.3f\n", l_device, l_param, val);   
  Serial.print(str_);
#endif

  return true;
}

bool patchHandler::saveParamValue(const char *l_device, const char *l_param, String &val)
{
  m_doc_write[l_device][l_param] = val;

#ifdef DEBUG_PATCH_HANDLER   
  sprintf(str_, "ph: savedParam (%s | %-8s): %sf\n", l_device, l_param, val.c_str());   
  Serial.print(str_);
#endif

 return true;
}


void patchHandler::saveWriteHandler(void){
  
  SD.remove(FILE_STR);
  File _file = SD.open(FILE_STR, FILE_WRITE);
  
  if(!_file){
#ifdef DEBUG_PATCH_HANDLER     
    Serial.print("could not create\n");
#endif
    return;
  }

  if (serializeJsonPretty(m_doc_write, _file) == 0) {
#ifdef DEBUG_PATCH_HANDLER      
    Serial.println(F("Failed to write to file"));
#endif    
  }

#ifdef DEBUG_PATCH_HANDLER      
    Serial.println(F("Saved to file"));
#endif    

  _file.close();
}











/*
  auto res = toml::parse(patch_str_raumLauf);
  if (!res.table) {
#ifdef DEBUG_PATCH_HANDLER    
    Serial.print("cannot parse str\n");
#endif    
    return false;
  }

  auto delay_s = res.table->getTable(l_device);  
  if (!delay_s){
#ifdef DEBUG_PATCH_HANDLER   
    sprintf(str_, "ph: missig table: %s\n", l_device);   
    Serial.print(str_);
#endif
    return false;
  }

  auto param = delay_s->getDouble(l_param);
  if(!param.first){
#ifdef DEBUG_PATCH_HANDLER   
    sprintf(str_, "ph: missig param: %s\n", l_param);   
    Serial.print(str_);
#endif
    return false;    
  }
  val = param.second;

#ifdef DEBUG_PATCH_HANDLER   
    sprintf(str_, "ph: getValue (%s | %-6s): %3.3f\n", l_device, l_param, val);   
    Serial.print(str_);
#endif  

*/
