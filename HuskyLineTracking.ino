/*
HuskyLineTracking
Adrie Huesman
20 Januari 2021

Hardware
Arduino Nano Atmega328
Arduino Nano sensor I/O shield v3.0
4 Alkaline batteries so 6 volts
2 SG90 continous or 360 degrees servos
Wheel for Standard Servo 25T-40×7mm 2-Pack Pololu 4905
Ball Caster with 3/4″ Plastic Ball Pololu 954
QTR-3A Reflectance Sensor Array Pololu 2456
Powerboard, own design based on 5V Step-Up/Step-Down Voltage Regulator S7V7F5 Pololu 2119
GRAVITY: HUSKYLENS - 2MP also see https://wiki.dfrobot.com/HUSKYLENS_V1.0_SKU_SEN0305_SEN0336
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
A4 I2C SDA Huskylens
A5 I2C SCL Huskylens
A6 --
A7 --

Other
SensorValues when all black 900 - 924, say 900
SensorValues when all white 488 - 579, say 600
 */

#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
#include <Servo.h>

HUSKYLENS huskylens;
//HUSKYLENS green line >> SDA; blue line >> SCL
void printResult(HUSKYLENSResult result);

Servo Lservo;
Servo Rservo;

// Declare variables
int32_t error;
int ID1; 
float Kc = 0.80;      // Controller gain (0.80)
float Paction;        
int Avspeed = 200;    // Average speed (200) 

void printResult(HUSKYLENSResult result){
    if (result.command == COMMAND_RETURN_BLOCK){
        Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
    }
    else if (result.command == COMMAND_RETURN_ARROW){
        Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
    }
    else{
        Serial.println("Object unknown!");
    }
}

void Lspeed(int speed)   // Set speed of left motor, -500 < speed < 500
{
  Lservo.writeMicroseconds(speed + 1500);    
}

void Rspeed(int speed)   // Set speed of right motor, -500 < speed < 500
{
  Rservo.writeMicroseconds(1500 - speed);    
}
 
void setup() {
    Lservo.attach(5);
    Rservo.attach(6);
    Serial.begin(115200);
    Wire.begin();
    while (!huskylens.begin(Wire))
    {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protol Type>>I2C)"));
        Serial.println(F("2.Please recheck the connection."));
        delay(100);
    }
    huskylens.writeAlgorithm(ALGORITHM_LINE_TRACKING); //Switch the algorithm to line tracking.
}
 
void loop() {
    if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
    else
    {
        HUSKYLENSResult result = huskylens.read();
        printResult(result);
 
        // Calculate the error
        error = (int32_t)160 - (int32_t)result.xTarget;
 
        // Perform P-only control
        Paction = Kc*error;
        Lspeed(Avspeed - int(Paction));
        Rspeed(Avspeed + int(Paction) + 40);  // Motors behave different
        delay(10);
    }
}
 
