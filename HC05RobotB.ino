/*
Adrie Huesman
HC05RobotB
To be used with Arduino Bluetooth Controlled Joystick, see
https://play.google.com/store/apps/details?id=uncia.robotics.joystick&hl=en_US&gl=US
Joystick (Advanced) mode
*/

// Declare variables
int Mspeed = 200;    // Motor speed
int hoek;
int sterkte;
int x;
int y;

// #include <SoftwareSerial.h>
// SoftwareSerial btSerial(9, 10); // RX, TX PIN

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

void setup() {
  pinMode(13,OUTPUT);
  Serial.begin(9600);
  // btSerial.begin(9600);
  Lservo.attach(5);
  Rservo.attach(6);
  // btSerial.println("Einde setup");
}
   
void loop()
{
  if(Serial.available() > 0) {
    String value = Serial.readStringUntil('#');
    if(value.length()==7)
    {
      String angle = value.substring(0, 3);
      String strength = value.substring(3, 6);
      String button = value.substring(6, 8);
      // Serial.print("angle: ");Serial.print(angle);Serial.print('\t');
      // Serial.print("strength: ");Serial.print(strength);Serial.print('\t');
      // Serial.print("button: ");Serial.print(button);Serial.println("");
      hoek = angle.toInt();
      sterkte = strength.toInt();
      // Serial.println(hoek);
      if (sterkte <= 10)
      {
        Lspeed(0);
        Rspeed(0);
      }
      if (sterkte > 10)
      {
        if ((hoek >= 0) && (hoek < 90))
        {
          Lspeed(Mspeed);
          x = hoek;
          y = map(x, 0, 90, -Mspeed, Mspeed);
          Rspeed(y);
        }
        if ((hoek >= 90) && (hoek < 180))
        {
          x = hoek;
          y = map(x, 90, 180, Mspeed, -Mspeed);
          Lspeed(y);
          Rspeed(Mspeed);
        }
        if ((hoek >= 180) && (hoek < 270))
        {
          Lspeed(-Mspeed);
          x = hoek;
          y = map(x, 180, 270, Mspeed, -Mspeed);
          Rspeed(y);
        }
        if ((hoek >= 270) && (hoek <= 360))
        {
          x = hoek;
          y = map(x, 270, 360, -Mspeed, Mspeed);
          Lspeed(y);
          Rspeed(-Mspeed);
        }
        Serial.flush();
        value="";
        delay(50);
      }
    }
  }
}  
