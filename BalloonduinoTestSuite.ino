#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "Adafruit_MCP9808.h"
#include "RTClib.h"
#include <Adafruit_BME280.h>
#include <SD.h>
#include <Adafruit_ADS1015.h>

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x29);
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
RTC_DS1307 rtc;
Adafruit_BME280 bme;
Adafruit_ADS1015 ads;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Balloonduino Test Suite");

  //BNO
  if(!bno.begin()){
    Serial.println("BNO055 NOT detected.");
  }
  else{
    Serial.println("BNO055 detected!");
  }
  delay(1000);
  bno.setExtCrystalUse(true);

  //MCP9808
  if (!tempsensor.begin(0x18)) {
    Serial.println("MCP9808 NOT detected.");
  }
  else{
    Serial.println("MCP9808 detected!");
  }

  if (! rtc.begin()) {
    Serial.println("DS1308 NOT detected.");
  }
  else{
    Serial.println("DS1308 detected!");
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  if (!bme.begin(0x76)) {
    Serial.println("BME280 NOT detected.");
  }
  else{
    Serial.println("BME280 detected!");
  }

  ads.begin();
  Serial.println("Initialized ADS1015");

  SPI.begin();
  pinMode(53,OUTPUT);
  if (!SD.begin(53)) {
    Serial.println("SD Card NOT detected.");
  }
  else{
    Serial.println("SD Card detected!");
  }
}

void loop(void)
{
  //BNO Test
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  
  Serial.print("BNO055 Test: ");
  Serial.print("X: ");
  Serial.print(euler.x());
  Serial.print(" Y: ");
  Serial.print(euler.y());
  Serial.print(" Z: ");
  Serial.print(euler.z());
  Serial.print("\t");

  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  Serial.print("Calibation: Sys=");
  Serial.print(system, DEC);
  Serial.print(", Gyro=");
  Serial.print(gyro, DEC);
  Serial.print(", Accel=");
  Serial.print(accel, DEC);
  Serial.print(", Mag=");
  Serial.print(mag, DEC);
  Serial.print(", Temp=");
  Serial.println(bno.getTemp());

  //MCP9808
  float c = tempsensor.readTempC();
  Serial.print("MCP9808 Temp: "); Serial.print(c); Serial.println("*C"); 

  //DS1308
  DateTime now = rtc.now();
  Serial.print("DS1307: "); Serial.print(daysOfTheWeek[now.dayOfTheWeek()]); Serial.print(' ');
  Serial.print(now.year(), DEC); Serial.print('/');
  Serial.print(now.month(), DEC); Serial.print('/');
  Serial.print(now.day(), DEC); Serial.print(' ');
  Serial.print(now.hour(), DEC); Serial.print(':');
  Serial.print(now.minute(), DEC); Serial.print(':');
  Serial.print(now.second(), DEC); Serial.print(" UnixTime:");
  Serial.println(now.unixtime());

  //BME280
  Serial.print("BME280: ");
  Serial.print("Pressure: ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.print(" hPa, Temp: ");
  Serial.print(bme.readTemperature());
  Serial.print("C, Humidity: ");
  Serial.print(bme.readHumidity());
  Serial.println("%");

  //ADS1015
  Serial.print("ADS1015: ");
  Serial.print("A0: ");
  Serial.print(ads.readADC_SingleEnded(0));
  Serial.print(", A1: ");
  Serial.print(ads.readADC_SingleEnded(1));
  Serial.print(", A2: ");
  Serial.print(ads.readADC_SingleEnded(2));
  Serial.print(", A3: ");
  Serial.println(ads.readADC_SingleEnded(3));

  //MicroSD
  File dataFile = SD.open("test.txt", FILE_WRITE);

  if (dataFile) {
    Serial.println("SD Card: Writing test string.");
    dataFile.println("This is a test.");
    dataFile.close();
  }
  else{
    Serial.println("SD Card: Card not detected.");
  }

  Serial.println(" ");
  delay(1000);

}
