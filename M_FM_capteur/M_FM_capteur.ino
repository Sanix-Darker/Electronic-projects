// MONITO Capteur envoi de donnees pas plus de 25 caracteres 
 unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
const byte e=5;

#include <VirtualWire.h>

void setup() {
  Serial.begin(9600);
  // Vous pouvez changez les broches RX/TX/PTT avant vw_setup() si nécessaire a=action()
  vw_setup(2000);
  vw_rx_start(); 
  Serial.println("CAPTEUR M"); 
}
 
void loop() {
  String m = "1,"+mesur()+","+millis();
  vw_send((byte *) &m, sizeof(m)); // On envoie la conso 
  vw_wait_tx(); // On attend la fin de l'envoi

  Serial.print("\n");
  Serial.print("\t Message envoye:");
  Serial.print(m);
  //--------------------------------------------- 
  delay(1000);
}


boolean action(byte message[VW_MAX_MESSAGE_LEN]){
  byte taille_message = VW_MAX_MESSAGE_LEN;
  vw_wait_rx();
  if (vw_get_message(message, &taille_message)) {
    if (strcmp((char*) message, "on") == 0) {
      return true;
    } else if (strcmp((char*) message, "off") == 0) {
      return false;
    }
  }
}

String mesure(){
    for (int x = 0; x <= 150; x++){
      AcsValue = analogRead(e);
      Samples = Samples + AcsValue;
      delay (7);
    }
    AvgAcs=Samples/150.0;
    AvgAcs=((AvgAcs * (5.0 / 1023.0)) )/0.066;
    return (String)AvgAcs;
}

String mesur(){
 float A=((514-(analogRead(e)))* 27.03 /1023);
 return (String)A;
}

void rexoi(){
  byte message[VW_MAX_MESSAGE_LEN];
  byte taille_message = VW_MAX_MESSAGE_LEN;
  vw_wait_rx();
  if (vw_get_message(message, &taille_message)) {
    Serial.println((char*) message); // Affiche le message
  }
}
