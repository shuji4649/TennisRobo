#include <Arduino.h>

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

String MACadd = "48:31:B7:3F:D2:38";
uint8_t address[6]  = {0x48, 0x31, 0xB7, 0x3F, 0xD2, 0x38};
String name="TennisRobo";
bool connected;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("TennisRobo", true); 
  Serial.println("device start");
  
  connected = SerialBT.connect(name);
  if(connected) {
    Serial.println("Connect OK");
  } else {
    while(!SerialBT.connected(10000)) {
      Serial.println("No connect"); 
    }
  }
  
  if (SerialBT.disconnect()) {
    Serial.println("Disconnected Succesfully!");
  }

  SerialBT.connect();

  pinMode(22, INPUT_PULLUP);

}



void loop() {
  if (digitalRead(22) == LOW) {
    Serial.println("LED is ON.");
    SerialBT.write('1');
  }
  if (digitalRead(22) == HIGH) {
    Serial.println("LED is OFF.");
    SerialBT.write('2');
  }
  delay(20);
}