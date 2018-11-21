#include <IRremote.h>

int RECV_PIN = 11;  // récepteur IR à la pin 11

IRrecv irrecv(RECV_PIN);

decode_results results;

int LED_state [8]; // 0 quand la LED est éteinte

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); 

  for (int i=0; i <= 7; i++){
    pinMode(i+2, OUTPUT);
  } 

}


void action(decode_results *results) {
  int count = results->rawlen;

  if (results->decode_type == RC5) {

    switch (results->value) {

    case 0x1:
    case 0x801:
      Serial.println("Bouton 1");
      LED_state[0] = !(LED_state[0]);
      break;
    case 0x2:
    case 0x802:
      Serial.println("Bouton 2");
      LED_state[1] = !(LED_state[1]);
      break;
    case 0x3:
    case 0x803:
      Serial.println("Bouton 3");
      LED_state[2] = !(LED_state[2]);
      break;
    case 0x4: 
    case 0x804:    
      Serial.println("Bouton 4");
      LED_state[3] = !(LED_state[3]);
      break;
    case 0x5: 
    case 0x805:    
      Serial.println("Bouton 5");
      LED_state[4] = !(LED_state[4]);
      break;
    case 0x6: 
    case 0x806:    
      Serial.println("Bouton 6");
      LED_state[5] = !(LED_state[5]);
      break;
    case 0x7: 
    case 0x807:    
      Serial.println("Bouton 7");
      LED_state[6] = !(LED_state[6]);
      break;   
    case 0x8:
    case 0x808:    
      Serial.println("Bouton 8");
      LED_state[7] = !(LED_state[7]);
      break;
    case 0x9:
    case 0x809:    
      Serial.println("Bouton 9");
      break;
    case 0x0:  
    case 0x800:   
      Serial.println("Bouton 0");
      break;
    }
  } 

  for (int i=0; i <= 7; i++){
    digitalWrite(i+2, LED_state[i]);
  }
}


void loop() {
  if (irrecv.decode(&results)) {
    action(&results);
    irrecv.resume(); // Receive the next value
  }
}
