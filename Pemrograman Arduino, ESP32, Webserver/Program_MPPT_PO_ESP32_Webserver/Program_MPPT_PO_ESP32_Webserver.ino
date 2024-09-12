#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <LCD_I2C.h>

TaskHandle_t Core2;

Adafruit_ADS1115 ads;

//==============================================================
// WiFi Setup //
//==============================================================
const char* ssid = "SesharHomeAtas";
const char* password = "marCOCOtjaraka2002";

AsyncWebServer server(80); //Server on port 80

//==============================================================
//LCD//
//==============================================================
#define Button_Right 35
#define Button_Left 32

//==============================================================
//MPPT//
//==============================================================
#define PWM 33
#define Shutdown 4

//==============================================================
//DEKLARASI BAGIAN LCD//
//==============================================================
int 
LCDColumns = 20,
LCDRows = 4,
LCDPage = 0,
LCDPages = 4,
millisLCDInterval = 1000;

LCD_I2C LCD(0x27, LCDColumns, LCDRows);

bool 
Button_Right_Command = 0,
Button_Left_Command = 0;

unsigned long 
currentLCDMillis = 0,
prevLCDMillis = 0;

//==============================================================
//DEKLARASI MPPT DAN ADS1115 VOLTAGE SENSOR//
//==============================================================
const int 
Frequency = 39000,
ledChannel = 0,
PWMresolution = 8;

float 
resolution = 0;

int
average_count_voltage_sensor = 3,
DutyCycle = 0;

float
raw_input_voltage_sensor = 0.0000,
raw_output_voltage_sensor = 0.0000,
raw_input_current_sensor = 0.0000,
raw_output_current_sensor = 0.0000,
voltage_input = 0.0000,
voltage_output = 0.0000,
current_input = 0.0000,
current_output = 0.0000,
power_input = 0.0000,
power_output = 0.0000,
voltage_input_previous = 0.0000, //
power_input_previous = 0.0000, //
min_voltage_input = 13.0, //V
max_voltage_output = 12.6, //V
current_charging = 5.0000, //A
divider_current_input = 0.046377, //9780.0/(201100.0+9780.0)
divider_current_output = 0.046797, //9760.0/(198800.0+9760.0)
divider_input = 0.0046179, //14730.0/(3175000.0+14730.0)
divider_output = 0.016713; //50910.0/(3150000.0+50910.0)

int Batt = 0;

String WiFiConnecting1 = "CONNECTED";
String WiFiConnecting2 = "NOT CONNECTED";
String WiFiConnecting;

String Koneksi1 = "TERHUBUNG";
String Koneksi2 = "TIDAK TERHUBUNG";
String Koneksi;

String Mengisi1 = "MENGISI";
String Mengisi2 = "TIDAK MENGISI";
String Mengisi;

//==============================================================
//DATA STRING TO WEBSERVER//
//==============================================================
String V_Panel(){
  float Vpv = voltage_input;
  if (isnan(Vpv)) {
    //Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    //Serial.println(h);
    return String(Vpv);
  }
}

String I_Panel(){
  float Ipv = current_input;
  if (isnan(Ipv)) {
    //Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    //Serial.println(h);
    return String(Ipv);
  }
}

String P_Panel(){
  float Ppv = voltage_input*current_input;
  if (isnan(Ppv)) {
    //Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    //Serial.println(h);
    return String(Ppv);
  }
}

String V_Baterai(){
  float Vb = voltage_output;
  if (isnan(Vb)) {
    //Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    //Serial.println(h);
    return String(Vb);
  }
}

String I_Baterai(){
  float Ib = current_output;
  if (isnan(Ib)) {
    //Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    //Serial.println(h);
    return String(Ib);
  }
}

String P_Baterai(){
  float Pb = voltage_output*current_output;
  if (isnan(Pb)) {
    //Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    //Serial.println(h);
    return String(Pb);
  }
}

