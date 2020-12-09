#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "MAX30100_PulseOximeter.h"

#define BLYNK_PRINT Serial
#include <Blynk.h>

#include <BlynkSimpleEsp32.h>
//#include <ESP8266WiFi.h>
//wifi id passwerd initilisation and authentication key
char auth[] = "-5ZwsQielaJIEHo7DnE2IRekxuN8odxJ";
char ssid[] = "abcd";
char pass[] = "password";
// Task handles for different cores

TaskHandle_t Task1;
TaskHandle_t Task2;

//variable initilisation
int val;
#define REPORTING_PERIOD_MS     1000
//BlynkTimer timer;
int BPM;
int SpO2;
int per;
int pulse;
PulseOximeter pox;
uint32_t tsLastReport = 0;
float Fahrenheit = 0;
const int oneWireBus = 4;//GPIO
float Celcius = 0;

int critical = 50; //can vary from person to person
OneWire oneWire(oneWireBus);

DallasTemperature sensors(&oneWire);

WidgetLCD lcd(V5);//blynk lcd display initlisation

void onBeatDetected()
{
  Serial.println("Beat!");
}


void setup()
{
  // bly.clear();
  Wire.begin();
  sensors.begin();
  pinMode(18, INPUT); //lo+ pin of ecg module
  pinMode(19, INPUT); //Lo- pin of ecg module
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 100, 10), 8080);//used to connect to local server at the specified ip address

  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  delay(500);

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);

}


void Task1code( void * pvParameters ) {
  if ((digitalRead(18) == 1) || (digitalRead(19) == 1));
  else {
    pulse = analogRead(A0);
    sensors.requestTemperatures();

    Fahrenheit = sensors.toFahrenheit(Celcius);
    Blynk.virtualWrite(V3, Fahrenheit);
    Blynk.virtualWrite(V4, pulse);
  }
  if (BPM < critical) {
    lcd.clear();
    lcd.print(4, 0, "id:1 critical"); //same for multiple patiensts
  }

}


void Task2code( void * pvParameters ) {
  pox.update();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
  Blynk.virtualWrite(V1, BPM);
  Blynk.virtualWrite(V2, SpO2);
  /*For visulising in serial monitor
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
    Serial.print("Heart rate:");
    Serial.print(pox.getHeartRate());
    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    Serial.println("%");
    tsLastReport = millis();
    }*/

  Blynk.run();
}


void loop()
{
}
