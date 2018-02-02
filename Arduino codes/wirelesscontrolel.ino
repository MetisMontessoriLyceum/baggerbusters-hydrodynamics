#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define DHT11_PIN 7

EthernetClient ethClient;
PubSubClient client(ethClient);
const int AOUTpin= 0; 
int value;
int ledPin = 13;                // choose the pin for the LED
int inputPin = 2;               // choose the input pin (for PIR sensor)
int motorPin = 6;               // choose the pin the motor is connected to
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
char tbuf[10], hbuf[10], cobuf[10];
int motorTimeRemaining = 0;
 
// Update these with values suitable for your network.
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0E, 0xA0, 0xC2 // 192.168.178.66
};
IPAddress ip(10,0,0,51);
IPAddress server(10,0,0,50); // tweede raspbi

void callback(char* topic, String payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if(topic = "motorControll"){
    motorTimeRemaining += payload.toInt(payload); // FIXME: convert to int correctly!
  }
  
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("motorResponse","connectedMotor");
      client.subscribe("motorControll");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(){
  Serial.begin(9600);
  pinMode(AOUTpin, INPUT);
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
  pinMode(motorPin, OUTPUT);

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  Serial.print("Hey");
  client.loop();
  client.publish("motorResponse", "hi");



  delay(1000);
  if(motorTimeRemaining > 0){
    motorTimeRemaining -= 1;
    Serial.print(motorTimeRemaining);
    digitalWrite(motorPin, HIGH);
  }else{
    digitalWrite(motorPin, LOW);
  }
  
}
