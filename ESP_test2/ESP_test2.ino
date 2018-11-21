#include <SoftwareSerial.h>

SoftwareSerial esp8266(9,10);

#define DEBUG true       
                    
String NomduReseauWifi = "MONITO"; 
String MotDePasse      = "123456789"; 

void setup() {
  esp8266.begin(9600); // your esp's baud rate might be different
  delay(2000);
  Serial.begin(9600);

Serial.println("////////////////////////////////////////////////////////");
Serial.println("          > Initialisation du module : <");
Serial.println("");
//Com_ESP8266("AT\r\n", 1500, 'OK', "Connexion au module " ," -> Terminé", " -> Erreur", DEBUG);
//Com_ESP8266("AT+RST\r\n", 1500, 'ready', "Réinitialisation du module" ," -> Terminé", " -> Erreur", DEBUG);
Com_ESP8266("AT+CIOBAUD=9600\r\n", 1500, "OK", "Configuration de la connexion à 9600bds " ,"-> Terminé", "-> Erreur", DEBUG);
Com_ESP8266("AT+CWMODE=3\r\n", 1500, "OK", "Configuration du module en tant que Client et Serveur " , "-> Terminé", "-> Erreur", DEBUG);
Com_ESP8266("AT+CWJAP=\""+ NomduReseauWifi + "\",\"" + MotDePasse +"\"\r\n", 10000, "OK", "Connexion à " + NomduReseauWifi , " -> Terminé", "-> Erreur", DEBUG);
Com_ESP8266("AT+CIFSR\r\n", 2000, "OK", "Obtention de l'adresse IP " , "-> Terminé", "-> Erreur", DEBUG);
Com_ESP8266("AT+CIPMUX=1\r\n", 1500, "OK", "Configuration du mode multi-client " , "-> Terminé", "-> Erreur", DEBUG);
Com_ESP8266("AT+CIPSERVER=1,80\r\n", 1500, "OK", "Ouverture du serveur Web sur le port 80 " , "-> Terminé", "-> Erreur", DEBUG);
Serial.println("");
Serial.println("          > Initialisation terminé <"); 
Serial.println("////////////////////////////////////////////////////////");
}

boolean Com_ESP8266(String cmd, int timeout, String reponse, String msg_config, String msg_correct, String msg_echec, boolean debug)
{ 
  
  delay(200);
  Serial.print(msg_config);    
  
  esp8266.print(cmd);
  if(debug){ Serial.println(""); Serial.println("Commande envoyé : " + cmd); }
  
  long int time = millis(); 
  String rep = "";   
  while( (time + timeout) > millis()) 
  {
    while ( esp8266.available() ) 
    {
      char c = esp8266.read();
      rep += c;
    }
  }

  int ok_place_id = rep.indexOf(reponse);
  
/* DEBUG */  if(debug){ Serial.println("////// Réponse du module : //////"); Serial.println(rep); Serial.println("////// Fin Réponse //////"); Serial.println("ok_place_id : "+ String(ok_place_id)); }

  if(ok_place_id > 0) 
  {
    Serial.println(msg_correct);
/* DEBUG */  if(debug){Serial.println("==============================="); }
    return true; 
  }else{ 
    Serial.println(msg_echec);
/* DEBUG */  if(debug){Serial.println("==============================="); }
    return false; 
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  if(esp8266.available()) // check if the esp is sending a message 
  {
    if(esp8266.find("+IPD,"))
    {
      delay(1000);
      int connectionId = esp8266.read()-48;
      Serial.println("");  
      Serial.println("-> Nouvelle Connexion en cours");
      Serial.println("");                 

      Send_html("<html><head><title>ESP8266 LED Control</title>", connectionId, 1000);
      Send_html("<link rel=\"icon\" type=\"image/png\" href=\"http://www.favicon.cc/favicon/695/679/favicon.png\" /></head>", connectionId, 1000);

      delay(500);
      Close_Connect(connectionId);
      delay(1000);
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Close_Connect(int connectionId) {
  Serial.println("");
  Serial.println("-> Fin de la connexion (ID : " + String(connectionId) + ")");
  Serial.println("////////////////////////////////////////////////////////");
  esp8266.print("AT+CIPCLOSE=" + String(connectionId) + "\r\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Send_html(String data, int connectionId, int timeout) {
 
   Com_ESP8266("AT+CIPSEND=" + String(connectionId) + "," + String(data.length()) + "\r\n", timeout, ">", "" , "Authorisation AT+CIPSEND", "Erreur AT+CIPSEND", DEBUG);
/* DEBUG */   if(DEBUG){ Serial.println("AT+CIPSEND=" + String(connectionId) + "," + String(data.length()) + "\r\n");}
   Com_ESP8266(data, timeout, "SEND OK", "" , "Envoi data : " + String(data), "", DEBUG);
   Serial.println("---------------------------");
   delay(50);
   
}
