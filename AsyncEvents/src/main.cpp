// ******************************************************************
//
// 
#include <Arduino.h>
#include <ESP8266WiFi.h> 
#include <secrets.h>
#include "ThingSpeak.h" // thingspeak header file 


 
// access credentials for WiFi network. 
const char* ssid = "Mohawk-IoT";
const char* password = "IoT@MohawK1";

WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;


//pin assignments for external LEDs
#define PIN_LED_GREEN D1
#define PIN_LED_YELLOW D2
#define PIN_LED_RED D3
//pin assignment for PIR input
#define PIN_PIR D5
// true when external event occurred
volatile bool bEventOccured = false;

void setup() {

 // setup LED outputs
 pinMode(PIN_LED_GREEN, OUTPUT);
 pinMode(PIN_LED_YELLOW, OUTPUT);
 pinMode(PIN_LED_RED, OUTPUT);
 // setup PIR input
 pinMode(PIN_PIR, INPUT);
 // built-in LED
 pinMode(LED_BUILTIN, OUTPUT);

Serial.begin(115200); 
Serial.println("AsyncEvents"); 
Serial.println("Yizhe Feng 000798153");


//When your application starts, Turn off all the LEDs, except YELLOW. This indicates that a WiFi connection is not yet available.
digitalWrite(PIN_LED_GREEN, LOW);
digitalWrite(PIN_LED_YELLOW, HIGH);
digitalWrite(PIN_LED_RED, LOW);


WiFi.mode(WIFI_STA); 

ThingSpeak.begin(client);  // Initialize ThingSpeak

// Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){

    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password);  
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  //When the WiFi connection has been established, turn off the YELLOW LED and turn on the GREEN LED. 
  if (WiFi.status() == WL_CONNECTED){
  digitalWrite(PIN_LED_GREEN, HIGH);
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_RED, LOW);
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP().toString().c_str());
 }
}


// isr for PIR sensor input
void IRAM_ATTR isr() {
  bEventOccured = true;  // set flag
}


void loop() {

  //If the WiFi connection is lost in the future, turn on YELLOW and turn off GREEN.
  if (WiFi.status() != WL_CONNECTED) {
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_YELLOW, HIGH);
  digitalWrite(PIN_LED_RED, LOW);
  Serial.println("WiFi disconnected");
  }else{
    //detect the motion when wifi is connected

     //attach interrupt to PIR sensor
     attachInterrupt(digitalPinToInterrupt(D5), isr, RISING);

   // if motion detected
   if (bEventOccured) {
     Serial.println("Motion Detected");
     // turn on RED LED
     digitalWrite(PIN_LED_RED, HIGH);
     delay(2000);
     //Log “1” to ThingSpeak channel
     int x = ThingSpeak.writeField(myChannelNumber, 1, 1, myWriteAPIKey);
      if(x == 200){
        Serial.println("Channel update successful.");
      }
      else{
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
      // turn off RED LED after 2 seconds
     digitalWrite(PIN_LED_RED, LOW);
     bEventOccured = false;
  }
  }





























 


}