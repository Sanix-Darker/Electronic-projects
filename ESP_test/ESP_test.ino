#include <SoftwareSerial.h>
SoftwareSerial esp8266(2, 3); //RX,TX

void setup()
{

  Serial.begin(9600);
  esp8266.begin(9600);
  esp8266.print("AT\r\n");

}

void loop() {
  
  esp8266.print("AT\r\n");
  String response = "";
  while (esp8266.available())     {
    char c = esp8266.read();
    response += c;
  }

    if (response != "") {
      Serial.println("Response: " + response);
    }
    else{
      Serial.println("Echec");
      Serial.println(esp8266.read());
    }
    delay(1000);
}
