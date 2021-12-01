#include "Adafruit_FONA.h"
#include <TinyGPS++.h>
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
//#define FONA_RX 4
//#define FONA_TX 6
//#define FONA_RX 4
//#define FONA_TX 5
//#define FONA_RST 11
#define FONA_RX 9
#define FONA_TX 8
#define FONA_RST 10
// this is a large buffer for replies
////////////////////////////////////////////////
char replybuffer[255];
String commands="";
String YourArduinoData="";
char latitude[15];
char longitude[15];
char fonaNotificationBuffer[64];          //for notifications from the FONA
char smsBuffer[250];
//////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
/////////////////////////////////////////////////////////////
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
SSD1306AsciiWire oled;
/////////////////////////////////////////////////////////////
// Use this for FONA 800 and 808s
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void setup() {

  Serial.begin(GPSBaud);
  //////////////////////////////
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  //oled.setFont(TimesNewRoman16_bold);
 oled.setFont(Callibri11_bold); 
  //////////////////////////////
  Serial.println(F("FONA SMS caller ID test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));
  pinMode(13,1);
  // make it slow so its easy to read!
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    //Serial.println(F("Couldn't find FONA"));
       oled.clear();
       oled.print("Couldn't find SIM ");
    while(1);
  }
 Serial.println(F("FONA is OK"));

  fonaSerial->print("AT+CNMI=2,1\r\n");  //set up the FONA to send a +CMTI notification when an SMS is received

 Serial.println("FONA Ready");
   oled.clear();
   oled.println("SIM Ready");
 
}

  


void loop() {
    getloc();
    char* bufPtr = fonaNotificationBuffer;  
   if (fona.available())     
  {
    int slot = 0;          
    int charCount = 0;
//    Read the notification into fonaInBuffer
    do  {
      *bufPtr = fona.read();
      //Serial.write(*bufPtr);
       oled.clear();
       oled.print(fonaNotificationBuffer);
      delay(1);
    } while ((*bufPtr++ != '\n') && (fona.available()) && (++charCount < (sizeof(fonaNotificationBuffer)-1)));
    
    *bufPtr = 0;

   
    if (1 == sscanf(fonaNotificationBuffer, "+CMTI: " FONA_PREF_SMS_STORAGE ",%d", &slot)) {
      Serial.print("slot: "); Serial.println(slot);
       //oled.clear();
       //oled.print("Couldn't find SIM ");
      char callerIDbuffer[32];  //we'll store the SMS sender number in here
      
   
      if (! fona.getSMSSender(slot, callerIDbuffer, 31)) {
        Serial.println("Didn't find SMS message in slot!");
      }
      Serial.print(F("FROM: ")); Serial.println(callerIDbuffer);

        // Retrieve SMS value.
        uint16_t smslen;
        if (fona.readSMS(slot, smsBuffer, 190, &smslen)) { // pass in buffer and max len!
          Serial.println(smsBuffer);
          commands=smsBuffer;
          oled.clear();
          oled.print(smsBuffer);
          Serial.println(commands);
          if(commands=="get loc"){
               digitalWrite(13,1);
               YourArduinoData += ("https://www.google.com/maps/place/");
               YourArduinoData.concat(latitude);
               YourArduinoData.concat(",");
               YourArduinoData.concat(longitude);
               Serial.println(YourArduinoData);
               char message[51];
               YourArduinoData.toCharArray(message, 51);
               
             if (!fona.sendSMS(callerIDbuffer,message)) {
                Serial.println(F("Failed"));
                 oled.clear();
                 oled.println("Failed");
                } else {
             Serial.println(F("Sent!"));
             oled.clear();
             oled.println("Location Sent!");
               digitalWrite(13,0);
                 }
          
            
          }
       if ((fona.deleteSMS(slot)) || ( fona.deleteSMS(1))|| (fona.deleteSMS(2)) || ( fona.deleteSMS(3))) {

      // Serial.println(F("OK!"));
      }
      else {
      //Serial.println("Deleting Smas from devvice");
      // oled.clear();
       //oled.println("Couldn't delete SMS ");
     // Serial.print(F("Couldn't delete SMS in slot ")); Serial.println(slot);
        fona.print(F("AT+CMGD=?\r\n"));
      }
            
          }
            
          
        }
        }
      }

  void getloc(){
     // This sketch displays information every time a new sentence is correctly encoded.
  if (Serial.available() > 0)
    if (gps.encode(Serial.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    oled.clear();
    oled.println("No GPS detected: check wiring..");
    while(true);
  }
    
 
    
   
  }
  void  displayInfo(){
     
     oled.clear();
     oled.print("Lati-");
     oled.print( latitude);
     oled.println(",Long-");
     oled.print( longitude);
     oled.println(" Date");
     oled.print(gps.date.month());
     oled.print("-");
     oled.print(gps.date.day());
     oled.print( "-");
     oled.print( gps.date.year());
     oled.println( "time");
     oled.print(gps.time.hour());
     oled.print(":");
     oled.print(gps.date.day());
     oled.print( ":");
     oled.print( gps.time.minute());
     
   dtostrf(gps.location.lat(),8,7,latitude);
   dtostrf(gps.location.lng(),8,7,longitude);
   delay(50);
  }
