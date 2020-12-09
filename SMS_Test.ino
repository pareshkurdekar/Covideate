#include <SoftwareSerial.h>
SoftwareSerial mySerial(9, 10);
String textMessage_1;
int pre1;
int pres1 = pre1 = LOW;
int led1 = 6;

void func(String textMessage)

{
  if (textMessage.indexOf("8277795414") > 0)               // Number from which message is received 
  {
    if (textMessage.indexOf("ON1") > 0)
    {
      Serial.println("Works");
      analogWrite(led1, 255);

    }
    if (textMessage.indexOf("OFF1") > 0)
    {
      Serial.println("Works");
      digitalWrite(led1, LOW);
    }
    delay(10);
  }
}





void setup() {


  Serial.begin(9600);
  mySerial.begin(2400);
  Serial.println("Initializing...");
  delay(1000);
  mySerial.println("AT");
  updateSerial();
  mySerial.println(" AT+CMEE=2");                    // AT Commands for Intiializing to SMS Mode & Error Debugging
  updateSerial();


  mySerial.println("AT+CMGF=1");
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0");
  updateSerial();

  pinMode(7, INPUT);
  pinMode(led1, OUTPUT);                         // LED for Testing

}

void loop() {
  updateSerial();
  pres1 = digitalRead(7);

  if (pre1 != pres1)
  {
    if (pres1 == HIGH)
    {

      Serial.println("HIGH");



      mySerial.println("AT+CMGF=1");

      delay(50);

      mySerial.println("AT+CMGS=\"+918277795414\"\r");                // Number to send to using AT Commands

      delay(50);

      mySerial.println("Critical Condition");                           // Message To be sent

      delay(50);

      mySerial.println((char)26);                               //ASCII Code for Ctr + Z

      delay(50);


    }
    pre1 = pres1;                  // Update Previous & Present State
  }
}
void updateSerial()
{

  delay(50);
  while (Serial.available())
  {
    mySerial.write(Serial.read());           // Forward what Serial received to Software Serial Port
  }
  while (mySerial.available())
  {
    Serial.write(mySerial.read());
    textMessage_1 = mySerial.readString();   // Forward what Software Serial received to Serial Port
  }
  Serial.print(textMessage_1);
  func(textMessage_1);
  delay(50);

}
}
