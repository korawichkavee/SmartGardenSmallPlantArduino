#include "SD.h" //SD card shield
#include <Wire.h> //to communicate with I2C/TWI devices
#include "RTClib.h" //Real Time Clock

#include "Adafruit_seesaw.h"
Adafruit_seesaw ss;

// A simple data logger for the Arduino analog pins
#define LOG_INTERVAL  3*1000 // mills between entries

// the digital pins that connect to the LEDs
#define redLEDpin 3 //LED on the logger

int photocellPin = 2;     // the cell and pulldown are connected to aX
int photocellReading;     // the analog reading from the analog resistor divider
float sunlight = 0; //Sunlight illumination in lux
const int pump  =  2;     //use digital I/O pin
int wateredToday = 0;
int recent_water_h = 0;
float soilTemp = 0; 
float soilMoisture = 0;
const float wateringThreshold = 25; //Value below which the garden gets watered

RTC_PCF8523 rtc; // define the Real Time Clock object
DateTime now;

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;

void setup() {
  // We'll send debugging information via the Serial monitor
  Serial.begin(115200);   
  pinMode(pump,OUTPUT);   //set pin to be an output output

  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while(1) delay(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }

  // initialize the SD card
  Serial.print("\n Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  //Establish connection with real time clock
  Wire.begin();  
  if (!rtc.begin()) {
    Serial.println("RTC failed");
  }
  delay(LOG_INTERVAL/2);

  //Set the time and date on the real time clock if necessary
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  now = rtc.now();
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  Serial.print("Logging to: ");
  Serial.println(filename);
  delay(20);
    
  //HEADER 
  Serial.println("Date,Soil Temp (C),Soil Moisture (%),Sunlight Illumination (lux),Watering?");
  logfile.println("Date,Soil Temp (C),Soil Moisture (%),Sunlight Illumination (lux),Watering?");
  delay(LOG_INTERVAL/2);

}
 
void loop() {
  //delay software
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));

  //Reset wateredToday variable if it's a new day
  if ( !(now.day() == rtc.now().day()) ) {
    wateredToday = 0;
    recent_water_h = 0;
  }
  delay(20);

  //Collect Variables
  float soilTemp = ss.getTemp();
  delay(20);
  
  float soilMoisture = ss.touchRead(0)*(-0.0683) + 80.512;
  delay(20);

  photocellReading = analogRead(photocellPin);  
  sunlight = photocellReading*0.8269-80.846 ; //LUX
  delay(20);

  now = rtc.now();
  delay(20);
  
  // log time
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(" ");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
  logfile.print(",");
  delay(20);
  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print(",");
  delay(20);
    
  //Log variables
  logfile.print(soilTemp);
  logfile.print(",");
  logfile.print(soilMoisture);
  logfile.print(",");
  logfile.print(sunlight);
  logfile.print(",");
  delay(20);
  Serial.print(soilTemp);
  Serial.print(",");
  Serial.print(soilMoisture);
  Serial.print(",");
  Serial.print(sunlight);
  Serial.print(",");
  delay(20);

  if ((soilMoisture < wateringThreshold) && (wateredToday < 3 ) && (sunlight > 150) && (now.hour()- recent_water_h > 4 )) {
    digitalWrite(pump,HIGH);   //turning on
    delay(60000);   	     //milliseconds 
    digitalWrite(pump,LOW);    //turning off
    recent_water_h = now.hour();
    //record that we're watering
    ++wateredToday;
  }
  logfile.print(wateredToday);
  Serial.print(wateredToday);
  delay(20);

  logfile.println();
  Serial.println();
  delay(20);
  
  //Write to SD card
  logfile.flush();
  //logfile.close();
  Serial.println("data is saved");
  delay(20);
}
