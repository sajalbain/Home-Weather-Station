#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include <ArduinoJson.h>
// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
StaticJsonBuffer<200> jsonBuffer;
IPAddress local_IP(192, 168, 1, 253); //ESP static ip
IPAddress gateway(192, 168, 1, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(192, 168, 1, 1);  //DNS
const char* deviceName = "temperature.sensor";
/*Put your SSID & Password*/
const char* ssid = "<SSID>";  // Enter SSID here
const char* password = "<PASSWORD>";  //Enter Password here
ESP8266WebServer server(80);

// DHT Sensor
uint8_t DHTPin = D1; 
               
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

float Temperature;
float Humidity;
 
void setup() {
  Serial.begin(115200);
  delay(100);
  
  pinMode(DHTPin, INPUT);

  dht.begin();              

  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
  Serial.println("STA Failed to configure");
  }

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

}
void loop() {
  
  server.handleClient();
  
}

void handle_OnConnect() {
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 
  server.send(200, "text/html", SendHTML(Temperature,Humidity)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float Temperaturestat,float Humiditystat){
  String ptr = "{\"Temperature\":";
  ptr +=Temperaturestat;
  ptr +=",";
  ptr +="\"Humidity\":";
  ptr +=Humiditystat;
  ptr +="}";
  char json[]= '';
  ptr.toCharArray(json, 200);
  JsonObject& root = jsonBuffer.parseObject(json);
  if(!root.success()) {
  Serial.println("parseObject() failed");
  return ptr;
}
  Serial.println(root);
  return ptr;
}
