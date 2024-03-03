#include <Arduino.h>
#include <BLE_Kit4C3.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

const char *ssid = "TennisRobo";
const char *password = "shuji4649";
WiFiServer server(80);



//BLE_Peripheral ble("TennisRobo");
int motor[6] = {3, 4, 5, 6, 20, 10};
int servo_pin = 2;
Servo servo;
uint8_t baseMac[6];
char baseMacChr[18] = {0};
void setup()
{
  // put your setup code here, to run once:
  Serial0.begin(115200);
  servo.attach(servo_pin);
  for (int i = 0; i < 6; i++)
  {
    pinMode(motor[i], OUTPUT);
  }


  // ble.init();
  // SerialBT.begin("TennisRobo");
  while(!WiFi.softAP(ssid, password)){
  }
  IPAddress IP = WiFi.softAPIP();
  Serial0.println(IP);
  server.begin();
  Serial0.println("Server started");



}
void StopMotor()
{
  for (int i = 0; i < 6; i++)
  {
    analogWrite(motor[i], 0);
  }
}
void RightMotor(){
  analogWrite(motor[0], 200);
  analogWrite(motor[1], 0);

}
void LeftMotor(){
  analogWrite(motor[0], 0);
  analogWrite(motor[1], 200);
}
void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial0.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial0.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<h1>Tennis Robot</h1>");
            client.print("Right : <a href=\"/H\">here!</a><br>");
            client.print("Left  : <a href=\"/L\">here!</a><br>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
        if (currentLine.endsWith("GET /H")) {
          Serial0.println("GET /H.");
          RightMotor();

        }
        if (currentLine.endsWith("GET /L")) {
          Serial0.println("GET /L.");
          LeftMotor();
        }
      }
    }
    client.stop();
    Serial0.println("Client Disconnected.");
  }
}

// void loop()
// {

//   if(SerialBT.available()){
//     char data = SerialBT.read();
//     if (data == '1')
//     {
//       servo.write(0);
//       analogWrite(motor[0], 255);
//       analogWrite(motor[1], 0);
//       // char sendDataArr[] = "wow!";
//       // int dataSize = sizeof(sendDataArr) / sizeof(sendDataArr[0]);

//       // ble.write(sendDataArr, dataSize);
//     }
//     if (data == '2')
//     {
//       servo.write(90);
//       analogWrite(motor[0], 0);
//       analogWrite(motor[1], 255);
//       // char sendDataArr[] = "yeah!";
//       // int dataSize = sizeof(sendDataArr) / sizeof(sendDataArr[0]);
//       // ble.write(sendDataArr, dataSize);
//     }
//     if (data == '3')
//     {
//       servo.write(0);
//       analogWrite(motor[0], 0);
//       analogWrite(motor[1], 0);
//     }
//   }


//   if (ble.checkConnection())
//   {
//     while (ble.available() != 0)
//     {
//       char data = ble.read();
//       if (data == '1')
//       {

//         StopMotor();
//         analogWrite(motor[0], 255);
//         analogWrite(motor[1], 0);
//         // char sendDataArr[] = "wow!";
//         // int dataSize = sizeof(sendDataArr) / sizeof(sendDataArr[0]);

//         // ble.write(sendDataArr, dataSize);
//       }
//       if (data == '2')
//       {

//         StopMotor();
//         analogWrite(motor[0], 0);
//         analogWrite(motor[1], 255);
//         // char sendDataArr[] = "yeah!";
//         // int dataSize = sizeof(sendDataArr) / sizeof(sendDataArr[0]);
//         // ble.write(sendDataArr, dataSize);
//       }
//       if (data == '3')
//       {

//         StopMotor();
//       }
//     }
//   }
// }
