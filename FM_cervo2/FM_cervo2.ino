/**
 * Exemple de code pour la bibliothèque VirtualWire – Serveur d'envoi de structure
 */

#include <VirtualWire.h>

typedef struct {
  char commande;
  int valeur;
} MaStructure;

void setup() {
  Serial.begin(9600);

  // Initialisation de la bibliothèque VirtualWire
  // Vous pouvez changez les broches RX/TX/PTT avant vw_setup() si nécessaire
  vw_setup(2000);
  vw_rx_start(); // On peut maintenant recevoir des messages

  Serial.println("Go !"); 
}

void loop() {
  MaStructure message;
  byte taille_message = sizeof(MaStructure);

  /* 
   La variable "taille_message" doit impérativement être remise à 
   la taille de la structure avant de pouvoir recevoir un message. 
   Le plus simple est d'utiliser une variable locale pour ne pas
   avoir à réassigner la valeur à chaque début de loop().
   */

  // On attend de recevoir un message
  vw_wait_rx();

  if (vw_get_message((byte *) &message, &taille_message)) {
    // On copie le message, qu'il soit corrompu ou non

    Serial.print("commande="); // Affiche le message
    Serial.print(message.commande);
    Serial.print(" valeur=");
    Serial.println(message.valeur);
  }
}
