/* Photocell simple testing sketch. 
 
Connect one end of the photocell to 5V, the other end to Analog x.
Then connect one end of a 1K resistor from Analog x to ground
 
For more information see http://learn.adafruit.com/photocells */
 
int photocellPin = 2;     // the cell and pulldown are connected to aX
int photocellReading;     // the analog reading from the analog resistor divider
 
void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
}
 
void loop(void) {
  photocellReading = analogRead(photocellPin);  
 
  Serial.print("\n Analog reading = ");
  Serial.print(photocellReading);     // the raw analog reading
 
  // We'll have a few threshholds, qualitatively determined
  if (photocellReading < 30) {
    Serial.println(" - Dark");
  } else if (photocellReading < 160) {
    Serial.println(" - Dim");
  } else if (photocellReading < 200) {
    Serial.println(" - Light");
  } else if (photocellReading < 400) {
    Serial.println(" - Bright");
  } else {
    Serial.println(" - Very bright");
  }

  Serial.print("\n in LUX = ");
  Serial.print(photocellReading*0.8269-80.846);
  Serial.print("\n *****");
  delay(1000);
}