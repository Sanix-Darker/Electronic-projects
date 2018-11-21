/************************************
* ESP8266 Web server for Arduino
* switches LED on/off
* written by Laurent Sineux
*
* Arduino      ESP
* PIN 10       TXD
* PIN 11       RXD
* +3,3V        CH_PD (always selected)
*************************************/

#include <SoftwareSerial.h>
const boolean debug=true;  //logs all response from ESP to serial monitor

SoftwareSerial espSerial(10,11); // defines arduino pin 10 as RX and 11 as TX
HardwareSerial & dbgSerial = Serial; // serial monitor

#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE]; //buffer to store incoming messages

#define pinLED 13
boolean ledValue = 0; //off by default

/******************************************
* FUNCTION : void setup(void)
* PURPOSE : run only once to initialize
*           the system after HW reset
*******************************************/
void setup()
{
    pinMode(2, INPUT);
    dbgSerial.begin(9600); // starts serial link to monitor
    espSerial.begin(9600); // starts serial link to ESP
    delay(5000);
    sendData("AT+RST\r\n",15000); // reset module
    sendData("AT+CWMODE=1\r\n",2000); // Configure as station
    sendData("AT+CWJAP=\"MONITO\",\"123456789\"\r\n",10000); // connecte à la livebox
    sendData("AT+CIPMUX=1\r\n",2000); // configure for multiple connections
    sendData("AT+CIPSERVER=1,80\r\n",2000); // turn on server on port 80
    pinMode(pinLED, OUTPUT); //define pin as output
}
  
/********************************************
* FUNCTION : void loop (void)
* PURPOSE : main loop
*********************************************/   
void loop()
{    
    int ch; // channel ID
    int len; //  message length
    char *pb; // pointer inside the buffer
    if(espSerial.available())
     {
       espSerial.readBytesUntil('\n', buffer, BUFFER_SIZE); //read until newline
    
        if(debug)
        {
          dbgSerial.print("www :");
          dbgSerial.println(buffer);
        }
     }
     else{dbgSerial.println("OUPS BIG ERREUR...");}
    // we expect a message like this "+IPD,ch,len:data"
    if(strncmp(buffer, "+IPD,", 5)==0) //check if message starts with +IPD 
    {
      sscanf(buffer+5, "%d,%d", &ch, &len); //extract channel and length
      if (len > 0) // if message not empty
      {
        // to find the data we must move the cursor after the colon
        pb=buffer;
        while(*pb!=':') pb++; // increase pointer until character is a colon
        pb++; // +1 to move after the colon, not at the colon

        if(debug)
        {
          dbgSerial.print("ch:");
          dbgSerial.println(ch,DEC);
          dbgSerial.print("len:");
          dbgSerial.println(len,DEC);
          dbgSerial.print("pb:");
          dbgSerial.println(pb);
        }


        if (strstr(pb, "GET /?LED=ON"))
        {
          dbgSerial.println("led ON");
          flushSerial(); // get rid of message end eg "HTML/1.1"
          ledValue=1;
          homepage(ch);
        }
        else if (strstr(pb, "GET /?LED=OFF"))
        {
          dbgSerial.println("led OFF");
          flushSerial();// get rid of message end eg "HTML/1.1"
          ledValue=0;
          homepage(ch);
        }
        else if (strstr(pb, "GET /"))
        {
          flushSerial();// get rid of message end eg "HTML/1.1"
          homepage(ch);
        }
        if(ledValue==1) digitalWrite(pinLED,HIGH);
        else digitalWrite(pinLED, LOW);
    }
    }
    clearBuffer();
} 

  
/****************************************************
* FUNCTION : boolean sendData(String command, int timeout)
* PURPOSE : sends command to ESP8266, 
*           waits for response until timeout,
*           compares response from ESP with expected response
* PARAMETERS :
*          String command : command to be sent to the ESP
*          int timeout : number of milliseconds to wait
* RESULT :
*          boolean: true if response contains "OK"
*/
boolean sendData(String command, int timeout)
{
    const int respLen = 128;
    char response[respLen];
    int i;
    // erase the whole response buffer
    for (i=0;i<respLen;i++)
    {
        response[i]='\0';
    }
    
    
    if(debug)
    {
      dbgSerial.print(" > ");
      dbgSerial.print(command);
    }
    
    espSerial.print(command); // send command to the esp8266
    i=0;
    long int time = millis();
    while( (time+timeout) > millis())
    {    
      while(espSerial.available() && (i < respLen-1)) // as long as we have something to read and the buffer is not full
      {
        // The esp has data so display its output to the serial window
        response[i] = espSerial.read(); // read the next character.
        i++; //increment number of characters
      } 
    }
    response[i]=0; // always end the string with a null character
      if(debug && i!=0)
        {
          dbgSerial.print(i);
          dbgSerial.print(" < ");
          dbgSerial.println(response);
        }
   
    

   if( strstr(response, "OK") || strstr(response,"no change" )) return true;
   else return false;
   
}

/*************************************************************
* FUNCTION : void flushSerial(void)
* PURPOSE : read any remaining characters in espSerial buffer
*************************************************************/
void flushSerial(void)
{
  if(debug) dbgSerial.print("flushing :");
  while ( espSerial.available()) // while there are characters available
  {
    if(debug) dbgSerial.write(espSerial.read());
  }
  if(debug) dbgSerial.println("");
}

/**************************************************************
* FUNCTION : void clearBuffer (void)
* PURPOSE : erases buffer to ensure no false readings
**************************************************************/
void clearBuffer (void)
{
   for (int i =0;i<BUFFER_SIZE;i++ )
   {
      buffer[i]=0;
   } 
}

/**************************************************************
* FUNCTION : void homepage(int id)
* PURPOSE : send a web page and close connection
***************************************************************/
void homepage(int id)
{
    
    String Content; // this is my HTML form

    Content =   "<HTML>";
    Content += "<HEAD><TITLE>Arduino + ESP8266 Webserver</TITLE></HEAD>";
    Content += "<BODY><H1>LED =";
    if(ledValue == 0) Content += "OFF";
    else Content += "ON";
    Content += "</H1><BR><FORM name=\"LED Form\" action=\"/\" method=get>";
    Content += "<input type=\"submit\" name=\"LED\" value=\"ON\"><BR>";
    Content += "<input type=\"submit\" name=\"LED\" value=\"OFF\">";
    Content += "</FORM></BODY></HTML>";
    

    String Header; // this is the header message before the page
     
    Header = "HTTP/1.1 200 OK\r\n";
    Header += "Content-Type: text/html; charset=UTF-8\r\n";
    Header += "Connection: close\r\n";
    //Header += "Refresh: 5\r\n";
    Header += "Content-Length: ";
    Header += (int)(Content.length());
    Header += "\r\n\r\n"; 

    espSerial.print("AT+CIPSEND=");
    espSerial.print(id);
    espSerial.print(",");
    espSerial.println(Header.length()+Content.length());
    delay(100);
    if (espSerial.find(">"))
    {
        espSerial.print(Header);
        espSerial.print(Content);
        delay(1000);
    } 
    espSerial.print("AT+CIPCLOSE=");
    espSerial.print(id); 
    espSerial.print("\r\n"); 

}

