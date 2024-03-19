#include <Arduino.h>
#include <BLE_Kit4C3.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

const char *ssid = "TennisRobo";
const char *password = "shuji4649";
WiFiServer server(80);

// BLE_Peripheral ble("TennisRobo");
int motor[6] = {3, 4, 8,10, 6, 5};
int servo_pin = 2;

uint8_t baseMac[6];
char baseMacChr[18] = {0};

bool AutoMove = false;
void MotorRotate(int num, int speed)
{
  if (speed > 0)
  {
    analogWrite(motor[2 * num], speed);
    analogWrite(motor[2 * num + 1], 0);
  }
  else if (speed == 0)
  {
    analogWrite(motor[2 * num], 255);
    analogWrite(motor[2 * num + 1], 255);
  }
  else
  {
    analogWrite(motor[2 * num], 0);
    analogWrite(motor[2 * num + 1], -speed);
  }
}

void ServoControl(int DutyCycle)
{
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(servo_pin, HIGH);
    ets_delay_us(DutyCycle);
    digitalWrite(servo_pin, LOW);
    ets_delay_us(20 * 1000 - DutyCycle);
  }
  delay(100);
}

void StopMotor()
{
  for (int i = 0; i < 3; i++)
  {
    MotorRotate(i, 0);
  }
}

void Forward(int speed)
{
  MotorRotate(0,-speed);
  MotorRotate(1,speed);
  MotorRotate(2,0);
}
void Go120(int speed)
{
  MotorRotate(0,0);
  MotorRotate(1,-speed);
  MotorRotate(2,speed);
}
void Go240(int speed)
{
  MotorRotate(0,speed);
  MotorRotate(1,0);
  MotorRotate(2,-speed);
}
void MoveLR(int speed)
{
  if (speed > 0)
  {
    MotorRotate(0, -speed);
    MotorRotate(1, 0);
    MotorRotate(2, speed * 1.3);
  }
  else
  {
    MotorRotate(0, -speed);
    MotorRotate(1, 0);
    MotorRotate(2, speed * 1.55);
  }
}
void RightMove(int speed)
{
  MotorRotate(0,speed/2);
  MotorRotate(1,speed/2);
  MotorRotate(2,-speed);
}
void LeftMove(int speed)
{
  MotorRotate(0,-speed/2);
  MotorRotate(1,-speed/2);
  MotorRotate(2,speed);
}
void RightRotate(int speed)
{
  for (int i = 0; i < 3; i++)
  {
    MotorRotate(i, speed);
  }
}
void LeftRotate(int speed)
{
  for (int i = 0; i < 3; i++)
  {
    MotorRotate(i, -speed);
  }
}
void SwingRacket()
{
  ServoControl(1200);
  delay(500);
  ServoControl(550);
  delay(200);
}
void setup()
{
  // put your setup code here, to run once:
  Serial0.begin(115200);
  // servo.attach(servo_pin);
  for (int i = 0; i < 6; i++)
  {
    pinMode(motor[i], OUTPUT);
  }

  StopMotor();
  pinMode(servo_pin, OUTPUT);
  // ble.init();
  // SerialBT.begin("TennisRobo");
  while (!WiFi.softAP(ssid, password))
  {
  }
  IPAddress IP = WiFi.softAPIP();

  server.begin();

  AutoMove = true;
  SwingRacket();
}

int targetX = 40;

void loop()
{

  if (AutoMove)
  {
    int ballX = 0;
    int ballSize = 0;
    if (Serial0.available())
    {
      int data = Serial0.read();
      ballX = data % 100;
      ballSize = data / 100;
      if (ballX == 0)
      {
        StopMotor();
      }

      else if (abs(ballX - targetX) <= 4)
      {
        StopMotor();
        if (ballSize == 1)
        {
          SwingRacket();
          Serial0.flush();
        }
      }
      else
      {
        if (ballX < targetX)
        {
          MoveLR(-160);
        }
        else
        {
          MoveLR(160);
        }
        // MoveLR((ballX - targetX) * 4);
      }
    }
  }
  else
  {
    WiFiClient client = server.available();

    if (client)
    {

      String currentLine = "";
      while (client.connected())
      {
        if (client.available())
        {
          char c = client.read();
          Serial0.write(c);
          if (c == '\n')
          {
            if (currentLine.length() == 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
              client.println("<h1>Tennis Robot</h1>");
              client.print("---SetAutoMove---<br>");
              client.print("AutoMove  : <a href=\"/au\">here</a><br>");
              client.print("StopAuto  : <a href=\"/sa\">here</a><br>");
              client.print("<br>");
              client.println("---motor control---<br>");
              client.print("RightRotate : <a href=\"/rr\">here</a><br>");
              client.print("LeftRotate  : <a href=\"/lr\">here</a><br>");
              client.print("<br>");
              client.print("Forward     : <a href=\"/fw\">here</a><br>");
              client.print("Go120       : <a href=\"/g1\">here</a><br>");
              client.print("Go240       : <a href=\"/g2\">here</a><br>");
              client.print("RightMove   : <a href=\"/rm\">here</a><br>");
              client.print("LeftMove    : <a href=\"/lm\">here</a><br>");
              client.print("Stop        : <a href=\"/sp\">here</a><br>");
              client.print("<br>");
              client.print("---servo control---<br>");
              client.print("Servo180    : <a href=\"/s8\">here</a><br>");
              client.print("Servo90     : <a href=\"/s9\">here</a><br>");
              client.print("Servo0      : <a href=\"/s0\">here</a><br>");
              break;
              client.println();
            }
            else
            {
              currentLine = "";
            }
          }
          else if (c != '\r')
          {
            currentLine += c;
          }
          if (currentLine.endsWith("GET /au"))
          {
            AutoMove = true;
          }
          if (currentLine.endsWith("GET /sa"))
          {
            AutoMove = false;
          }
          if (currentLine.endsWith("GET /rr"))
          {
            RightRotate(50);
          }
          if (currentLine.endsWith("GET /lr"))
          {
            LeftRotate(50);
          }
          if (currentLine.endsWith("GET /fw"))
          {
            Forward(120);
          }
          if (currentLine.endsWith("GET /g1"))
          {
            Go120(120);
          }
          if (currentLine.endsWith("GET /g2"))
          {
            Go240(120);
          }
          if (currentLine.endsWith("GET /rm"))
          {
            MoveLR(120);
          }
          if (currentLine.endsWith("GET /lm"))
          {
            MoveLR(-120);
          }
          if (currentLine.endsWith("GET /sp"))
          {
            StopMotor();
          }
          if (currentLine.endsWith("GET /s9"))
          {
            ServoControl(1450);
          }
          if (currentLine.endsWith("GET /s0"))
          {
            ServoControl(500);
          }
          if (currentLine.endsWith("GET /s8"))
          {
            ServoControl(2400);
          }
        }
      }

      client.stop();
    }
  }
}
