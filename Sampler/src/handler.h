#ifndef patchhandler_handler_h_
#define patchhandler_handler_h_

#include <ArduinoJson.h>
#include <SD.h>

#define DEBUG_PATCH_HANDLER

class patchHandler {

  public:
    patchHandler();
    ~patchHandler(){};

    bool getParamValue(const char *l_device, const char *l_param, float &val);
    bool getParamValue(const char *l_device, const char *l_param, String &val);

    bool saveParamValue(const char *l_device, const char *l_param, float val);
    void saveWriteHandler(void);

  private:
    StaticJsonDocument<4096> m_doc_write;
    StaticJsonDocument<4096> m_doc_read;
    File m_file_write;
    File m_file_read;

#ifdef DEBUG_PATCH_HANDLER
    char str_[100];
#endif
};


#endif /*patchhandler_handler_h_*/
