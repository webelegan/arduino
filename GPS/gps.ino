#include <TinyGPS++.h>
#include <SoftwareSerial.h>
 
static const int RX = 4, TX = 3;
static const uint32_t GPSBaud = 4800;
 
TinyGPSPlus gps;
 
SoftwareSerial gpsneo(RX, TX);
 
void setup()
{
  Serial.begin(9600);
  gpsneo.begin(GPSBaud);
 
  Serial.println(F("Trial GPS menggunakan Arduino"));
  Serial.println();
}
 
void tampilkandata()
{
 
  //Menampilkan data LOKASI
  Serial.print(F("Lokasi : ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }
 
//Menampilkan data tanggal
  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }
 
  //Menampilkan data waktu
  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }
 
  Serial.println();
}
 
void loop()
{
  while (gpsneo.available() > 0)
    if (gps.encode(gpsneo.read()))
      tampilkandata();
 
//Cek komunikasi arduino dengan module gps neo6mv2
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("tidak ada module GPS, cek wiringmu!"));
    while(true);
  }
}