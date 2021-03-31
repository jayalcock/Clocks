#ifndef comms_h
#define comms_h

#include <cstring>
#include "stdio.h"

class UART{  
  int *data;
  const char* m_ssid, *m_password; 

public:
  UART();
  UART(const char* ssid,const char* password);
  void printData();
  ~UART();
};
    



#endif