/*
  MONITO en mode Client
 */

#include <SPI.h>
#include <WiFi101.h>


char ssid[] = "MONITO";      // Nom du reso
char pass[] = "123456789";   // Mot de passe
int keyIndex = 0;                 // cle special pour le WEP

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  //Initialisation en attendant l'ouverture des ports
  Serial.begin(9600);
  while (!Serial) {
    ; // attendre que le port série pour se connecter . Nécessaire pour le port USB natif uniquement
  }

  // vérifier la présence du module WIFI :
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // ne pas continuer
    while (true);
  }

  // tenter de se connecter à un réseau Wifi
  while ( status != WL_CONNECTED) {
    Serial.print("connection au WIFI de nom: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);

    // on wait 10s pour la connection
    delay(10000);
  }
  server.begin();
  // C'est connectee donc coul
  printWifiStatus();
}


void loop() {
  // On ecoute pour les differents clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("nouvo client");
    // une requête http se termine par une ligne blanche
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // Si on a obtenu à la fin de la ligne ( reçu une nouvelle ligne
        // Caractère ) et la ligne est vide, la requête http est terminée,
        // Afin que vous puissiez envoyer une réponse
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // la connexion est fermée après l'achèvement de la réaction
          client.println("Refresh: 5");  // on actualise apres 5S
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // en sortie la valeur de chaque broche d'entrée analogique
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // on lance une nouvelle ligne
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // on a un caractere sur la nouvelle ligne
          currentLineIsBlank = false;
        }
      }
    }
    // on done le temps o navigateur
    delay(1);

    // on close la connectio
    client.stop();
    Serial.println("client deconnecter");
  }
}


void printWifiStatus() {
  // on affiche les ways
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // mm l'IP
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // et la force du signal
  long rssi = WiFi.RSSI();
  Serial.print("Force du Signal (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

