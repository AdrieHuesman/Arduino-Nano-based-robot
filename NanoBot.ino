/*
NanoBot
Adrie Huesman
13 September 2021

Hardware
Arduino Nano Atmega328
Arduino Nano sensor I/O shield v3.0
4 Alkaline batteries so 6 volts
2 SG90 continous or 360 degrees servos
Wheel for Standard Servo 25T-40×7mm 2-Pack Pololu 4905
Ball Caster with 3/4″ Plastic Ball Pololu 954
QTR-3A Reflectance Sensor Array Pololu 2456
Powerboard, own design based on 5V Step-Up/Step-Down Voltage Regulator S7V7F5 Pololu 2119
SG90 270 degrees servo and ultrasonic distance sensor HC-SR04
Chassis, own design 4mm aviation plywood 13 by 8 cm (two levels)

Software

I/O
D0 --
D1 --
D2 --
D3 --
D4 --
D5 Continuous servo left
D6 Continuous servo right
D7 --
D8 --
D9 --
D10 --
D11 --
D12 --
D13 --
A0 Sensor array left
A1 Sensor array center
A2 Sensor array right
A3 --
A4 --
A5 --
A6 --
A7 --

Other
SensorValues when all black 900 - 924, say 900
SensorValues when all white 488 - 579, say 600
 */

// Declare variables
int sensorValueL;     // variable to store the value coming from the left sensor
int sensorValueC;     // variable to store the value coming from the center sensor
int sensorValueR;     // variable to store the value coming from the right sensor
int Error;
float Kc = 0.40;      // Controller gain (0.40)
float Paction;        
int Avspeed = 200;    // Average speed (150)    

#include <Servo.h> 
Servo Lservo;
Servo Rservo;

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

void setup() 
{ 
  Serial.begin(9600);   // set up Serial library at 9600 bps
  Lservo.attach(5);
  Rservo.attach(6);
} 

void loop()
{
  sensorValueL = analogRead(A0);
  sensorValueC = analogRead(A1);
  sensorValueR = analogRead(A2);
  Error = sensorValueL - sensorValueR;
  Paction = Kc*Error;
  Lspeed(Avspeed - int(Paction));
  Rspeed(Avspeed + int(Paction) + 40);  // Motors behave different
  delay(10);
}
