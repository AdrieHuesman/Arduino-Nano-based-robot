/*
NanoRoverC
Adrie Huesman
Start: 26 January 2022
Last update: 26 Feb 2022

Hardware
Arduino Nano Atmega328
Arduino Nano sensor I/O shield v3.0
4 Alkaline batteries so 6 volts
2 SG90 continous or 360 degrees servos
Wheel for Standard Servo 25T-40×7mm 2-Pack Pololu 4905
Ball Caster with 3/4″ Plastic Ball Pololu 954
QTR-3A Reflectance Sensor Array Pololu 2456
Powerboard, own design based on 5V Step-Up/Step-Down Voltage Regulator S7V7F5 Pololu 2119
SG90 270 degrees servo and ultrasonic (US) distance sensor HC-SR04
2 Digital infrared proximity sensors FC-51 or HW-201
Chassis, own design 4mm aviation plywood 13 by 8 cm (two levels)

Software

I/O
D0 -- Reserved UART Rx
D1 -- Reserved UART Tx
D2 Right proximity sensor
D3 Left proximity sensor
D4 --
D5 Continuous servo left
D6 Continuous servo right
D7 --
D8 --
D9 270 degrees servo
D10 --
D11 --
D12 HC-SR04 Trig
D13 HC-SR04 Echo
A0 Sensor array left
A1 Sensor array center
A2 Sensor array right
A3 --
A4 -– Reserved I2C SDA
A5 -- Reserved I2C SDL
A6 -- 
A7 Potmeter to set bias between motor speeds

Other
SensorValues when all black 900 - 924, say 900
SensorValues when all white 488 - 579, say 600

Same RPMs achieved when:
Forward Lspeed 200 Rspeed 240
Backward Lspeed -250 Rspeed -200 
Left turn Lspeed -200 Rspeed 200
Right turn Lspeed 200 Rspeed -200
*/

// Declare variables
int Avspeed = 200;   // Only used in MFO state, typical value 200
int val;   // Value from potentiometer
int bias;   // To run left and right motor at same speed (33 - 34)
enum state
{
  INI,                
  SAM,
  MFO,                
  T90,
  T15,
  ESC
};
state Robotstate = INI;
float Cdistance;
float Climit = 15.0;
float Ldistance;
float Rdistance;
bool LProx;
bool RProx;
bool CProx;

#include <Servo.h>
#include <HCSR04.h>
Servo Lservo;
Servo Rservo;
Servo Cservo;

// Initialize US sensor that uses digital pins 12 and 13
UltraSonicDistanceSensor distanceSensor(12, 13);   // Trigger and echo

// Set speed of left motor, -500 < speed < 500
void Lspeed(int speed) 
{
  Lservo.writeMicroseconds(speed + 1500);    
}

// Set speed of right motor, -500 < speed < 500
void Rspeed(int speed) 
{
  Rservo.writeMicroseconds(1500 - speed);    
}

// Exclusive OR via truth table, see below
// An alternative is to use (a || b) && !(a && b)
bool EOR(bool a, bool b)
{
  bool c;
  if (!a && !b) {c = false;}
  if (a && !b) {c = true;}
  if (!a && b) {c = true;}
  if (a && b) {c = false;}
  return c;
}

void setup() 
{ 
  pinMode(2, INPUT);    // Sets digital pin 2 as input for RProx
  pinMode(3, INPUT);    // sets digital pin 3 as input for LProx
  Serial.begin(9600);   // Set up Serial library at 9600 bps
  Lservo.attach(5);
  Rservo.attach(6);
  Cservo.attach(9);
  Cservo.writeMicroseconds(1430);   // set HC-SR04 at 90 degrees
  delay(500);
  Cservo.writeMicroseconds(2110);   // set HC-SR04 at 0 degrees
  delay(500);
  Cservo.writeMicroseconds(790);   // set HC-SR04 at 180 degrees
  delay(500);
  Cservo.writeMicroseconds(1430);   // set HC-SR04 at 90 degrees
  delay(500);
} 

void loop()
{
  switch(Robotstate)
  {
    case INI:
    Lspeed(0);
    Rspeed(0);
    Cservo.writeMicroseconds(1430);
    delay(100);
    Cdistance = distanceSensor.measureDistanceCm();
    delay(100);    
    Robotstate = SAM;
    break;
 
    case SAM:
    Lspeed(0);
    Rspeed(0);
    Cservo.writeMicroseconds(1430);
    Cdistance = distanceSensor.measureDistanceCm();
    LProx = !(digitalRead(3));   // LProx is true when an object is detected
    RProx = !(digitalRead(2));   // RProx is true when an object is detected
    CProx = (Cdistance < Climit);   // CProx is true when an object is detected 
    if (EOR(LProx, RProx))   // Object detected left or right
    {
      Robotstate = T15;
    }
    if (!LProx && !RProx && CProx)   // No object detected left or right, but object detected in front 
    {
      Robotstate = T90;
    }
    if (!LProx && !RProx && !CProx)   // No object detected left or right or in front
    {
      Robotstate = MFO;
    }
    if (LProx && RProx)   // Object detected left and right
    {
      Robotstate = ESC;
    }
    break;

    case MFO:
    val = analogRead(A7);
    bias = map(val, 0, 1023, -100, 100);
    // Serial.println(bias);
    Lspeed(Avspeed - bias);
    Rspeed(Avspeed + bias);
    delay(100);   // Allows for LProx and RProx to become true simultaneously
    Cservo.writeMicroseconds(1430);
    Cdistance = distanceSensor.measureDistanceCm();
    LProx = !(digitalRead(3));   // LProx is true when an object is detected
    RProx = !(digitalRead(2));   // RProx is true when an object is detected
    CProx = (Cdistance < Climit);   // CProx is true when an object is detected 
    if (!LProx && !RProx && !CProx)   // No object detected left or right or in front
    {
      Robotstate = MFO;
    }
    if (LProx || RProx || CProx)   // Object detected left or right or in front
    {
      Robotstate = SAM;
    }
    break;

    case T90:
    Lspeed(0);
    Rspeed(0);
    Cservo.writeMicroseconds(1430);
    delay(500);
    Cservo.writeMicroseconds(2110);
    delay(500);
    Ldistance = distanceSensor.measureDistanceCm();
    Cservo.writeMicroseconds(790);
    delay(500);
    Rdistance = distanceSensor.measureDistanceCm();
    if (Ldistance >= Rdistance)   // turn left
    {
      Lspeed(-200);
      Rspeed(200);
      delay(600);
    }
    if (Ldistance < Rdistance)   // turn right
    {
      Lspeed(200);
      Rspeed(-200);
      delay(600);
    }
    Robotstate = SAM;
    break;

    case T15:
    Lspeed(0);
    Rspeed(0);
    if (LProx)   // turn right
    {
      Lspeed(200);
      Rspeed(-200);
      delay(100);
    }
    if (RProx)   // turn left
    {
      Lspeed(-200);
      Rspeed(200);
      delay(100);
    }
    Robotstate = SAM;
    break;

    case ESC:
    Lspeed(0);
    Rspeed(0);
    Lspeed(-200);
    Rspeed(-200);
    delay(1200);
    Lspeed(0);
    Rspeed(0);
    Lspeed(200);
    Rspeed(-200);
    delay(1200);
    Lspeed(0);
    Rspeed(0);
    Robotstate = SAM;
    break;

  }
}
