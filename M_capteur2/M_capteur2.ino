#include "ESP8266.h"

#define SSID        "MONITO"
#define PASSWORD    "123456789"
#define HOST_NAME   "192.168.2.10"
#define HOST_PORT   (80)

ESP8266 wifi(Serial);
char machaine[15] = "";
char id=1;

void setup(void)
{
    Serial.begin(9600);
    Serial.print("setup begin\r\n");

    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());

    if (wifi.setOprToStationSoftAP()) {
        Serial.print("to station + softap ok\r\n");
    } else {
        Serial.print("to station + softap err\r\n");
    }

    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP:");
        Serial.println( wifi.getLocalIP().c_str());       
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }
    
    Serial.print("setup end\r\n");
}
 
void loop(void)
{
    uint8_t buffer[1024] = {0};
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
    }

    sprintf(machaine, "GET /?id=%d&val=%d HTTP/1.1", id,mesure());    
    wifi.send((const uint8_t*)machaine, strlen(machaine));

    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }

    if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }
    while(1);
}

float mesure(){
  unsigned int x=0;
  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
    for (int x = 0; x <= 150; x++){
      AcsValue = analogRead(A0);
      Samples = Samples + AcsValue;
      delay (7);
    }
    AvgAcs=Samples/150.0;
    return ((AvgAcs * (5.0 / 1023.0)) )/0.066;
}
     
