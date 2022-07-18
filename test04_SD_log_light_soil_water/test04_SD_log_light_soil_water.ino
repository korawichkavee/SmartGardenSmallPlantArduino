#include "SD.h"
#include <Wire.h>
#include "RTClib.h"

#include "Adafruit_seesaw.h"
Adafruit_seesaw ss;

// A simple data logger for the Arduino analog pins
#define LOG_INTERVAL  5000 // mills between entries
#define ECHO_TO_SERIAL   1 // echo data to serial port
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

void error(char *str) //when something Really Bad happened, like we couldn't write to the SD card or open it
{
  Serial.print("error: ");
  Serial.println(str);
  
  // red LED indicates error
  digitalWrite(redLEDpin, HIGH);
  
  while(1);
}

void setup(void) {
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

  // create a new file
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
  
  if (! logfile) {
    error("couldnt create file");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);

  //Establish connection with real time clock
  Wire.begin();  
  if (!rtc.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  //ECHO_TO_SERIAL
  }

  //Set the time and date on the real time clock if necessary
  if (! rtc.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  //HEADER
  logfile.println("Unix Time (s),Date,Soil Temp (C),Soil Moisture (%),Sunlight Illumination (lux),Watering?");    
#if ECHO_TO_SERIAL
  Serial.println("Unix Time (s),Date,Soil Temp (C),Soil Moisture (%),Sunlight Illumination (lux),Watering?");
#endif ECHO_TO_SERIAL// attempt to write out the header to the file

  now = rtc.now();
}
 
void loop(void) {
  //delay software
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));

  //Reset wateredToday variable if it's a new day
  if ( !(now.day() == rtc.now().day()) ) {
    wateredToday = 0;
    recent_water_h = 0;
  }
  
  now = rtc.now();
  
  // log time
  logfile.print(now.unixtime());
  logfile.print(",");
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
 #if ECHO_TO_SERIAL
  Serial.print(now.unixtime());
  Serial.print(",");
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
#endif //ECHO_TO_SERIAL

 //Collect Variables
  float soilTemp = ss.getTemp();
  delay(20);
  
  float soilMoisture = ss.touchRead(0)*(-0.0683) + 80.512;
  delay(20);

  photocellReading = analogRead(photocellPin);  
  sunlight = photocellReading*0.8269-80.846 ; //LUX
  delay(20);
  
  //Log variables
  logfile.print(soilTemp);
  logfile.print(",");
  logfile.print(soilMoisture);
  logfile.print(",");
  logfile.print(sunlight);
  logfile.print(",");
#if ECHO_TO_SERIAL
  Serial.print(soilTemp);
  Serial.print(",");
  Serial.print(soilMoisture);
  Serial.print(",");
  Serial.print(sunlight);
  Serial.print(",");
#endif

  if ((soilMoisture < wateringThreshold) && (wateredToday < 2 ) && (sunlight > 200) && (now.hour()- recent_water_h > 6 )) {
    digitalWrite(pump,HIGH);   //turning on
    delay(30000);   	     //milliseconds 
    digitalWrite(pump,LOW);    //turning off
    recent_water_h = now.hour();
  
    //record that we're watering
    ++wateredToday;
    logfile.print(wateredToday);
    #if ECHO_TO_SERIAL
      Serial.print(wateredToday);
    #endif
  }
  else {
    logfile.print(wateredToday);
    #if ECHO_TO_SERIAL
      Serial.print(wateredToday);
    #endif
  }
  
  logfile.println();
#if ECHO_TO_SERIAL
  Serial.println();
#endif
  delay(50);
  
  //Write to SD card
  logfile.flush();
  delay(5000);
}
