#include <Adafruit_GPS.h>
#include <SPI.h>
#include <SD.h>
const int chipSelect = 4;
const uint8_t SDCARD_LED =  13;
const uint8_t RED_LED = 13;
int donePin = 5; // Done pin - can be any pin.
int i=0;
int attempts , good_fixes , bad_fixes ;
char fileName[] = "00000000.CSV";
// hardware serial port
#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);
#define GPSECHO false
uint32_t timer = millis();
void setup ()
{
// while (!Serial); // uncomment to wait until Serial is ready
pinMode(RED_LED, OUTPUT);
digitalWrite(RED_LED, LOW);  
Serial.begin(115200);
attempts = 0; good_fixes = attempts;
bad_fixes = attempts;
// Initialization sub-routines defined at end of sketch
SDcheck() ;
GPSconnect() ; 
delay(10000) ;
}
void loop() {
  /////////////////////////////////////////////////////////////////////////////////////////////
  /////////////    \/ this number chooses how many data points per power cycle (number wanted minus 1)
  if (good_fixes > 0) {digitalWrite(donePin, LOW); digitalWrite(donePin, HIGH); digitalWrite(donePin, LOW);}
   ///////////////////////////////////////////////////////////////////////////////////////////// 
char c = GPS.read();
if (!GPS.fix) Serial.println("no location! no fix! ");
if (GPSECHO)
if (c) Serial.print(c);
if (GPS.newNMEAreceived()) {
if (!GPS.parse(GPS.lastNMEA()))
return; }
//change the length of pause here..............................\/   1000=1 second do 500 less than you want ie 15 seconds do 14500
if (timer > millis()) timer = millis(); if (millis() - timer > 4500) {
timer = millis(); // reset the timer attempts = attempts + 1;


sprintf(fileName, "20%02d%02d%02d.txt", GPS.year, GPS.month, GPS.day); //name file with gps date

File dataFile = SD.open(fileName, FILE_WRITE);
if (dataFile) {
dataFile.print("20");
dataFile.print(GPS.year, DEC);
dataFile.print('-');
if (GPS.month < 10) dataFile.print('0');
dataFile.print(GPS.month, DEC);
dataFile.print('-');
if (GPS.day < 10) dataFile.print('0');
dataFile.print(GPS.day, DEC);
dataFile.print(", ");
// time
if (GPS.hour < 10) dataFile.print('0');
dataFile.print(GPS.hour, DEC); dataFile.print(':');
if (GPS.minute < 10) dataFile.print('0');
dataFile.print(GPS.minute , DEC); dataFile.print(':');
if (GPS.seconds < 10) dataFile.print('0');
dataFile.print(GPS.seconds , DEC);
dataFile.print(", ");
// location data
if (GPS.fix) {
good_fixes = good_fixes + 1;
digitalWrite(SDCARD_LED, HIGH); //SD card LED on
// location data
dataFile.print(GPS.latitude , 4); dataFile.print(GPS.lat); dataFile.print(", ");
dataFile.print(GPS.longitude , 4); dataFile.print(GPS.lon);
dataFile.print(", "); dataFile.print(GPS.altitude);
dataFile.print(", "); dataFile.print(GPS.speed);
dataFile.print(", "); dataFile.print(GPS.angle);
dataFile.print(", "); dataFile.print((int)GPS.fix);
dataFile.print(", "); dataFile.print((int)GPS.fixquality);
dataFile.print(", "); dataFile.println((int)GPS.satellites);
}
delay(500);
digitalWrite(SDCARD_LED, LOW); //SD card LED off
dataFile.close();
}
else {
bad_fixes = bad_fixes + 1;
dataFile.print("na ,"); dataFile.print("na, ");
dataFile.print("na, ");
dataFile.print("na, "); dataFile.print("na, ");
dataFile.print((int)GPS.fix); dataFile.print(", ");
dataFile.print((int)GPS.fixquality); dataFile.print(", ");
dataFile.print((int)GPS.satellites);
dataFile.println(", "); dataFile.close();
}
} // close file
//  delay(10);
} // close if timer } // close loop


void SDcheck (void) {
if (!SD.begin(chipSelect)) {
Serial.println("Card failed , or not present");
// don't do anything more:
return;
}
Serial.println("card initialized."); delay(1000);
File dataFile = SD.open("log.txt", FILE_WRITE); if (dataFile) {
Serial.println("File open. Logging!");
delay(1000); }
// If the file is not open, pop up an error
else {
Serial.println("No file! Not logging!");
delay(1000); return;
} }
void GPSconnect (void) {
GPS.begin(9600); GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
GPS.sendCommand(PGCMD_ANTENNA); delay(1000);
// Ask for firmware version
GPSSerial.println(PMTK_Q_RELEASE); }
