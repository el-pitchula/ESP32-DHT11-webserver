#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <painlessMesh.h>
#include <Arduino_JSON.h>

// DHT config
#define DHTPIN 19   // Define o pino do sensor DHT22
#define DHTTYPE DHT11   // Define o tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// LEDs config
#define LEDtemp 15
#define LEDhumi 18
#define LEDok   23
#define LEDhigh 22

// lim config (by slider control)
#define lim_temp 28
#define lim_humi 60

const char* ssid = "Lau";
const char* password = "12345678";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

/* -------------------------- procedimentos :3 --------------------------*/
// Initialize SPIFFS
void initFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else{
   Serial.println("SPIFFS mounted successfully");
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.println(WiFi.localIP());
}

void check_temp_humi(float unid, int lim){
  if (isnan(unid)){ // FALHA NA LEITURA DE DADOS
    Serial.print("falha ao ler dados do sensor DHT11");
    return;
  }
  else if (unid <= lim){ // TEMP ou HUMI OK
    Serial.println("testei a temperatura ou a humidade e deu BOM");
    digitalWrite(LEDok, HIGH);
    digitalWrite(LEDhigh, LOW);
  }
  else{
    Serial.println("testei a temperatura ou a humidade e deu RUIM");
    digitalWrite(LEDok, LOW);
    digitalWrite(LEDhigh, HIGH);
  }
}

void getTempPage(AsyncWebServerRequest *request) {
  float temp = dht.readTemperature();
  digitalWrite(LEDtemp, HIGH);
  digitalWrite(LEDhumi, LOW);
  Serial.print("temp: ");
  Serial.print(temp);
  Serial.println("°C ");
  request->send(SPIFFS, "/temp.html", "text/html");
}

void getHumiPage(AsyncWebServerRequest *request) {
  float humi = dht.readHumidity();
  digitalWrite(LEDtemp, LOW);
  digitalWrite(LEDhumi, HIGH);
  Serial.print("humi: ");
  Serial.print(humi);
  Serial.println("% ");
  request->send(SPIFFS, "/humi.html", "text/html");
}

void handleTemp(AsyncWebServerRequest *request) {
  float temp = dht.readTemperature();   // Lê a temperatura do sensor
  check_temp_humi(temp, lim_temp);
  String TempValue = String(temp);
  request->send(200, "text/plane", TempValue); //Send temp value only to client ajax request
}

void handleHumi(AsyncWebServerRequest *request) {
  float humi = dht.readHumidity();   // Lê a temperatura do sensor
  check_temp_humi(humi, lim_humi);
  String HumiValue = String(humi);
  request->send(200, "text/plane", HumiValue); //Send temp value only to client ajax request
}

void leds_off(){
  digitalWrite(LEDtemp    , LOW);
  digitalWrite(LEDhumi    , LOW);
  digitalWrite(LEDok      , LOW);
  digitalWrite(LEDhigh    , LOW);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LEDtemp    , OUTPUT);
  pinMode(LEDhumi    , OUTPUT);
  pinMode(LEDok      , OUTPUT);
  pinMode(LEDhigh    , OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  leds_off();

  initFS();
  initWiFi();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    leds_off();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    leds_off();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/help.html", HTTP_GET, [](AsyncWebServerRequest *request){
    leds_off();
    request->send(SPIFFS, "/help.html", "text/html");
  });
  
  server.on("/temp.html", HTTP_GET, getTempPage); //To get update of temp value only
  server.on("/temp/getTemp", HTTP_GET, handleTemp);

  server.on("/humi.html", HTTP_GET, getHumiPage); //To get update of humi value only
  server.on("/humi/getHumi", HTTP_GET, handleHumi);

  server.serveStatic("/", SPIFFS, "/");
  
  server.begin(); //Start server
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}