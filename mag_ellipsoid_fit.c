#include <EEPROM.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
//for mpu9150 ic.
#include <Wire.h>
#include<Servo.h>
#include <Dynamixel_Serial.h> 

Servo aziservo;
double magx,magy,magz;
int addr = 0;
void setup(){
   aziservo.attach(8);
   Wire.begin();
   Serial.begin(9600);
     if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }
 
}

void loop(){
  
sensors_event_t event;
mag.getEvent(&event);
magx = event.magnetic.x;
magy = event.magnetic.y;
magz = event.magnetic.z;

String dataString = "";
dataString = String(magx)+ "," +String(magy)+ "," + String(magz);

EEPROM_writeDouble(addr,0);
addr = addr + sizeof(double);
EEPROM_writeDouble(addr,magx);
addr = addr + sizeof(double);
EEPROM_writeDouble(addr,magy);
addr = addr + sizeof(double);
EEPROM_writeDouble(addr,magz);
addr = addr + sizeof(double);


if (addr == EEPROM.length()) {
    addr = 0;
  }
  
Serial.println(dataString);
// Take 1 measurement every 500 milliseconds
delay(200);
aziservo.writeMicroseconds(1510);
delay(4);

}

void EEPROM_writeDouble(int ee, double value)
{
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       EEPROM.write(ee++, *p++);
}
