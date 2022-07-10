int photocellPin = 2;     // the cell and pulldown are connected to aX
int photocellReading;     // the analog reading from the analog resistor divider
float sunlight = 0; //Sunlight illumination in lux
const int pump  =  2;     //use digital I/O pin

void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(115200);   
  pinMode(pump,OUTPUT);   //set pin to be an output output
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
  sunlight = photocellReading*0.8269-80.846 ;
  Serial.print(sunlight);
  Serial.print("\n *****");

  if (sunlight < 30) { // if it is dark
    digitalWrite(pump,HIGH);   //turning on
    delay(1000);   	     //milliseconds 
    digitalWrite(pump,LOW);    //turning off
    delay(5000);   	     //milliseconds 
  } else {
    delay(1000);
  }
}