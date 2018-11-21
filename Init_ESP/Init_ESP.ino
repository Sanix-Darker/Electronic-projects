    #include <SoftwareSerial.h>
    #define DEBUG true
    
    SoftwareSerial ESP8266(10, 11);

    String NomduReseauWifi = "MONITO", MotDePasse  = "123456789ABCdef"; // Garder les guillements
    String readString, Contenu="Nothing";
    String SSID_="capteurM", PASSWORD_="123456789";
    
    /****************************************************************/
    /*                             INIT                             */
    /****************************************************************/
    void setup()
    {
      Serial.begin(9600);
      
      ESP8266.begin(115200);
      Serial.println("Initialisation de l'ESP!!!");
      envoieAuESP8266("AT+CIOBAUD=9600");
      recoitDuESP8266(4000);
      
      ESP8266.begin(9600);
      while(1){
        initESP8266();
      }
    }
    
    /****************************************************************/
    /*                        BOUCLE INFINIE                        */
    /****************************************************************/
    void loop()
    {
//       while(ESP8266.available())
//       {
//         Serial.println(ESP8266.readString());
//       }
      if(ESP8266.available()) // check if the esp is sending a message 
      {
        if(ESP8266.find("+IPD,")) //Reception d'une requette
        {
         delay(1000);
         Serial.println("\n*=======================================*\nUne Connexion detectee!\n*=======================================*");
         int connectionId = ESP8266.read()-48; // subtract 48 because the read() function returns 
                                               // the ASCII decimal value and 0 (the first decimal number) starts at 48
         showParameters();

         sendHTTPResponse(connectionId, Contenu);
         closeCIP(connectionId);
        }
      } 
    }
    
    /****************************************************************/
    /*                Fonction qui initialise l'ESP8266             */
    /****************************************************************/
    void initESP8266()
    {  
      Serial.println("**********************************************************");  
      Serial.println("**************** DEBUT DE L'INITIALISATION ***************");
      Serial.println("**********************************************************");  
      envoieAuESP8266("AT"); recoitDuESP8266(2000);
      //Serial.println("**********************************************************");
      //envoieAuESP8266("AT+GMR"); recoitDuESP8266(2000);
      Serial.println("**********************************************************");
      envoieAuESP8266("AT+CWMODE=3"); recoitDuESP8266(5000);
      Serial.println("**********************************************************");
      
      //sendData("AT+CWSAP=\""+SSID_+"\",\""+PASSWORD_+"\",3,3\r\n",1000,DEBUG); sendData("AT+CWSAP?\r\n",1000,DEBUG); // SSID, mot de passe, niveau de securite puis Verification 
      //Serial.println("**********************************************************");
      
      envoieAuESP8266("AT+CWJAP=\""+ NomduReseauWifi + "\",\"" + MotDePasse +"\""); recoitDuESP8266(10000);
      Serial.println("**********************************************************");
      
      envoieAuESP8266("AT+CIFSR"); recoitDuESP8266(1000);
      Serial.println("**********************************************************");
      envoieAuESP8266("AT+CIPMUX=1"); recoitDuESP8266(1000);
      Serial.println("**********************************************************");
      envoieAuESP8266("AT+CIPSERVER=1,80"); recoitDuESP8266(1000);
      Serial.println("**********************************************************");
      Serial.println("***************** INITIALISATION TERMINEE ****************");
      Serial.println("**********************************************************");
      Serial.println("");  
    }

    /****************************************************************/
    /*        Fonction qui envoie une commande à l'ESP8266          */
    /****************************************************************/
    void envoieAuESP8266(String commande)
    {  
      ESP8266.println(commande);
    }
    
    /****************************************************************/
    /*Fonction qui lit et affiche les messages envoyés par l'ESP8266*/
    /****************************************************************/
    void recoitDuESP8266(const int timeout)
    {
      String reponse = "";
      long int time = millis();
      while( (time+timeout) > millis())
      {
        while(ESP8266.available())
        {
          char c = ESP8266.read();
          reponse+=c;
        }
      }
      Serial.print(reponse);   
    }

/* 
 *  Name: sendData
 *  Description: To send data to the ESP and get the reponse
 */
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    ESP8266.print(command); // send the read character to the esp8266
    long int time = millis();
    while( (time+timeout) > millis())
    {
      while(ESP8266.available())
      {
        // The esp has data so display its output to the serial window 
        char c = ESP8266.read(); // read the next character.
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
      char c = ESP8266.read();
      readString += c;
    }
    Serial.println("\n-----------------------------------");
    Serial.println(readString);
  
    if(ifParameter(readString, "command"))
    {
      someValue=getValueOfParameter(readString, "command", 8);
      if(someValue=="on"){
        digitalWrite(LED_BUILTIN, HIGH);
      }else{
        digitalWrite(LED_BUILTIN, LOW);
      }
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

