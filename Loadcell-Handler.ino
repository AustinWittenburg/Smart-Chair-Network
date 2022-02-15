#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

HX711 scale;

void setup() {
  //Serial.begin(57600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  pinMode(0,OUTPUT);
}

void loop() {

  if (scale.is_ready()) {
    long reading = scale.read();
    //Serial.print("HX711 reading: ");
    //Serial.println(reading);
    if (scale.read() < 0){
    digitalWrite(0,HIGH);
    } 
    else {
    //Serial.println("HX711 not found.");
    digitalWrite(0,LOW);
    }
  }

  delay(1000);
  delay(1000);
  delay(1000);
  delay(1000);
  delay(1000);


}
