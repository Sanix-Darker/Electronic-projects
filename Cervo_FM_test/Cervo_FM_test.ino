// MONITO Capteur envoi de donnees pas plus de 25 caracteres 

#include <VirtualWire.h>
#include <SPI.h>
#include <SD.h>

const int id=1;
File myFile;
Sd2Card card;
SdVolume volume;
SdFile root;


unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
const byte e=5;
const int chipSelect = 4;

void setup() {
  Serial.begin(9600);
  // Vous pouvez changez les broches RX/TX/PTT avant vw_setup() si nécessaire a=action()

  Serial.print("\nInitialisation de la SD card...");

  pinMode(10, OUTPUT);
  if (!SD.begin(4)) {
    Serial.println("initialization mauvaise!");
    return;
  }
  intit_SD();
  
  vw_setup(2000);
  vw_rx_start(); 
  Serial.println("CAPTEUR M"); 
}
 
void loop() {
  String m = id+","+mesur()+","+millis();
  vw_send((byte *) &m, sizeof(m)); // On envoie la conso 
  vw_wait_tx(); // On attend la fin de l'envoi
  
  ecrire("a.m",m);
  Serial.print("\n");
  Serial.print("\t Message envoye:");
  Serial.print(m);
  //--------------------------------------------- 
  delay(1000);
}

//---------------------------------------------------------------------- Ecris dans un fichier
void ecrire(String f,String m){
  Serial.println("initialization Ecriture");  
  myFile = SD.open(f, FILE_WRITE);
  if (myFile) {
    Serial.println("Enregistrement...");
    myFile.println(m);
    myFile.close();
    Serial.println("Ok");
  } else {
    Serial.println("ERROR");
  }
}

//---------------------------------------------------------------------- Lis dans un fichier

void lire(String f){
  Serial.println("initialisation Lecture.");
  myFile = SD.open(f);
  if (myFile) {
    Serial.println("Lecture du fichier:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("erreur de lecture");
  }
 }
 
 //------------------------------------------------------------
 
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
//---------------------------------------------------------------------
void intit_SD(){
    if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialisation RATEE. Things to check:");
    Serial.println("* La carte est elle inseree");
    Serial.println("* l'equipement est il correct");
    Serial.println("* Avez vous respecter les branchements!?");
    return;
  } else {
    Serial.println("Tout est correct.");
  }

  /* Type de carte
  Serial.print("\nType de carte: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }
  */

  if (!volume.init(card)) {
    Serial.println("On ne trouve pas de fichier FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }

  /*
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
*/
  Serial.println("\nFichiers trouvees dans la carte ");
  root.openRoot(volume);
  //liste des differents fichiers
  root.ls(LS_R | LS_DATE | LS_SIZE);
}
