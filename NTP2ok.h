#include <WiFi.h>
#include "time.h"
 
const char* ssid       = "IOT_TREE_BLUE";
const char* password   = "98550526";
 
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
 
//nao esqueca de ajustar o fuso
const int   daylightOffset_sec = -3600*3;
 
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Falha ao obter a hora");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
 
void setup()
{
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Conectando em %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" Feito");
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
 
void loop()
{
  delay(5000);
  printLocalTime();
}