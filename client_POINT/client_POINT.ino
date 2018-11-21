/* 
 * MONITO Cerveau V1.0 
 */
#include <SoftwareSerial.h>
#define DEBUG true

    const int sensorIn = A0;
    int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
    double Voltage = 0;
    double VRMS = 0;
    double AmpsRMS = 0;
    double p=0;
    double Cout=0;
    double PrixKW=79;


SoftwareSerial esp8266(10,11); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

// Pour que le module ESp se rappel du point d'access AT+CWQAP
// Pour retourner le firmware(information du firmware) du module, c'est: AT+GMR
// Pour metttre le module en Veille AT+GSLP=1000 // Avec 1000 pour 1s
// Pour lister les Point d'access AT+CWLAP
// Pour se connecter a un point d'accesss: AT+CWJAP="SSID","PASS"
// Pour se deconnecter du Point d'access: AT+CWQAP
// POur set l'adressse IP AT+CIPSTA="192.168.0.101", Chercher pour AT+CIPAP
String SSID_="MONITO";
String PASSWORD_="123456789ABCdef";
String ADRESSEIP_="192.168.7.1";
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  esp8266.begin(115200);
  sendData("AT+CIOBAUD=9600\r\n",4000,DEBUG); // reset module(RRESET ou reinitialisation)
  esp8266.begin(9600); // your esp's baud rate might be different
  while(1){initESP8266();}
}

void loop() {
  // put your main code here, to run repeatedly:
  printResponse();
}
void printResponse() {
  while (esp8266.available()) {
    Serial.println(esp8266.readStringUntil('\n')); 
  }
}
  void initESP8266()
  {  
    Serial.println("************* INITIALISATION MONITO CAPTEUR **************"); 
    sendData("AT+RST\r\n",2000,DEBUG);
    Serial.println("**********************************************************");
    sendData("AT+CWMODE=3\r\n",5000,DEBUG);
    Serial.println("**********************************************************");
    sendData("AT+CWJAP=\""+SSID_+"\",\""+PASSWORD_+"\"\r\n",3000,DEBUG); sendData("AT+CWJAP?\r\n",3000,DEBUG);
    Serial.println("**********************************************************");
    sendData("AT+CIFSR\r\n",1000,DEBUG);
    Serial.println("**********************************************************");
    sendData("AT+CIPMUX=1\r\n",1000,DEBUG);
    Serial.println("**********************************************************");
    sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG);

    Serial.println("");
  }

  void printMesure()
  {
    // Pour la mesure!!!!!!!!!!!!!!!!!!!!!
    Serial.println("Mesure en cours");
    digitalWrite(LED_BUILTIN, HIGH);
    p=(p+getPower(getVPP())); Cout+=((PrixKW*p)/3600);
    Serial.print("->P: "); Serial.print((p/3600),7); Serial.print("KWh et Vous depenssez\t  ->Facture: "); Serial.print(Cout,7); Serial.println(" FCFA ");
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
  }

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    if(debug)
    {
      Serial.print(response);
    }
    return response;
}

//Tension de voltage
    float getVPP()
    {
      float result;
      
      int readValue;             
      int maxValue = 0;          
      int minValue = 1024;          
      
       uint32_t start_time = millis();
       while((millis()-start_time) < 1000) //Pour 1 Seconde
       {
           readValue = analogRead(sensorIn);
           if (readValue > maxValue){
               maxValue = readValue;
           }
           if (readValue < minValue){
               minValue = readValue;
           }
       }
       // Subtract min from max
       result = ((maxValue - minValue) * 5.0)/1024.0;
       return result;
    }

    float getPower(float v)
    {
        Voltage = v;
        VRMS = (Voltage/2.0) *0.707; 
        AmpsRMS = (VRMS * 1000)/mVperAmp;
        return (AmpsRMS*VRMS);
    }
 
