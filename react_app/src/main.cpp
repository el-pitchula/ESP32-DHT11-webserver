#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
  
const char* ssid = "Lau";
const char* password =  "12345678";
  
AsyncWebServer server(80);
  
void setup(){
  Serial.begin(115200);
  
  if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
  }
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println(WiFi.localIP());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
 
  server.on("/demo.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/demo.js", "text/javascript");
  });
  
  server.begin();
}
  
void loop(){}