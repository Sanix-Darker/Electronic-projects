#define CURRENT_SENSOR A0  // Define Analog input pin that sensor is attached
 
float amplitude_current;      // Float amplitude current
float effective_value;       // Float effective current
int mod=66;// mode de l'ACS
void setup()
{
    Serial.begin(9600);
    pins_init();
}
void loop()
{
    int sensor_max;
    sensor_max = getMaxValue();
    Serial.print("sensor_max = ");
    Serial.println(sensor_max);
    
    //the VCC on the Grove interface of the sensor is 5v
    
    amplitude_current=(float)(sensor_max-512)/1024*5/mod*1000; 
    effective_value=amplitude_current/1.414; // Valeur Effective
   
    
    Serial.print("\nAmplitude en Milli Ampere:");
    Serial.print(amplitude_current,5);
    
    Serial.print("\t La valeur Net est:");
    Serial.println(effective_value,5);
}
void pins_init()
{
    pinMode(CURRENT_SENSOR, INPUT);
}
 
int getMaxValue()
{
    int sensorValue;    //Valeur lue par le Capteur sensible
    int sensorMax = 0;
    uint32_t start_time = millis();
    while((millis()-start_time) < 2500) //boucle pour 1000ms
    {
        sensorValue = analogRead(CURRENT_SENSOR);
        if (sensorValue > sensorMax)
        {
            /*On recupere la valeur la plus Max*/
            
            sensorMax = sensorValue;
        }
    }
    return sensorMax;
}
