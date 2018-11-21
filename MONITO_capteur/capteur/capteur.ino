/*
  MONITO capteur
*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define ACSPIN 2

// Parametres WIFI
const char* ssid = "MONITO";
const char* password = "12345678";

// Server WEB ESP
String serverHost = "http://192.168.4.1/feed";
String data;
// DEEP_SLEEP Timeout interval
int sleepInterval = 1;
// DEEP_SLEEP Timeout interval when connecting to AP fails
int failConnectRetryInterval = 1;
int counter = 0;

//Les donnees a envoyer tout le temps(ID du capteur et la consommation en temps reel)
String MONITO_ID="jksAS43";
float value;
// Static network configuration
IPAddress ip(192, 168, 4, 4);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiClient client;

void setup() {
  ESP.eraseConfig();
  WiFi.persistent(false);
  Serial.begin(74880);
  Serial.println();
  Serial.println("**************************");
  Serial.println("**************************");
  Serial.println("******** STARTING ********");

  // Ici la methode qui permet de demarer les fonctionnalitee ou de checker l'ACS
  
  delay(500);
  Serial.println("- set ESP STA mode");
  WiFi.mode(WIFI_STA);
  Serial.println("- connecting to wifi");
  WiFi.config(ip, gateway, subnet); 
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    if(counter > 30){
       Serial.println("- Connexion impossible, ...dodo");    
       hibernate(failConnectRetryInterval);
    }
    delay(500);
    Serial.print(".");
    counter++;
  }
  
  Serial.println("- wifi connected");
  Serial.println("- read DHT sensor");
  
  //Methode qui mesure l'intensite
  
  Serial.println("- Fabrication de la requette d'envoi");
  //buildDataStream();
  Serial.println("- Envoi de la requette");
  //sendHttpRequest();
  Serial.println();
  Serial.println("- Data SEND");
  Serial.println("**************************");
  Serial.println("**************************");
}

void sendHttpRequest() {
  HTTPClient http;
  http.begin(serverHost);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST(data);
  http.writeToStream(&Serial);
  http.end();
}

void readDHTSensor() {
  delay(200);
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    t = 0.00;
    h = 0.00;
  }
  Serial.println("- intensity read : "+String(value));
  Serial.println("- MONITO_ID : "+String(MONITO_ID));
}

void buildDataStream() {
  data = "value=";
  data += String(value);
  data += "&monito_id=";
  data += String(MONITO_ID);
  Serial.println("- data stream: "+data);
}

//Petite hybernation de l'ESP
void hibernate(int pInterval) {
  WiFi.disconnect();
  ESP.deepSleep(10 * 600000 * pInterval, WAKE_RFCAL);
  delay(100);
}

void loop() {}
