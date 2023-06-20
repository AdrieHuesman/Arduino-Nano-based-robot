/*
SumoB
Adrie Huesman
26 January 2022

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
D0 -- Reserved UART Rx
D1 -- Reserved UART Tx
D2 --
D3 --
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
A7 --

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
int sensorValueL;     // variable to store the value coming from the left sensor
int sensorValueC;     // variable to store the value coming from the center sensor
int sensorValueR;     // variable to store the value coming from the right sensor
int Avspeed = 200;    // Average speed (200)
enum state
{
  INI,                
  TAM,                
  PSH,
  BCK
};
state Robotstate = INI;
float Mdistance;
float Mlimit = 15.0;

#include <Servo.h>
#include <HCSR04.h>
Servo Lservo;
Servo Rservo;
Servo Cservo;

// Initialize sensor that uses digital pins 12 and 13
const byte triggerPin = 12;
const byte echoPin = 13;
UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);

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
    delay(100);
    Mdistance = distanceSensor.measureDistanceCm();
    delay(100);    
    Robotstate = TAM;
    break;
 
    case TAM:
    Lspeed(200);
    Rspeed(-200);
    Mdistance = distanceSensor.measureDistanceCm();
    while (Mdistance > Mlimit) 
    {
      Mdistance = distanceSensor.measureDistanceCm();
      // delay(10);
    }
    Lspeed(0);
    Rspeed(0);
    Robotstate = PSH;
    break;

    case PSH:
    Lspeed(200);
    Rspeed(240);
    sensorValueC = analogRead(A1);
    while (sensorValueC < 800) 
    {
      sensorValueC = analogRead(A1);
      delay(10);
    }
    delay(200); // to push the object really outside the circle
    Lspeed(0);
    Rspeed(0);
    Robotstate = BCK;
    break;

    case BCK:
    Lspeed(-250);
    Rspeed(-200);
    delay(1200); // to return to the center of the cirle
    Lspeed(0);
    Rspeed(0);
    Robotstate = TAM;
    break;
  }
}
