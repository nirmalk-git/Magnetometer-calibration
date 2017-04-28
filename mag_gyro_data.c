#include <Time.h>
#include <EEPROM.h>
#include <math.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
//for mpu9150 ic.
#include <Wire.h>
#include<Servo.h>

Servo aziservo;
int16_t accX, accY, accZ;
int16_t gyroX, gyroY, gyroZ;
int addr;
double accXangle, accYangle; // Angle calculate using the accelerometer
double gyroZangle,new_AZIMUTH,AZIMUTH; // Angle calculate using the gyro
double kalAngleX, kalAngleY; // Calculate the angle using a Kalman filter
uint32_t timer;
uint32_t Starttimer1;


#define MPU9150_GYRO_CONFIG        0x1B   // R/W
#define MPU9150_ACCEL_CONFIG       0x1C   // R/W
#define MPU9150_ACCEL_XOUT_H       0x3B   // R  
#define MPU9150_ACCEL_XOUT_L       0x3C   // R  
#define MPU9150_ACCEL_YOUT_H       0x3D   // R  
#define MPU9150_ACCEL_YOUT_L       0x3E   // R  
#define MPU9150_ACCEL_ZOUT_H       0x3F   // R  
#define MPU9150_ACCEL_ZOUT_L       0x40   // R  
#define MPU9150_GYRO_XOUT_H        0x43   // R  
#define MPU9150_GYRO_XOUT_L        0x44   // R  
#define MPU9150_GYRO_YOUT_H        0x45   // R  
#define MPU9150_GYRO_YOUT_L        0x46   // R  
#define MPU9150_GYRO_ZOUT_H        0x47   // R  
#define MPU9150_GYRO_ZOUT_L        0x48   // R
#define MPU9150_PWR_MGMT_1         0x6B   // R/W

// I2C address 0x69 could be 0x68 depends on your wiring.
int MPU9150_I2C_ADDRESS = 0x68;

void setup()
{
  Serial.begin(9600);
  aziservo.attach(8);
  Wire.begin();
  timer = micros();
 
  // for magneto meter
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }
      sensors_event_t event;
      mag.getEvent(&event);
      float heading = atan2(-event.magnetic.x, event.magnetic.y);
      //float declinationAngle = 0.0270;
      float declinationAngle = 0.0;
      heading += declinationAngle;
 
 // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
// Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
// Convert radians to degrees for readability.
  new_AZIMUTH = heading * 180/M_PI;
 }

 
 void loop()
{
  unsigned long Present1 = millis();
  unsigned long Endtimer1=Starttimer1 + 100;
  unsigned long now5 = millis();
    if (Endtimer1 < Present1)
    {
      sensors_event_t event;
      mag.getEvent(&event);
      float heading = atan2(-event.magnetic.x, event.magnetic.y);
      //float declinationAngle = 0.0270;
      float declinationAngle = 0.0;
      heading += declinationAngle;
 
 // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
// Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
// Convert radians to degrees for readability.
  AZIMUTH = heading * 180/M_PI;

  
  gyroZ = MPU9150_readSensor(MPU9150_GYRO_ZOUT_L,MPU9150_GYRO_ZOUT_H);
  double gyroZrate = -((double)gyroX /131);
  gyroZangle += (gyroZrate+1.32) * ((double)(micros() - timer) / 1000000); // Calculate gyro angle without any filter
  timer = micros();
  new_AZIMUTH = new_AZIMUTH+gyroZangle;
  unsigned long now6 = millis();
  Starttimer1 = Starttimer1 + 100 + now6 - now5;

EEPROM_writeDouble(addr,0);
addr = addr + sizeof(double);
EEPROM_writeDouble(addr,AZIMUTH);
addr = addr + sizeof(double);
EEPROM_writeDouble(addr,new_AZIMUTH);

if (addr == EEPROM.length()) {
    addr = 0;
  }

   String dataString = "";
   dataString += String(AZIMUTH)+" "+String(new_AZIMUTH);
   Serial.println(dataString);
}

aziservo.writeMicroseconds(1510);
delay(4);
}


// All the functions.

int MPU9150_readSensor(int addrL, int addrH){
  Wire.beginTransmission(MPU9150_I2C_ADDRESS);
  Wire.write(addrL);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU9150_I2C_ADDRESS, 1, true);
  byte L = Wire.read();

  Wire.beginTransmission(MPU9150_I2C_ADDRESS);
  Wire.write(addrH);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU9150_I2C_ADDRESS, 1, true);
  byte H = Wire.read();

  return (H<<8)+L;
}

int MPU9150_readSensor(int addr){
  Wire.beginTransmission(MPU9150_I2C_ADDRESS);
  Wire.write(addr);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU9150_I2C_ADDRESS, 1, true);
  return Wire.read();
}

int MPU9150_writeSensor(int addr,int data){
  Wire.beginTransmission(MPU9150_I2C_ADDRESS);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission(true);

  return 1;
}

void EEPROM_writeDouble(int ee, double value)
{
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       EEPROM.write(ee++, *p++);
}

