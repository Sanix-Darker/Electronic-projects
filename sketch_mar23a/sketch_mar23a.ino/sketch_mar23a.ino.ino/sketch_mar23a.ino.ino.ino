  const int bouton=13;
  const int L1=12;
  //const int broche2=12;
int etat;

 void setup() {
  // put your setup code here, to run once:
 pinMode(bouton,INPUT); 
 pinMode(L1, OUTPUT);
 etat=HIGH;

}

void loop() { attachInterrupt(0,Allume,CHANGE);
 /* boolean i=0;
  i!=i;
  go(var,var1);*/
etat=digitalRead(bouton);
  if(etat==HIGH){
    digitalWrite(L1,HIGH);
  }
  else{
    digitalWrite(L1,LOW);
  }
  /* digitalWrite(broche2,HIGH);// On eteint la LED
  delay(1000); // On attends 1000ms donc 1s
  digitalWrite(broche2,LOW);// On allume la LED
  delay(5000);*/
}/*
void go(int a, int b){
  a=a+b;
  b=a-b;
  for(int i;i<a; i++){
    b--;
    if(b==0){
      a++;
    }
  }
}
*/
void Allume(){
  digitalWrite(13,HIGH);
  delay(1000);
  digitalWrite(13,LOW);
  }
