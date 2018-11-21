/*
  MONITO mesure de courant 
*/
const int analogIn = A0;
int mVperAmp = 66; // on utilisera bon euuuhhh  185 pour le module de 5A, 100 pour 20A et 66 pour le module de 30A.
int RawValue= 0;
int ACSoffset = 2500; 
double V = 0;
double A = 0;
double P = 0;

void setup(){ 
 Serial.begin(9600);
}

void loop(){
 
 RawValue = analogRead(analogIn);
 V = (RawValue / 1023.0) * 5000; // on a nos Volts
 A = ((V - ACSoffset) / mVperAmp);
 P = A * V;
 Serial.print("\n >>Raw Value = " ); // shows pre-scaled value 
 Serial.print(RawValue); 
 Serial.print("\t mVolt = "); // Voltage en mcro montre moi
 Serial.print(V,5); // Laisse '3' valeurs apres la virgule pour le detail
 Serial.print("\t Ampere = "); // Ampere donnees par Monito 
 Serial.println(A,5); // Laisse '3' valeurs apres la virgule pour le detail
 Serial.print(" =->   Puissance = "); // Ampere donnees par Monito 
 Serial.println(P,5); // Laisse '3' valeurs apres la virgule pour le detail
 
 delay(2000); 
 
}

