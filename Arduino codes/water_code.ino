#include <Arduino.h>
#define USE_SERIAL Serial
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//variabelen
const int  buttonPin = D5;
float calibrationFactor = 3.3;
volatile byte pulseCount = 0;
float flowRate;
unsigned int flowMilliliters;
unsigned long totalMilliliters;
unsigned long oldTime;
unsigned long oldtotalMilliliters;
bool sturen = false;
char string_mLiters[50];

//wifi instellingen
const char* ssid = "baggerwifi";
const char* password = "baggerbusters";
const char* wifiName = "test";
const char* mqtt_server = "10.0.0.50";
WiFiClient espClient;
PubSubClient client(espClient);

void pin_ISR(){
 {   
  pulseCount++;
 }
}
 
void setup_wifi() {
 delay(10);
 WiFi.mode(WIFI_STA);
 WiFi.hostname(wifiName);
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
 }
 Serial.print("WiFi connected - ESP IP address: ");
 Serial.println(WiFi.localIP());
 Serial.printf("Connection status: %d\n", WiFi.status());
}

void reconnect() {
 while (!client.connected()) {
  if (client.connect(wifiName)) {
   client.publish("debug", "Reconnected");
   client.subscribe("waterflow1");
  } 
  else {
   delay(5000);
   Serial.print("waiting...");
  }
 }
}

void bereken_milliliters(){ 
 if ((millis() - oldTime) > 1000){
  flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
  oldTime = millis();
  flowMilliliters = (flowRate / 60) * 1000;
  totalMilliliters += flowMilliliters;
  pulseCount = 0;
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  delay(10);
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), pin_ISR, RISING);
}

void loop() {
 if (!client.connected()) {
  reconnect();
 }
 if (!client.loop()){
  client.connect(wifiName);
 }
 delay(300);
 bereken_milliliters();
 delay(300);
 bereken_milliliters();
 delay(300);
 bereken_milliliters();
 delay(300);
 if (oldtotalMilliliters == totalMilliliters) {
  if (sturen){
   Serial.println("milliliters");
   Serial.println(totalMilliliters);
   sprintf(string_mLiters,"%ld", totalMilliliters);
   client.publish("waterflow1/mLiter", string_mLiters);
   sturen = false;
   totalMilliliters = 0;
  }
 }
 else{
  sturen = true;
 }
 oldtotalMilliliters = totalMilliliters;
}

