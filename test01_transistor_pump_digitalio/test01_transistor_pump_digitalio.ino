const int pump  =  2;     //use digital I/O pin

int watering_dur  = 1*1000 ;
int wait_dur  =  6*1000;

void setup() {
  // put your setup code here, to run once:
  pinMode(pump,OUTPUT);   //set pin to be an output output
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(wait_dur);   	     //milliseconds 
  digitalWrite(pump,HIGH);   //turning on
  delay(watering_dur);   	     //milliseconds 
  digitalWrite(pump,LOW);    //turning off

}