/*
MONITO V1.0
*/
const int sensorIn = A0;
int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
double p=0;
double Cout=0;
double PrixKW=79;

void setup(){
  Serial.print("\t\t --- MONITO CAPTEUR V1.0 ---\n");
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop(){
    digitalWrite(LED_BUILTIN, HIGH);
    p=(p+getPower(getVPP())); Cout+=((PrixKW*p)/3600);
    Serial.print("->P: "); Serial.print((p/3600),7); Serial.print("KWh et Vous depenssez\t  ->Facture: "); Serial.print(Cout,7); Serial.println(" FCFA ");
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}


float getPower(float v)
{
    Voltage = v;
    VRMS = (Voltage/2.0) *0.707; 
    AmpsRMS = (VRMS * 1000)/mVperAmp;
    return (AmpsRMS*VRMS);
}

//Tension de voltage
float getVPP()
{
  float result;
  
  int readValue;
  int maxValue = 0;          
  int minValue = 1024;          
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //Pour 1 Seconde
   {
       readValue = analogRead(sensorIn);
       if (readValue > maxValue){
           maxValue = readValue;
       }
       if (readValue < minValue){
           minValue = readValue;
       }
   }
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
   return result;
 }
