#include <ESP8266.h>
#define SSID        "MONITO"
#define PASSWORD    "123456789"

ESP8266 wifi(Serial);

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
        Serial.print("Connection OK\r\n");
        Serial.print("IP: ");
        Serial.println(wifi.getLocalIP().c_str());    
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.enableMUX()) {
        Serial.print("multiple ok\r\n");
    } else {
        Serial.print("multiple err\r\n");
    }
    
    if (wifi.startTCPServer(8090)) {
        Serial.print("Demarrage du Serveur ok\r\n");
    } else {
        Serial.print("Demarrage du Serveur Error\r\n");
    }
    
    if (wifi.setTCPServerTimeout(10)){
        Serial.print("set tcp server timout 10 seconds\r\n");
    } else {
        Serial.print("set tcp server timout err\r\n");
    }
    
    Serial.print("Installation End\r\n");
}
 
void loop(void)
{
    uint8_t buffer[128] = {0};
    uint8_t mux_id;
    uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);
    if (len > 0) {
        Serial.print("Status:[");
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println("]");
        
        Serial.print("Reception de:");
        Serial.print(mux_id);
        Serial.print("[");
        for(uint32_t i = 0; i < len; i++) { //enregistrement dans la carte SD
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
        
        if(wifi.send(mux_id, buffer, len)) {
            Serial.print("Tout est ok\r\n");
        } else {
            Serial.print("Error \r\n");
        }
        
        if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp");
            Serial.print(mux_id);
            Serial.println(" err");
        }
        
        Serial.print("Status:[");
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println("]");
    }
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
        
