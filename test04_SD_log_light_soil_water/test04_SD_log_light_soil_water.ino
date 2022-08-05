#include "SD.h" //SD card shield
#include <Wire.h> //to communicate with I2C/TWI devices
#include "RTClib.h" //Real Time Clock

#include "Adafruit_seesaw.h"
Adafruit_seesaw ss;

// A simple data logger for the Arduino analog pins
#define LOG_INTERVAL  10000 // mills between entries
//#define WAIT_TO_START    0 // Wait for serial input in setup()

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
const float wateringThreshold = 20; //Value below which the garden gets watered

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

  // initialize the SD card
  Serial.print("\n Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  if (! SD.exists("smart_garden.txt")) {
    // only open a new file if it doesn't exist
    Serial.println("creating a new file");
    logfile = SD.open("smart_garden.csv", FILE_WRITE); 
  } else {
    Serial.println("Same file already exists");
    logfile = SD.open("smart_garden.txt");
  }

  //Establish connection with real time clock
  Wire.begin();  
  if (!rtc.begin()) {
    logfile.println("RTC failed");
    Serial.println("RTC failed");
  }
  delay(LOG_INTERVAL/2);
  //Set the time and date on the real time clock if necessary
  //if (! rtc.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //}
  
  //HEADER 
  Serial.println("Date,Soil Temp (C),Soil Moisture (%),Sunlight Illumination (lux),Watering?");
  logfile.println("Date,Soil Temp (C),Soil Moisture (%),Sunlight Illumination (lux),Watering?");
  delay(LOG_INTERVAL/2);
  now = rtc.now();
}
 
void loop() {
  //delay software
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));

  //Reset wateredToday variable if it's a new day
  if ( !(now.day() == rtc.now().day()) ) {
    wateredToday = 0;
    recent_water_h = 0;
  }

  //Collect Variables
  float soilTemp = ss.getTemp();
  delay(20);
  
  float soilMoisture = ss.touchRead(0)*(-0.0683) + 80.512;
  delay(20);

  photocellReading = analogRead(photocellPin);  
  sunlight = photocellReading*0.8269-80.846 ; //LUX
  delay(20);

  now = rtc.now();
  
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
  delay(100);
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
  delay(100);
    
  //Log variables
  logfile.print(soilTemp);
  logfile.print(",");
  logfile.print(soilMoisture);
  logfile.print(",");
  logfile.print(sunlight);
  logfile.print(",");
  
  Serial.print(soilTemp);
  Serial.print(",");
  Serial.print(soilMoisture);
  Serial.print(",");
  Serial.print(sunlight);
  Serial.print(",");

  if ((soilMoisture < wateringThreshold) && (wateredToday < 2 ) && (sunlight > 400) && (now.hour()- recent_water_h > 6 )) {
    digitalWrite(pump,HIGH);   //turning on
    delay(30000);   	     //milliseconds 
    digitalWrite(pump,LOW);    //turning off
    recent_water_h = now.hour();
    //record that we're watering
    ++wateredToday;
  }
  logfile.print(wateredToday);
  Serial.print(wateredToday);
  
  logfile.println();
  Serial.println();
  
  //Write to SD card
  //logfile.flush();
  logfile.close();
}
