#include "Adafruit_seesaw.h"

Adafruit_seesaw ss;
float soilMoisture = 0;

void setup() {
  Serial.begin(115200);

  Serial.println("seesaw Soil Sensor example!");
  
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while(1) delay(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }
}

void loop() {
  float tempC = ss.getTemp();
  uint16_t capread = ss.touchRead(0);
  soilMoisture = ss.touchRead(0)*(-0.0683) + 80.512;

  Serial.print("\n Temperature: "); Serial.print(tempC); Serial.println("*C");
  Serial.print("Capacitive: "); Serial.println(capread);
  Serial.print("Approx Mositure: ");
  Serial.print(soilMoisture);
  delay(100);
}
