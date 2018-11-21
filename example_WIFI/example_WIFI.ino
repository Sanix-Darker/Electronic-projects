 
#define SSID "your_SSID"
#define PASS "your_password"
#define DST_IP "192.168.0.100"
#define CONNECT_ATTEMPTS 5
#define RST 8
#define CHP 9
#define LED 13
 
void setup() {
 
  pinMode(RST, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(CHP, OUTPUT);
 
  reset();
 
  Serial.begin(9600);
  Serial.begin(115200);
 
  delay(5000);
 
  Serial.println("AT+RST");
  delay(1000);
 
  if (Serial.find("ready")) Serial.println("Module is ready");
  else {
    Serial.println("ESP8266 Module did not respond.");
    Serial.println("Enter Commands Manually.");
    while (1) chk_serial_io();
  }
  delay(1000);
  boolean connected = false;
  for (int i = 0; i < CONNECT_ATTEMPTS; i++) {
 
    if (connectWiFi()) {
      connected = true;
      break;
    }
  }
  if (!connected) {
    Serial.println("Enter Commands Manually.");
    while (1) chk_serial_io();
  }
  delay(5000);
  Serial.println("AT+CIFSR");
  Serial.println("ip address:");
  while (Serial.available()) Serial.write(Serial.read());
  Serial.println("AT+CIPMUX=0");
}
 
void reset() {
  digitalWrite(LED,HIGH);
  digitalWrite(CHP,HIGH);
  digitalWrite(RST,LOW);
  delay(100);
  digitalWrite(RST,HIGH);
  delay(1000);
  digitalWrite(LED,LOW);
}
 
void loop() {
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  Serial.println(cmd);
  Serial.println(cmd);
  if (Serial.find("Error")) return;
  cmd = "GET / HTTP/1.0\r\n\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if (Serial.find(">")) {
    Serial.print(">");
 
  } else {
    Serial.println("AT+CIPCLOSE");
    Serial.println("connect timeout");
    delay(1000);
    return;
  }
  Serial.print(cmd);
  delay(2000);
  while (Serial.available()) {
    char c = Serial.read();
    Serial.write(c);
    if (c == '\r') Serial.print('\n');
  }
  Serial.println("====");
  delay(1000);
}
 
void chk_serial_io() {
  if (Serial.available()) {
//    if (Serial1.find("busy")) {
//      Serial.println("Resetting...");
//      reset();
//    }
    int inByte = Serial.read();
    Serial.write(inByte);
  }
  if (Serial.available()) {
    int inByte = Serial.read();
    Serial.write(inByte);
  }
}
 
boolean connectWiFi() {
  Serial.println("AT+CWMODE=1");
  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  Serial.println(cmd);
  Serial.println(cmd);
  delay(2000);
  if (Serial.find("OK")) {
    Serial.println("OK, WiFi Connected.");
    return true;
  } else {
    Serial.println("Can not connect to WiFi.");
    return false;
  }
}
