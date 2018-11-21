/*
  SD card test
MONITO
 */
// include the SD library:
#include <SPI.h>
#include <SD.h>


File myFile;
Sd2Card card;
SdVolume volume;
SdFile root;

#define chipSelect  4

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // On attends pour le Serial moniteur
  }


 Serial.print("\nInitialisation de la SD card...");

 pinMode(10, OUTPUT);
  if (!SD.begin(4)) {
    Serial.println("initialization mauvaise!");
    return;
  }
  intit_SD();
}

void loop(void) { //Boucle et cervo MONITO
  ecrire("a.m","234,Frigo,4,2015-05-11");
  lire("a.m");
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
 
//---------------------------------------------------------------------- Mesure direc compteur

double mesure(){
 return ((514-(analogRead(A0)))* 27.03 /1023);
}
//----------------------------------------------------------------------

void envoi_commande(){
  
}
//---------------------------------------------------------------------

void recoi_commande(int ID,byte etat){
  
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

