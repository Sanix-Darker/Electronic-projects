/**
 * MONITO cote Serveur
 */
float valeur;
byte taille_message = sizeof(float);
#include <VirtualWire.h>

void setup() {
  Serial.begin(9600);
  // Vous pouvez changez les broches RX/TX/PTT avant vw_setup() si nécessaire
  vw_setup(2000);
  vw_rx_start(); // On peut maintenant recevoir des messages
  Serial.println("\n\t\tCervo MONITO BEGIN..."); 
}

void loop() {
  // On attend de recevoir un message
  vw_wait_rx();
  if (vw_get_message((byte *) &valeur, &taille_message)) {
    // On copie le message, qu'il soit corrompu ou non
    Serial.println("Message rexu");
    Serial.println(valeur); // Affiche le message
  }
  else{
    Serial.println("rexoi rien");
  }
  Serial.println("Gros NADA!!!");
  delay(1000);
}
