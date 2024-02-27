
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include "SoftwareSerial.h"
#include<TinyGPS++.h>

#define LM35 A1 // Temperature sensor

//Pulse sensor
const int PulseWire = 0;
int Threshold = 550;
const int thresholdBPM = 80;
PulseSensorPlayground pulseSensor;

//Vibration Sensor
int vs = A3; 

//GSM
SoftwareSerial mySerial(7, 8); 
String cmd = "";

//GPS
static const int RXPin = 3, TXPin = 2;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;


SoftwareSerial ss(RXPin, TXPin);//GPS

void setup()
{
 

  pinMode(12,INPUT_PULLUP);//BUTTON
 
  mySerial.begin(115200);
  Serial.begin(115200);
  
   
  Serial.println("Initializing...");
  delay(1000);

  mySerial.println("AT");                 // Sends an ATTENTION command, reply should be OK
  updateSerial();
  mySerial.println("AT+CMGF=1");          // Configuration for sending SMS
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0");  // Configuration for receiving SMS
  updateSerial();
   ss.begin(GPSBaud);

  //Pulse Sensor
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.setThreshold(Threshold);


  //vibration sensor
  pinMode(vs, INPUT);

}

void loop()
{
  gpsm();
  checking_value_function();
 

  
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {

    cmd+=(char)Serial.read();
 
    if(cmd!=""){
      cmd.trim();  // Remove added LF in transmit
      if (cmd.equals("S")) {
        sendSMS();
      } else {
        mySerial.print(cmd);
        mySerial.println("");
      }
    }
  }
  
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}



void gpsm(){                       ////MAIN GPS INTERFACING
  
    while(ss.available()>0){
      

     
    gps.encode(ss.read());
    if(gps.location.isUpdated()){
      Serial.println("onn");
     delay(1000);
      Serial.print("Latitude: ");
      Serial.print(gps.location.lat(),6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(),6);
      String c=String(gps.location.lng(),6);
       String b=String(gps.location.lat(),6);
       String d=("https://maps.google.com/?q="+b+","+c);
      Serial.println(d);
      button(d);
      delay(10000);
    }
    
  }
  
   
}






void button(String y){
                                                  //////BUTTON FUNCTION
   Serial.println("start"); 
  int a=digitalRead(10);
  if(a==LOW){
     Serial.println("active");
    sendSMS1(y);
    digitalWrite(8,HIGH);
  }
  else{
    Serial.println("not active");
  }
  delay(1000);

  
}



void sendSMS1(String x){                                ///GSMMM IMPORTANT 
  mySerial.println("AT+CMGF=1");
  delay(500);
  mySerial.println("AT+CMGS=\"+917483932110\"\r");
  delay(500);
  mySerial.print(x);
  delay(500);
  mySerial.write(26);


}
void sendSMS(){
  mySerial.println("AT+CMGF=1");
  delay(500);
  mySerial.println("AT+CMGS=\"+917483932110\"\r");
  delay(500);
  mySerial.print("help");
  delay(500);
  mySerial.write(26);


}

// ------------------------------- SENSORS ---------------------------------------

void pulse_sensor() {
  if (pulseSensor.sawStartOfBeat()) {            
    int myBPM = pulseSensor.getBeatsPerMinute();                  
  }

  delay(20);
}

//---------------------------------------------------------------------------------

void vibration_sensor(){
  long measurement =vibration();
  delay(50);
}

long vibration(){
  long measurement=pulseIn (vs, HIGH);  //wait for the pin to get HIGH and returns measurement
  return measurement;
  }


//---------------------------------------------------------------------------------

void checking_value_function(){
  long measurement = vibration();
  float lmvalue = analogRead(LM35);
  if(pulseSensor.getBeatsPerMinute() > thresholdBPM && measurement>7000){
    Serial.println("Pulse and Vibration high!");
    
    gpsm1();
  }
  else if(pulseSensor.getBeatsPerMinute() > thresholdBPM && lmvalue > 100){
    Serial.println("Pulse and Temperature high!");
    gpsm1();
  }
  else if(measurement > 7000 && lmvalue > 100){
    Serial.println("Vibration and Temperature high!");
    gpsm1();
  }
  else{
    Serial.println("No worries!");
  }
}

//---------------------------------------------------------------------------------

void help_call_function(){

  Serial.println("HELPPP----------!!!!!!!");
  delay(2500);


}




void gpsm1(){                       ////MAIN GPS INTERFACING
  
    while(ss.available()>0){
      

     
    gps.encode(ss.read());
    if(gps.location.isUpdated()){
      Serial.println("onn");
     delay(1000);
      Serial.print("Latitude: ");
      Serial.print(gps.location.lat(),6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(),6);
      String c=String(gps.location.lng(),6);
       String b=String(gps.location.lat(),6);
       String d=("https://maps.google.com/?q="+b+","+c);
      Serial.println(d);
      sendSMS1(d);
      delay(10000);
    }
    
  }
  
   
}