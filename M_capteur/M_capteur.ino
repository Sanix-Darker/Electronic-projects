char id=1;
#include <SPI.h>
#include <WiFi.h>
 
char ssid[] = "MONITO";
char pass[] = "123456789";
// char keyIndex = 0;   Index de clee pour un reso WEP
 
int status = WL_IDLE_STATUS;
char machaine[15] = "";

//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.google.com";    // name address for Google (using DNS)
 
WiFiClient client;
 
void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("Composant WIFI absend"); 
    // On continu pas!
    while(true);
  } 
 
  // Connection au serveur
  while (status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

    delay(10000);
  } 
  Serial.println("Connecter o wifi");
 /* printWifiStatus(); */
 
  Serial.println("\nConnexion au cervo...");
  
  if (client.connect(server, 80)) {
    Serial.println("Connecter au Cervo");
    // Make a HTTP request:
    sprintf(machaine, "GET /?id=%d&val=%d HTTP/1.1", id,mesure());
    client.println(machaine);
    client.println("Host: www.google.com");
    client.println("Connection: fermer");
    client.println();
  }
}
 
void loop() {

  while (client.available()) {
    char c = client.read();
    if(c==id)
      //On eteint l'appareil

    Serial.write(c);//On affiche juste ce qui est envoyer
  }

  if (client.connect(server, 80)) {
    Serial.println("Connecter au Cervo");
    // Make a HTTP request:
    sprintf(machaine, "GET /?id=%d&val=%d HTTP/1.1", id,mesure());
    client.println(machaine);
    client.println("Host: www.google.com");
    client.println("Connection: fermer");
    client.println();
  }
 
  // Si le cervo est deconnecter ou ne fonctionne plus
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
 
    // do nothing forevermore:
    while(true);
  }
 delay(70);
}
 
/*
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
 
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
 
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
*/

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

