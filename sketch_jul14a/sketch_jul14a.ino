#include <LiquidCrystal.h>
#include <WiFi.h>
#include <WString.h>

char ssid[] = "my_ssid";     //  your network SSID (name)
char pass[] = "My_password";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
IPAddress ip;

// wifi server
WiFiServer server(80);

LiquidCrystal lcd(9, 8, 6, 5, 3, 2);

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int ledArduino = 13;

String readString = String(100);

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(ledArduino, OUTPUT);
  lcd.begin(16, 2);
  printLcd("Kitpages Wifi", "Connecting");

   // initialize serial:
  Serial.begin(9600);
  // attempt to connect using WPA2 encryption:
  Serial.println("Attempting to connect to WPA network...");
  status = WiFi.begin(ssid, pass);

  // if you're not connected, stop here:
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    printLcd("Wifi failed");
    while(true);
  }
  // if you are connected, print out info about the connection:
  else {
    Serial.println("Connected to network");
    Serial.print("LocalIp=");
    ip = WiFi.localIP();
    Serial.println(ip);
    printWifiStatus();
        
    printLcd("Kitpages WiFi OK");
    lcd.print(ip);
  }
  
  server.begin();
}

// the loop routine runs over and over again forever:
void loop() {
//  Serial.println("entering loop");
  WiFiClient client = server.available();
  //Serial.println("client found");
  if (client) {
    while (client.connected()) {
      digitalWrite(ledArduino, HIGH);
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (readString.length() < 100) {
          //store characters to string 
          readString += c; 
        } 
        printLcd("Kitpages", readString);
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
  delay(10);

  digitalWrite(ledArduino, LOW);
}

void printLcd(char* line1, char* line2)
{
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}
void printLcd(char* line1, String line2)
{
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}
void printLcd(char *line)
{
  printLcd(line, "");
}

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
  
  Serial.print("Firmware version: ");
  Serial.println(WiFi.firmwareVersion());
}
