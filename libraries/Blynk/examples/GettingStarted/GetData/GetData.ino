/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * You can use this sketch as a debug tool that prints all incoming values
 * sent by a widget connected to a Virtual Pin 1 in the Blynk App.
 *
 * App project setup:
 *   Slider widget (0...100) on V1
 *
 **************************************************************/

#define BLYNK_PRINT Serial
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "YourAuthToken";

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth);
}

// This function will be called every time
// when App writes value to Virtual Pin 1
BLYNK_WRITE(V1)
{
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  // You can also use:
  // int i = param.asInt() or
  // double d = param.asDouble()
}

void loop()
{
  Blynk.run();
}

