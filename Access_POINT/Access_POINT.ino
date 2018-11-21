/* 
 * MONITO Cerveau V1.0 
 */
//include SoftwareSerial for the ESP
#include <SoftwareSerial.h>
// include the SD library:
#include <SPI.h>
#include <SD.h>

#define DEBUG true

SoftwareSerial esp8266(10,11); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

String SSID_="MONITO", PASSWORD_="123456789ABCdef";
//String ADRESSEIP_="192.168.7.1";
String readString, Contenu="Nothing";

File myFile;
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 4, pinCS = 53; // Pin 10 on Arduino Uno;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  esp8266.begin(9600);// Demarrage de l'ESP en frequence 9600
  
  InitESP(); // Initialisation de l'ESP
  
  Serial.println("\nInitialisation du module SD card...");
  pinMode(pinCS, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("ERROR");
    return;
  }
  
//  Serial.println("Creating example.txt...");
//  myFile = SD.open("example.txt", FILE_WRITE);
//  myFile.close();
//
//  // Check to see if the file exists:
//  if (SD.exists("example.txt")) {
//    SD.remove("test.txt");
//  }
  
  InitSD(); //Initialisation de la carte SD
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
        <br><a href='http://192.168.4.1?command=on'><button>ON</button></a><a href='http://192.168.4.1?command=off'><button>OFF</button></a></center>";
     
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
      if(someValue=="on"){
        digitalWrite(LED_BUILTIN, HIGH);
        sendHTTPRequest("?command=on", "192.168.4.2");
      }else{
        digitalWrite(LED_BUILTIN, LOW);
        sendHTTPRequest("?command=off", "192.168.4.2");
      }
      Contenu="Commande=";Contenu+=someValue;
      Serial.print("Commande: "); Serial.println(someValue);
      // On envoi la commande d'allumage ou d'extinction au capteur ici
    }
    else if(ifParameter(readString, "planing_capteur"))
    {
      someValue=getValueOfParameter(readString, "planing_capteur", 16);
      Contenu="Planing=";Contenu+=someValue;
      Serial.print("Planing: "); Serial.println(someValue);
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
  * Name: sendHTTPRequest
  * Description: To send an http request to a peer in the network
  * Request, String
  */
  void sendHTTPRequest(String Request, String IP) { // Request: update?key=P55Z0BDASNQBFQKT&field1=15&field2=25
    String cmd = "AT+CIPSTART=\"TCP\",\"";// Setup TCP connection
    cmd += IP;
    cmd += "\",80\r\n";
    sendData(cmd,2000,DEBUG);
  
    String url = "GET /";
    url += Request;
    url += " \r\n\r\n"; //hard code value 15 and 25
    //Cipsend command
    int connectionId = esp8266.read()-48;
    sendCIPData(connectionId, url);

    Serial.println("Sending data to Capteur...");
    
    //String stringLength="AT+CIPSEND=";
    //stringLength +=String(url.length( ));
    //Serial.println(stringLength);

    //delay(10000); I even tried to delay for few seconds
  
   //Here is where it fails,it jumps to closing the connection
//   if(Serial.find( ">" ) )
//    {
//     softSerial.print(">");
//     softSerial.print(cmds);
//     Serial.print(cmds);
//    }
//    else
//    {
      sendData( "AT+CIPCLOSE\r\n" ,2000,DEBUG); //close TCP connection
    //}
  }

  /*
  * Name: connectToWiFi
  * Description: To connecct to a network
  * ssid_, pass_
  */
  boolean connectToWiFi(String ssid_, String pass_) {
    // sendData( "AT+CWMODE=1\r\n" ,2000,DEBUG); 1(Mode ST, se comporte comme un client), 2(Mode AP se comporte comme un access point) et 3( Les 2 modes, ST et AP)
    
    String cmd="AT+CWJAP=\""; // Join accespoint
    cmd+=ssid_;
    cmd+="\",\"";
    cmd+=pass_;
    cmd+="\"\r\n";
    sendData( cmd ,5000,DEBUG);
    
    if(Serial.find("OK"))
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

  /*
  * Name: writeInFile
  * Description: To write in a file
  * mode_ 0: write without override, 1: write and override
  */
  void writeInFile(String f,String m, int mode_){
    myFile = SD.open(f, FILE_WRITE);
    if (myFile) {
      Serial.print("Writing...");
      myFile.println(m); myFile.close();
      Serial.println("Ok");
    } else {
      Serial.println("ERROR in Writing");
    }
  }

  /*
  * Name: readInFile
  * Description: To read in a file
  * mode_ 0: write without override, 1: write and override
  */
  void readInFile(String f){
    myFile = SD.open(f);
    if (myFile) {
      Serial.print("Reading...");
      while (myFile.available()) {
        Serial.write(myFile.read());
      }
      myFile.close();
      Serial.println("Ok");
    } else {
      Serial.println("ERROR in reading");
    }
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
    Serial.println("Initialisation du module ESP...");
    sendData("AT+RST\r\n",2000,DEBUG); // reset module(RRESET ou reinitialisation)
    
    //Definition des parametres du point d'acces
    sendData("AT+CWSAP=\""+SSID_+"\",\""+PASSWORD_+"\",3,3\r\n",1000,DEBUG); sendData("AT+CWSAP?\r\n",1000,DEBUG); // SSID, mot de passe, niveau de securite puis Verification
    
    //sendData("AT+CIPSTA="+ADRESSEIP_+"\r\n",1000,DEBUG); //On fixe l'adresse IP: du point d'access
    //Configuration du mode DHCP
    //sendData("AT+CWDHCP=2,0\r\n",1000,DEBUG); sendData("AT+CWDHCP?\r\n",1000,DEBUG);//0 pour DHCP, 1 pour juste une station et 2 pour jouer les 2 roles
    
    sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point and client (on configure en tant que point d'access)
    sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address (Recuperation du mot de passe!!)
    sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
    sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
    //sendData("AT+CWLAP\r\n",1000,DEBUG);

    //===============================================================================
    // To close the server the command is AT+CIPSERVER=0 followed by a Reset AT+RST
    //===============================================================================
  }


  /*
  * Name: InitSD
  * Description: Initialisation du module de la carte Memoire
  * 
  */
  void InitSD(){
    if (!card.init(SPI_HALF_SPEED, chipSelect)) {
      Serial.println("initialisation RATEE. Things to check:");
      Serial.println("* La carte est elle inseree");
      Serial.println("* l'equipement est il correct");
      Serial.println("* Avez vous respecter les branchements!?");
      return;
    } else {
      Serial.println("Tout est correct.");
    }
  
    if (!volume.init(card)) {
      Serial.println("On ne trouve pas de fichier FAT16/FAT32 partition.\nMake sure you've formatted the card");
      return;
    }
  
    Serial.println("\nFichiers trouvees dans la carte ");
    root.openRoot(volume);
    //liste des differents fichiers
    root.ls(LS_R | LS_DATE | LS_SIZE);
  }
