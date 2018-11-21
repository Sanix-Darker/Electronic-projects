/* 
 * MONITO Cerveau V1.0 
 */
//include SoftwareSerial for the ESP
#include <SoftwareSerial.h>

#define DEBUG true

SoftwareSerial esp8266(10,11); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

String SSID_="capteurM", PASSWORD_="123456789";
//String ADRESSEIP_="192.168.7.1";
String readString, Contenu="Nothing";

void setup()
{
  Serial.begin(9600);
  esp8266.begin(9600);// Demarrage de l'ESP en frequence 9600
  InitESP(); // Initialisation de l'ESP
}
 
void loop()
{
  if(esp8266.available()) // check if the esp is sending a message 
  {
    if(esp8266.find("+IPD,")) //Reception d'une requette
    {
     delay(1000);
     Serial.println("\n*=======================================*\nUne Connexion detectee!\n*=======================================*");
     int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
      showParameters();
      String webpage = "<center><br><h1>- Serveur MONITO -</h1>-------------------<br>\
        Connection <b style='color:green;'>OK</b><br> Message:"+Contenu+"\
        </center>";
     
     sendHTTPResponse(connectionId, webpage);
     closeCIP(connectionId);
    }
  }
}

/* 
 *  Name: sendData
 *  Description: To send data to the ESP and get the reponse
 */
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

/*
* Name: sendHTTPResponse
* Description: Function that sends HTTP 200, HTML UTF-8 response
*/
void sendHTTPResponse(int connectionId, String content)
{
     // build HTTP response
     String httpResponse;
     String httpHeader;
     // HTTP Header
     httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n"; 
     httpHeader += "Content-Length: ";
     httpHeader += content.length();
     httpHeader += "\r\n";
     httpHeader +="Connection: close\r\n\r\n";
     httpResponse = httpHeader + content + " "; // There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
     sendCIPData(connectionId,httpResponse);
}
 
/*
* Name: sendCIPDATA
* Description: sends a CIPSEND=<connectionId>,<data> command
*
*/
void sendCIPData(int connectionId, String data)
{
   String cipSend = "AT+CIPSEND=";
   cipSend += connectionId;
   cipSend += ",";
   cipSend +=data.length();
   cipSend +="\r\n";
   sendData(cipSend,1000,DEBUG);
   sendData(data,1000,DEBUG);
}

/*
* Name: CloseCIPDATA
* Description: Close the connexion
*
*/
void closeCIP(int connectionId){
   String closeCommand = "AT+CIPCLOSE="; 
   closeCommand+=connectionId; // append connection id
   closeCommand+="\r\n";
   sendData(closeCommand,3000,DEBUG);  
}

/*
* Name: getParameter
* Description: To identify, witch parameter is send
* planing_capteur[16], command=[8]
*/
  void showParameters()
  {
    String someValue;
    Serial.println("\n===============================\nStarting Showing parameters.......\n===============================\n");
    while(readString.length() < 100) {
      char c = esp8266.read();
      readString += c;
    }
    Serial.println("\n-----------------------------------");
    Serial.println(readString);
  
    if(ifParameter(readString, "command"))
    {
      someValue=getValueOfParameter(readString, "command", 8);
      Contenu="Commande=";Contenu+=someValue;
      Serial.print("Commande: "); Serial.println(someValue);
    }
    //clearing string for next read
    readString="";
  }
  
  /*
  * Name: ifParameter
  * Description: To identify, If Yes or no a parameter exists in the query string
  * planing_capteur[16], command=[8]
  */
  bool ifParameter(String InString, String param){
    if(InString.indexOf(param) >0)
      return true; 
    else
      return false; 
  }

  /*
  * Name: findParameter
  * Description: To get the value of parameter
  * planing_capteur[16], command=[8]
  */
  String getValueOfParameter(String InString, String param, int sizeToValue){
    String result;
    for (int i=0; i<100; i++)
    {
      if(InString[InString.indexOf(param)+(sizeToValue+i)]=='&' || InString[InString.indexOf(param)+(sizeToValue+i)]==' '){
        break;
      }else{
        result+=InString[InString.indexOf(param)+(sizeToValue+i)];
      }
    }
    return result;
  }
  
  /*
  * Name: InitESP
  * Description: Initialisation de l'ESP8266
  * Nom du point d'access et du mot de passe
  */
  void InitESP(){
    // Pour que le module ESp se rappel du point d'access AT+CWQAP
    // Pour retourner le firmware(information du firmware) du module, c'est: AT+GMR
    // Pour metttre le module en Veille AT+GSLP=1000 // Avec 1000 pour 1s
    // Pour lister les Point d'access AT+CWLAP
    // Pour se connecter a un point d'accesss: AT+CWJAP="SSID","PASS"
    // Pour se deconnecter du Point d'access: AT+CWQAP
    // POur set l'adressse IP AT+CIPSTA="192.168.0.101", Chercher pour AT+CIPAP
    sendData("AT+RST\r\n",2000,DEBUG); // reset module(RRESET ou reinitialisation)
    
    //Definition des parametres du point d'acces
    sendData("AT+CWSAP=\""+SSID_+"\",\""+PASSWORD_+"\",3,3\r\n",1000,DEBUG); sendData("AT+CWSAP?\r\n",1000,DEBUG); // SSID, mot de passe, niveau de securite puis Verification
    
    //sendData("AT+CIPSTA="+ADRESSEIP_+"\r\n",1000,DEBUG); //On fixe l'adresse IP: du point d'access
    //Configuration du mode DHCP
    //sendData("AT+CWDHCP=2,0\r\n",1000,DEBUG); sendData("AT+CWDHCP?\r\n",1000,DEBUG);//0 pour DHCP, 1 pour juste une station et 2 pour jouer les 2 roles
    
    sendData("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point (on configure en tant que point d'access)
    sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address (Recuperation du mot de passe!!)
    sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
    sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
    //sendData("AT+CWLAP\r\n",1000,DEBUG);
  }
