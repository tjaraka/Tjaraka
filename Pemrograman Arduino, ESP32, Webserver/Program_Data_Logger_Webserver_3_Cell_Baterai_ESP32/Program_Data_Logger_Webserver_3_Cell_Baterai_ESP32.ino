#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <LCD_I2C.h>

#include "WebPageHTML.h"
  
#define LED 2        //On board LED
#define Sensor1 34    //D5 of NodeMCU is GPIO14
#define Sensor2 35
#define Sensor3 32

//SSID and Password of your WiFi router
const char* ssid = "...";
const char* password = "...";
 
WebServer server(80); //Server on port 80

#include "SendData.h"

LCD_I2C lcd(0x27, 16, 2);

//==============================================================
//                  SETUP
//==============================================================
void setup()
{
  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  //Onboard LED port Direction output
  pinMode(LED,OUTPUT); 
  
  // Wait for connection
  lcd.print("Connecting Wifi");
  lcd.setCursor(0,1);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }

  lcd.clear();
  delay(100);
  lcd.setCursor(1,0);
  lcd.print("Wifi : ");
  lcd.print(ssid);
  lcd.setCursor(1,1);
  lcd.print(WiFi.localIP());
  
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/readADC", handleCELL); //This page is called by java Script AJAX

  server.begin();                  //Start server
  Serial.println("HTTP server started");
}

//==============================================================
//                     LOOP
//==============================================================
void loop()
{
  server.handleClient();          //Handle client requests
  delay(1);
}
