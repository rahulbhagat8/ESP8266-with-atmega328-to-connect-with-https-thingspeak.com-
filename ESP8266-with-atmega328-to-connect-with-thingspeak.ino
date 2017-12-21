#include <SoftwareSerial.h>
#include <stdlib.h>
#include <Wire.h>
#include <FaBo3Axis_ADXL345.h>
#include "TimerOne.h"
#define IP "184.106.153.149" // ThingSpeak IP Address: 184.106.153.149
FaBo3Axis fabo3axis;
#define illumination_light 4
#define UV_light 5
#define buzzer 8
int x, y, z;
int IsWifiConnected = 0;
String field1 = "1=";
char buffer[16];
float bat_volt = 10;

String network = "";  //enter wifi name
String password = ""; // enter wifi password
// replace with your channel's thingspeak API key
String apiKey = "XEQZV9WCJWM1RFD0";

SoftwareSerial ESP8266(3, 4); // RX, TX
String GET = "GET /update?key=XEQZV9WCJWM1RFD0&field1=";

void setup()
{
  pinMode(2, OUTPUT);

  Serial.begin(115200);
  ESP8266.begin(115200);
  ESP8266.println("AT");
  Serial.println("AT");
  delay(500);
  if (ESP8266.find("OK")) {
    Serial.println("ESP Connected");
  }
  else {
    ("ESP Not connected");
  }
//  ESP8266.println("AT+CWJAP");
//  Serial.println("AT+CWJAP");
  delay(100);
  digitalWrite(2, LOW);
  delay(1000);
  digitalWrite(2, HIGH);
  delay(100);
  runEsp8266();
}

void loop()
{
  //  int tempC = 120;
  //  char buffer[10];
  //  String tempF = dtostrf(tempC, 4, 1, buffer);
  ////  updateTemp(tempF);
  //  delay(16000);
  esp_8266();
}

//----- update the  Thingspeak string
void esp_8266()
{
  // convert to string
  char buf[32];
  String strVolt = dtostrf( bat_volt, 4, 1, buf);
  Serial.print(strVolt);
  Serial.println(" V");
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ESP8266.println(cmd);

  if (ESP8266.find("Error")) {
    Serial.println("AT+CIPSTART error");
    return;
  }

  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr += "&field1=";
  getStr += String(strVolt);
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ESP8266.println(cmd);

  if (ESP8266.find(">")) {
    ESP8266.print(getStr);
  }
  else {
    ESP8266.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
  }

  // thingspeak needs 15 sec delay between updates
//  delay(16000);
}

//---------------------------------------------------------------
void runEsp8266() {
  Serial.println("TRYING changeWifiMode");
  changeWifiMode();
  Serial.println("checkWifiStatus");
  checkWifiStatus();
  connectToWifi();
  ESP8266.println("AT+CWJAP");
}

// END MAIN ESP8266 FUNCTION

// 1 - CHANGE MODE
void changeWifiMode()
{
  ESP8266.println(F("AT+CWMODE?"));
  delay(1000);
  if (ESP8266.find("3"))
  {
    Serial.println("WiFi mode is 3");
  }
  else
  {
    Serial.println("Changing WiFi mode");
    ESP8266.println(F("AT+CWMODE=3"));
    delay(1000);
    if (ESP8266.find("no change") || ESP8266.find("OK"))
    {
      Serial.println("WiFi mode is 3 now");
    }
    else
    {
      Serial.println("Error while changing WiFi mode to 3");
    }
  }

}
// END CHANGE MODE

// 2 - CHECK WIFI NETWORK STATUS
void checkWifiStatus() {
  ESP8266.println("AT+CWJAP?");
  delay(1000);
  if (ESP8266.find(":")) {
    Serial.println("WIFI NETWORK CONNECTED");
    IsWifiConnected = 1;
  }
  else
  {
    Serial.println("WIFI NETWORK NOT CONNECTED");
    IsWifiConnected = 0;
  }
}
// END CHECK WIFI NETWORK STATUS

// 3 - CONNECT TO WIFI
void connectToWifi()
{
  if (IsWifiConnected == 0)
  {
    Serial.println("Connecting to WiFi");
    String cmd = F("AT+CWJAP=\"");
    cmd += network;
    cmd += F("\",\"");
    cmd += password;
    cmd += F("\"");
    ESP8266.println(cmd);
    delay(2000);
    if (ESP8266.find("OK"))
    {
      Serial.println("CONNECTED TO WIFI");
    }
    else
    {
      Serial.println("ERROR While connecting to WiFi");
      delay(2000);
      connectToWifi();
    }
  }
}

