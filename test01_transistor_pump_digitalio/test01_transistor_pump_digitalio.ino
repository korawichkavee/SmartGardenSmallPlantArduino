const int pump  =  2;     //use digital I/O pin

void setup() {
  // put your setup code here, to run once:
  pinMode(pump,OUTPUT);   //set pin to be an output output
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(6000);   	     //milliseconds 
  digitalWrite(pump,HIGH);   //turning on
  delay(1000);   	     //milliseconds 
  digitalWrite(pump,LOW);    //turning off

}