String Pre_Baterai(){
  float Pre = map(Batt, 600, 1260, 0, 100);
  if (isnan(Pre)) {
    //Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    //Serial.println(h);
    return String(Pre);
  }
}

String Duty_Cycle(){
  float Dut = map(DutyCycle, 0, 255, 0, 100);
  if (isnan(Dut)) {
    //Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    //Serial.println(h);
    return String(Dut);
  }
}

String Efisiensi(){
  float Eff = ((voltage_output*current_output)/(voltage_input*current_input))*100.00;
  if (isnan(Eff)) {
    //Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    //Serial.println(h);
    return String(Eff);
  }
}

//==============================================================
//----------VOID LCD SETTTING----------//
//==============================================================
// MENAMPILKAN IP ADDRESS WIFI //
void displayConfig0(){
  if (WiFi.status() != WL_CONNECTED){
    WiFiConnecting = WiFiConnecting2;
  }
  else{
    LCD.clear();
    WiFiConnecting = WiFiConnecting1;
  }

  LCD.clear();
  LCD.setCursor(0,0);
  LCD.print("WiFi");
  LCD.setCursor(5,0);
  LCD.print(WiFiConnecting);
  LCD.setCursor(0,1);
  LCD.print(ssid);
  LCD.setCursor(0,2);
  LCD.print("IP Address");
  LCD.setCursor(0,3);
  LCD.print(WiFi.localIP());
}

// MENAMPILAKAN NILAI TEGANGAN/ARUS INPUT OUTPUT //
void displayConfig1(){
  LCD.setCursor(0,0);
  LCD.print("Vin :");
  LCD.print(voltage_input);
  LCD.print("V");
  LCD.setCursor(0,1);
  LCD.print("Vo  :");
  LCD.print(voltage_output);
  LCD.print("V");
  LCD.setCursor(0,2);
  LCD.print("Iin :");
  LCD.print(current_input);
  LCD.print("A");
  LCD.setCursor(0,3);
  LCD.print("Io  :");
  LCD.print(current_output);
  LCD.print("A");
  LCD.setCursor(20,0);
  LCD.print("M");
  LCD.setCursor(20,1);
  LCD.print("P");
  LCD.setCursor(20,2);
  LCD.print("P");
  LCD.setCursor(20,3);
  LCD.print("T");  
}

// MENAMPILKAN TEGANGAN, ARUS, DAYA PV //
void displayConfig2(){
  LCD.setCursor(0,0);
  LCD.print("Pin      :");
  LCD.print(voltage_input*current_input);
  LCD.print("W");
  LCD.setCursor(0,1);
  LCD.print("Pout     :");
  LCD.print(voltage_output*current_output);
  LCD.print("W");
  LCD.setCursor(0,2);
  LCD.print("Duty Cyle:");
  LCD.print(map(DutyCycle, 0, 255, 0, 100));
  LCD.print("%");
  LCD.setCursor(0,3);
  LCD.print("Eff      :");
  LCD.print(((voltage_output*current_output)/(voltage_input*current_input))*100.00);
  LCD.print("%");
  LCD.setCursor(20,0);
  LCD.print("M");
  LCD.setCursor(20,1);
  LCD.print("P");
  LCD.setCursor(20,2);
  LCD.print("P");
  LCD.setCursor(20,3);
  LCD.print("T");
}

void displayConfig3(){
  LCD.setCursor(0,0);
  LCD.print("Panel Surya");
  LCD.setCursor(0,1);
  LCD.print("Vpv    :");
  LCD.print(voltage_input);
  LCD.print("V");
  LCD.setCursor(0,2);
  LCD.print("Koneksi:");
  LCD.setCursor(0,3);
  LCD.print(Koneksi);
}

