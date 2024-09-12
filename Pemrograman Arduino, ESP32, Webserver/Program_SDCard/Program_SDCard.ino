#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "DS3231.h"

RTClib RTC;
DS3231 Clock;

float Cell1;
float Cell2;
float Cell3;

float voltage;

char *loggerfile = "DataLog.txt";

int Year;
int Month;
int Day;
int Hour;
int Minute;
int Second;

boolean SDCard = 0;



void setup() {
  Serial.begin(9600);
  Wire.begin(); //for DS3231
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  delay(2000);  //afer reset Arduino, 2s to take out SD

  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
  }
  else
  {
    SDCard = true;
    Serial.println("initialization done.");
    File myFile;
    myFile = SD.open(loggerfile, FILE_WRITE);
    if(myFile)
    {
      Serial.println("initialization done.");
      myFile.println("   DATE    |   TIME   |  CELL 1  |  CELL 2  |  CELL 3  |");
      myFile.close();
    }
    else{
      Serial.println("initialization failed!");
    }
  }
}

void loop() {
  int SensorCELL1 = analogRead(A0);//read A0 pin
  //convert value of 0 to 1023 to voltage. 
  //Arduino UNO is 5V so we use 5
  int SensorCELL2 = analogRead(A1);
  int SensorCELL3 = analogRead(A2);
  
  float VoltCELL1 = SensorCELL1 * (5.0 / 1023.0);
  float VoltCELL2 = SensorCELL2 * (5.0 / 1023.0);
  float VoltCELL3 = SensorCELL3 * (5.0 / 1023.0);

  float HitungCELL3 = VoltCELL3 / (3300.0/(900.0+3300.0));
  Cell3 = HitungCELL3;
  float HitungCELL2 = VoltCELL2 / (1300.0/(2010.0+1300.0));
  Cell2 = HitungCELL2 - Cell3;
  float HitungCELL1 = VoltCELL1 / (710.0/(2000.0+710.0));
  Cell1 = HitungCELL1 - (Cell2 + Cell3);

  DateTime now = RTC.now();
  Year = now.year();
  Month = now.month();
  Day = now.day();
  Hour = now.hour();
  Minute = now.minute();
  Second = now.second();

//  write_data(voltage);
char buffer1[22];
  // if the file opened okay, write to it:
  if (SDCard) {
    File myFile;
    myFile = SD.open(loggerfile, FILE_WRITE);
    sprintf(buffer1, "%02u-%02u-%04u | %02u:%02u:%02u", Day, Month, Year, Hour, Minute, Second);
    myFile.print(buffer1);
    myFile.print(" |  ");
    myFile.print(Cell1);
    myFile.print("V   |  ");
    myFile.print(Cell2);
    myFile.print("V   |  ");
    myFile.print(Cell3);
    myFile.println("V   |");
    myFile.close();
    Serial.println("Write file successful!"); //print out COM Port
  } else {
    Serial.print("error opening");
    Serial.println(loggerfile);
  }
  delay(1000);
}

//void write_data(voltage)
//{
//  // if the file opened okay, write to it:
//  if (SDCard) {
//    File myFile;
//    myFile = SD.open(loggerfile, FILE_WRITE);
//    sprintf(buffer1, "%02u-%02u-%04u | %02u:%02u:%02u", Day, Month, Year, Hour, Minute, Second);
//    muFile.print(buffer1);
//    myFile.print(" |  ");
//    myFile.print(Cell1);
//    myFile.print("V   |  ");
//    myFile.print(Cell2);
//    myFile.print("V   |  ");
//    myFile.println(Cell3);
//    myFile.close();
//    Serial.println("Write file successful!"); //print out COM Port
//  } else {
//    Serial.print("error opening");
//    Serial.println(loggerfile);
//  }
//  delay(1000);
//}
