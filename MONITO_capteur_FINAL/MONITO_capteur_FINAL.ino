    /*
      Capteur MONITO V1.0
    */
    #include <SoftwareSerial.h>
    SoftwareSerial esp8266(10,11);
    #define DEBUG true
    
    const int sensorIn = A0;
    int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
    double Voltage = 0, VRMS = 0, AmpsRMS = 0, p=0, Cout=0, PrixKW=79;

    String SSID_="capteurM", PASSWORD_="123456789ABCdef1";

    String ssidCERVO="MONITO", passCERVO="123456789ABCdef";
    
    //String ADRESSEIP_="192.168.7.1";
    String readString, Contenu="Nothing";
    
    void setup()
    { 
      pinMode(LED_BUILTIN, OUTPUT);
      Serial.begin(9600);
      esp8266.begin(9600);
      //sendData("AT+CIOBAUD=9600\r\n",4000,DEBUG); // reset module(RRESET ou reinitialisation)
      //esp8266.begin(9600); // your esp's baud rate might be different
      
      sendData("AT+RST\r\n",2000,DEBUG); // reset module(RRESET ou reinitialisation)
      //Definition des parametres du point d'acces
      sendData("AT+CWSAP=\""+SSID_+"\",\""+PASSWORD_+"\",3,3\r\n",2000,DEBUG); sendData("AT+CWSAP?\r\n",1000,DEBUG); // SSID, mot de passe, niveau de securite puis Verification
      
      //sendData("AT+CIPSTA="+ADRESSEIP_+"\r\n",1000,DEBUG); //On fixe l'adresse IP: du point d'access
      //Configuration du mode DHCP
      //sendData("AT+CWDHCP=2,0\r\n",1000,DEBUG); sendData("AT+CWDHCP?\r\n",1000,DEBUG);//0 pour DHCP, 1 pour juste une station et 2 pour jouer les 2 roles
      
      sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point (on configure en tant que point d'access)
      sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address (Recuperation de l'adresse IP)
      sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
      sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80

      //===============================================================================
      // To close the server the command is AT+CIPSERVER=0 followed by a Reset AT+RST
      //===============================================================================
      
      if(!connectToWiFi(ssidCERVO, passCERVO)) {connectToWiFi(ssidCERVO, passCERVO);} // Connexion au reseau WIFI du cerveau
      
    }
    /****************************************************************/
    /*                        BOUCLE INFINIE                        */
    /****************************************************************/
    void loop()
    {
      ListClients();
      sendHTTPRequest("?command=off HTTP/1.1", "192.168.4.1");
      if(esp8266.available()) // check if the esp is sending a message
      { 
        //if(!connectToWiFi(ssidCERVO, passCERVO)) {connectToWiFi(ssidCERVO, passCERVO);} // Connexion au reseau WIFI du cerveau
        if(esp8266.find("+IPD,")) //Reception d'une requette
        {
         delay(1000);
         Serial.println("\n*=======================================*\nUne Connexion detectee!\n*=======================================*");
         int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                               // the ASCII decimal value and 0 (the first decimal number) starts at 48
         showParameters();
         sendHTTPResponse(connectionId, Contenu);
         closeCIP(connectionId);
        }
      }
    }


  /* 
   *  Name: printMesure
   *  Description: To print Mesure
   */
  void printMesure()
  {
    // Pour la mesure!!!!!!!!!!!!!!!!!!!!!
    Serial.println("Mesure en cours");
    p=(p+getPower(getVPP())); Cout+=((PrixKW*p)/3600);
    Serial.print("->P: "); Serial.print((p/3600),7); Serial.print("KWh et Vous depenssez\t  ->Facture: "); Serial.print(Cout,7); Serial.println(" FCFA ");
  }

  /* 
   *  Name: ListClients
   *  Description: Pour le listing des clients
   */
  void ListClients(){
    // listing des Clients
    Serial.println("Listing des Clients...");
    sendData("AT+CWLIF\r\n",1500,DEBUG);  
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
  * Name: sendHTTPRequest
  * Description: To send an http request to a peer in the network
  * Request, String
  */
  void sendHTTPRequest(String Request, String IP) { // Request: update?key=P55Z0BDASNQBFQKT&field1=15&field2=25
    
    if(!connectToWiFi(ssidCERVO, passCERVO)) {connectToWiFi(ssidCERVO, passCERVO);}
    
    String cmd = "AT+CIPSTART='TCP','";// Setup TCP connection
    cmd += IP;
    cmd += "',80\r\n";
    sendData(cmd,2000,DEBUG);

    Serial.println("Sending data to Cervo...");
    
    String url = "GET /";
    url += Request;
    url += " \r\n\r\n"; //hard code value 15 and 25

//    String stringLength="AT+CIPSEND=";
//    stringLength +=String(url.length());
//    Serial.println(stringLength);
    
    //Cipsend command
    int connectionId = esp8266.read()-48;
    sendCIPData(connectionId, url);

    delay(10000); //I even tried to delay for few seconds

    sendData("AT+CIPSEND?\r\n",1000,DEBUG);
  
   //Here is where it fails,it jumps to closing the connection
   if(esp8266.find(">"))
    {
     Serial.print(cmd);
     Serial.println("BINGOO!!");
     sendData(cmd ,2100,DEBUG);
    }
    else
    {
      Serial.print(cmd);
      Serial.println("LOSE!!");
      sendData("AT+CIPCLOSE\r\n" ,2000,DEBUG); //close TCP connection
    }
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
    while(readString.length() < 30) {
      char c = esp8266.read();
      readString += c;
    }
    Serial.println("\n-----------------------------------");
    Serial.println(readString);
  
//    if(ifParameter(readString, "command"))
//    {
      someValue=getValueOfParameter(readString, "command", 8);
      if(someValue=="on"){
        digitalWrite(LED_BUILTIN, HIGH);
      }else{
        digitalWrite(LED_BUILTIN, LOW);
      } 
      Contenu="Commande=";Contenu+=someValue;
      Serial.print("Commande: "); Serial.println(someValue);
    //}
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
  * Name: connectToWiFi
  * Description: To connecct to a network
  * ssid_, pass_
  */
  boolean connectToWiFi(String ssid_, String pass_) {
    Serial.println("Etablishing connexion to Network...");
    
    // sendData( "AT+CWMODE=1\r\n" ,2000,DEBUG); 1(Mode ST, se comporte comme un client), 2(Mode AP se comporte comme un access point) et 3( Les 2 modes, ST et AP)
    
    String cmd="AT+CWJAP=\""; // Join accespoint
    cmd+=ssid_;
    cmd+="\",\"";
    cmd+=pass_;
    cmd+="\"\r\n";
    sendData( cmd ,5000,DEBUG);
    
    Serial.println("\nConnexion en cours....");
    if(esp8266.find("OK"))
    {
      Serial.println("RECEIVED: OK");
      return true;
    }
    else
    {
      Serial.println("RECEIVED: Error");
      return false;
    }
  }


  

  float getPower(float v)
  {
      Voltage = v;
      VRMS = (Voltage/2.0) *0.707; 
      AmpsRMS = (VRMS * 1000)/mVperAmp;
      return (AmpsRMS*VRMS);
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