void displayConfig4(){
  LCD.setCursor(0,0);
  LCD.print("Baterai ->");
  LCD.setCursor(0,1);
  LCD.print("Vbatt    :");
  LCD.print(voltage_output);
  LCD.print("V");
  LCD.setCursor(10,0);
  LCD.print(map(Batt, 600, 1260, 0, 100));
  LCD.print("%");
  LCD.setCursor(0,2);
  LCD.print("Pengisian:");
  LCD.setCursor(0,3);
  LCD.print(Mengisi);
}

//==============================================================
//----------VOID VOLTAGE SENSOR INPUT OUTPUT DAN BATERAI----------//
//==============================================================
void VOLTAGE_SENSOR(){
  raw_input_voltage_sensor = 0.0000;
  raw_output_voltage_sensor = 0.0000;
  raw_input_current_sensor = 0.0000;
  raw_output_current_sensor = 0.0000;
  
  for(int i = 0; i<average_count_voltage_sensor; i++){
    raw_input_voltage_sensor = raw_input_voltage_sensor + ads.readADC_SingleEnded(0);
    raw_output_voltage_sensor = raw_output_voltage_sensor + ads.readADC_SingleEnded(1);
    raw_input_current_sensor = raw_input_current_sensor + ads.readADC_SingleEnded(2);
    raw_output_current_sensor = raw_output_current_sensor + ads.readADC_SingleEnded(3);
  }
  resolution = ads.voltsPerBit()*1000.0F;
  voltage_input = ((((raw_input_voltage_sensor/average_count_voltage_sensor)*resolution)+0.00)/divider_input)/1000.0;
  voltage_output = ((((raw_output_voltage_sensor/average_count_voltage_sensor)*resolution)+0.00)/divider_output)/1000.0;
  current_input = ((((((raw_input_current_sensor/average_count_voltage_sensor)*resolution)+0.00)/divider_current_input)/1000.0)-2.49)/0.066;
  current_output = ((((((raw_output_current_sensor/average_count_voltage_sensor)*resolution)+0.00)/divider_current_output)/1000.0)-2.5)/0.066;

  power_input = voltage_input*current_input;
  power_output = voltage_output*current_output;

  if(current_input<0){current_input = 0;}
  if(current_output<0){current_output = 0;}

  if(voltage_input>1){Koneksi = Koneksi1;}
  else{Koneksi = Koneksi2;}

  if(current_output>0.01){Mengisi = Mengisi1;}
  else{Mengisi = Mengisi2;}

  Batt = voltage_output*100;
  if(Batt<600){Batt = 600;}
  if(Batt>1260){Batt = 1260;}
}

//==============================================================
//----------VOID MPPT SYNCHRONOUS BUCK CONVERTER----------//
//==============================================================
void MPPT_BUCK(){
  if(voltage_input<min_voltage_input) {digitalWrite(Shutdown,LOW);}
  else{digitalWrite(Shutdown,HIGH);}
  if(current_output>current_charging){DutyCycle--;}                                                  //  Current Is Above → Decrease Duty Cycle
  else if(voltage_output>max_voltage_output){DutyCycle--;}                                           //  Voltage Is Above → Decrease Duty Cycle   
  
  else{                                                                                              //  MPPT ALGORITHM
    if(power_input>power_input_previous && voltage_input>voltage_input_previous)     {DutyCycle--;}  //  ↑P ↑V ; →MPP  //D--
    else if(power_input>power_input_previous && voltage_input<voltage_input_previous){DutyCycle++;}  //  ↑P ↓V ; MPP←  //D++
    else if(power_input<power_input_previous && voltage_input>voltage_input_previous){DutyCycle++;}  //  ↓P ↑V ; MPP→  //D++
    else if(power_input<power_input_previous && voltage_input<voltage_input_previous){DutyCycle--;}  //  ↓P ↓V ; ←MPP  //D--
    else if(voltage_output<max_voltage_output)                                       {DutyCycle++;}  //  MP MV ; MPP Reached - 
    power_input_previous   = power_input;                                                            //  Menyimpan Nilai Daya Terbaru
    voltage_input_previous = voltage_input;                                                          //  Menyimpan Nilai Tegangan terbaru        
    }
  ledcWrite(ledChannel, DutyCycle);
}

