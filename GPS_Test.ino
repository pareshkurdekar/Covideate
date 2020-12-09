#include<SoftwareSerial.h>
#include<string.h>


SoftwareSerial Serial3 (2,3);            // For GPS Communication

void setup()
{
  
  Serial.begin(57600);
  
  Serial3.begin(9600);
  
}

String str;
char buf[50];
int len=50;

void loop()
{
  while(Serial3.available()>0)
{
  str = Serial3.readStringUntil('\n');
  //Serial.println(str);
}
if(str.substring(1,6) == "GPGGA")                // Longitude + Latitude according to NMEA Format
{
  Serial.println(str);
  str.toCharArray(buf , len);
  
}
Serial3.flush();

}
