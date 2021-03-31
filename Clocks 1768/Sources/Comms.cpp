#include "../Headers/comms.h"

using namespace std;

UART::UART(){
  data = new int(0);
}

UART::UART(const char* ssid, const char* password){
  m_ssid = new char;
  m_password = new char; 

  m_ssid = ssid;
  m_password = password;


}


void UART::printData(){
  
  printf("%s\n", m_ssid);
  printf("%s\n", m_password);
  
}

UART::~UART(){
  delete m_ssid;
  delete m_password;

}