//==============================================================
//----------VOID SERIAL PRINT----------//
//==============================================================
void SERIAL_PRINT(){
  Serial.print("BUCK = ");
  Serial.print(Shutdown);
  Serial.print("   ");
  Serial.print("Duty = ");
  Serial.print(DutyCycle);
  Serial.print("   ");
  Serial.print("Tegangan Input = ");
  Serial.print(voltage_input);
  Serial.print("   ");
  Serial.print("Arus Input = ");
  Serial.print(current_input);
  Serial.print("   ");
  Serial.print("Tegangan Output = ");
  Serial.print(voltage_output);
  Serial.print("   ");
  Serial.print("Arus Output = ");
  Serial.println(current_output);
}

//==============================================================
//----------VOID LCD COMMAND----------//
//==============================================================
void LCD_COMMAND(){
  if(digitalRead(Button_Right)==1) {Button_Right_Command=1;}
  if(digitalRead(Button_Left)==1) {Button_Left_Command=1;}

  currentLCDMillis = millis();
  if(currentLCDMillis-prevLCDMillis>=millisLCDInterval){
    prevLCDMillis = currentLCDMillis;

    if(Button_Right_Command==1) {Button_Right_Command=0; LCDPage++; LCD.clear();}
    else if(Button_Left_Command==1) {Button_Left_Command=0; LCDPage--; LCD.clear();}

    if(LCDPage>LCDPages) {LCDPage=0;}
    else if(LCDPage<0) {LCDPage=LCDPages;}

    if(LCDPage==0) {displayConfig0();}
    else if(LCDPage==1) {displayConfig1();}
    else if(LCDPage==2) {displayConfig2();}
    else if(LCDPage==3) {displayConfig4();}
  }
}

//================= CORE0: SETUP (DUAL CORE MODE) =====================//
void coreTwo(void * pvParameters){
  SPIFFS.begin();
  WiFi.begin(ssid, password);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/Webserver.html");
  });
  server.on("/TeganganPanel", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",V_Panel().c_str()); 
  });
  server.on("/ArusPanel", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",I_Panel().c_str()); 
  });
  server.on("/DayaPanel", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",P_Panel().c_str()); 
  });
  server.on("/TeganganBaterai", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",V_Baterai().c_str()); 
  });
  server.on("/ArusBaterai", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",I_Baterai().c_str()); 
  });
  server.on("/DayaBaterai", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",P_Baterai().c_str()); 
  });
  server.on("/PresentaseBaterai", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",Pre_Baterai().c_str()); 
  });
  server.on("/DutyMPPT", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",Duty_Cycle().c_str()); 
  });
  server.on("/EfisiensiMPPT", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",Efisiensi().c_str()); 
  });
  
  server.begin();
                                           
//================= CORE0: LOOP (DUAL CORE MODE) ======================//
  while(1){}                             
}

//==============================================================
//----------VOID SETUP----------//
//==============================================================
void setup() {
  Serial.begin(115200);
  LCD.begin();
  LCD.backlight();
  pinMode(Button_Right,INPUT);
  pinMode(Button_Left,INPUT);
  pinMode(PWM,OUTPUT);
  pinMode(Shutdown,OUTPUT);
  ledcSetup(ledChannel, Frequency, PWMresolution);
  ledcAttachPin(PWM, ledChannel);
  ads.setGain(GAIN_SIXTEEN);
  ads.begin();
  
  xTaskCreatePinnedToCore(coreTwo,"coreTwo",10000,NULL,0,&Core2,0);
  
}

//==============================================================
//----------VOID LOOP----------//
//==============================================================
void loop() {
  VOLTAGE_SENSOR();
  MPPT_BUCK();
  SERIAL_PRINT();
  LCD_COMMAND();
}
