#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// DHT config
#define DHTPIN 19   // Define o pino do sensor DHT22
#define DHTTYPE DHT11   // Define o tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// LEDs config
#define LEDtemp 15
#define LEDhumi 18
#define LEDok   23
#define LEDhigh 22

// lim config
// obs: fazer ser escolha do cliente
#define lim_temp 28
#define lim_humi 60

// acess point config
const char* ssid = "Lau";
const char* password =  "12345678";
  
AsyncWebServer server(80);

// procedimento :3
void check_temp_humi(float unid, int lim){
  if (isnan(unid)){ // FALHA NA LEITURA DE DADOS
    Serial.print("falha ao ler dados do sensor DHT11");
    return;
  }
  else if (unid <= lim){ // TEMP ou HUMI OK
    digitalWrite(LEDok, HIGH);
    digitalWrite(LEDhigh, LOW);
  }
  else{
    digitalWrite(LEDok, LOW);
    digitalWrite(LEDhigh, HIGH);
  }
}

void setup(){
  Serial.begin(115200);
  dht.begin();

  pinMode(LEDtemp    , OUTPUT);
  pinMode(LEDhumi    , OUTPUT);
  pinMode(LEDok      , OUTPUT);
  pinMode(LEDhigh    , OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LEDtemp    , LOW);
  digitalWrite(LEDhumi    , LOW);
  digitalWrite(LEDok      , LOW);
  digitalWrite(LEDhumi    , LOW);
  digitalWrite(LED_BUILTIN, LOW);


  if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
  }
  
  WiFi.begin(ssid, password);
  Serial.println(" ");
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  Serial.println(" ");
  Serial.println(WiFi.localIP());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
    digitalWrite(LEDtemp, LOW);
    digitalWrite(LEDhumi, LOW);
    digitalWrite(LEDok,   LOW);
    digitalWrite(LEDhumi, LOW);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");});
  
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/js");});

  server.on("/navbar.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/navbar.html", "text/html");});
  
  server.on("/footer.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/footer.html", "text/html");});
  
  
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
    digitalWrite(LEDtemp, LOW);
    digitalWrite(LEDhumi, LOW);
    digitalWrite(LEDok,   LOW);
    digitalWrite(LEDhumi, LOW);
  });

  server.on("/help.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/help.html", "text/html");
    digitalWrite(LEDtemp, LOW);
    digitalWrite(LEDhumi, LOW);
    digitalWrite(LEDok,   LOW);
    digitalWrite(LEDhumi, LOW);
  });

  server.begin();
}
  
void loop(){
  delay(2000);

  server.on("/temp.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/temp.html", "text/html"); // -> ponteiro
    digitalWrite(LEDtemp, HIGH);
    digitalWrite(LEDhumi, LOW);
    float temp = dht.readTemperature();
    check_temp_humi(temp, lim_temp);
    Serial.print("temp: ");
    Serial.println(temp);
  });

  server.on("/humi.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/humi.html", "text/html");
    digitalWrite(LEDtemp, LOW);
    digitalWrite(LEDhumi, HIGH);
    float humi = dht.readHumidity();
    check_temp_humi(humi, lim_humi);
    Serial.print("humi: ");
    Serial.println(humi);
  });
}
