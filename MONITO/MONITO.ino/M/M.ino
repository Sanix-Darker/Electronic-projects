//MONITO Brain


void setup() {
 Serial.begin(9600);
}

void loop() {
  float A=0.0, P=0.0;
  int  M_s;
 M_s=analogRead(A0);
    A=((514-M_s)* 27.03 /1023);
      P=A*230/1000;
      Serial.print("\n Analog: ");
      Serial.print(M_s,5);
      Serial.print("\t A: ");
      Serial.print(A,5);
      Serial.print(" ->P: ");
      Serial.print(P,5);
      Serial.print("KWatt ");
      Serial.print("et Vous depenssez\t  ->Facture: ");
      Serial.print(79*P,5);
      Serial.print(" FCFA ");
      delay(1000);

  /*
      EEPROM.put(adresse, ss);
      adresse += sizeof(ss);

      EEPROM.put(adresse, pp); 
      adresse += sizeof(pp);
      
      float va=EEPROM.get(adresse, ss);
      */

}
