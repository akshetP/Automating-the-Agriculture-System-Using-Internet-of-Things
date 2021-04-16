#include <ESP8266WiFi.h>
#include <PubSubClient.h>  //callback    //reconnect 
#include "DHT.h"
#define DHTPIN D2
#define DHTTYPE DHT11 
int sensorPin = A0;
int sensorValue = 0;

DHT dht(DHTPIN,DHTTYPE);
const char* ssid = "Patel";          // SSID for your wifi Router
const char* passkey = "Kanta@1949";  //Password for wifi Router

const char* mqtt_server = "mqtt.fluux.io";  //mqtt server domain 
const char* topic = "waterpump";  // topic for mqtt

//const char* username = "";   //username for mqtt broker
//const char* password = "";  // password for mqtt broker

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, passkey);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String topicstr = topic;
   char sub[1000];
   int c =0;
  Serial.print("Message arrived [");        // Message Arrived [test] a
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == 'b') {
    digitalWrite(D4, LOW);
    client.publish("status","Water Pump is OFF.",false);
   Serial.println("D4 is OFF");
  }
  if ((char)payload[0] == 'a'){
    digitalWrite(D4, HIGH);
    Serial.println("D4 is ON");
    client.publish("status","Water Pump is ON.",false);
  }
  if ((char)payload[0] == 'c') {
    digitalWrite(D6, LOW); 
   // client.publish("testingpub","c",false);  
  }
  if ((char)payload[0] == 'd'){
    digitalWrite(D6, HIGH);  
    //client.publish("testingpub","d",false);
  }
  if ((char)payload[0] == 'e') {
    digitalWrite(D7, LOW);  
    //client.publish("testingpub","e",false);
  }
  if ((char)payload[0] == 'f'){
    digitalWrite(D7, HIGH); 
    //client.publish("testingpub","f",false); 
  }
  if ((char)payload[0] == 'g') {
    digitalWrite(D3, LOW);  
   // client.publish("testingpub","g",true);
  }
  if ((char)payload[0] == 'h'){
    digitalWrite(D3, HIGH); 
   // client.publish("testingpub","h",false);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
      String clientId = "Nodemcu";
    // Attempt to connect
    if (client.connect(clientId.c_str())) { // will message 
       Serial.println("connected");
       client.publish("Notifications","System is Live!",true);
       client.subscribe(topic);
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(D4, OUTPUT); 
  pinMode(D6, OUTPUT); 
  pinMode(D7, OUTPUT); 
  pinMode(D3, OUTPUT);
  pinMode(D0, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
  dht.begin();
}

void loop() {
    client.loop();
    if (!client.connected()) {
    reconnect();
  }
      sensorValue = analogRead(sensorPin);
    Serial.print("Moisture = " );
    int s = sensorValue;
    int s1 = map(s, 0, 620, 0, 100);
    Serial.println(sensorValue);
    delay(1000);
float h = dht.readHumidity();
float t = dht.readTemperature();
String temp = String(t);
String hum = String(h);
String mois = String(s1);
float f = dht.readTemperature(true);
client.publish("moisture", mois.c_str(), true);
client.publish("temperature", temp.c_str(), true);
client.publish("humidity", hum.c_str(), true);
Serial.print("Humidity:");
Serial.print(h);
Serial.print(" %\t");
Serial.println("");
Serial.print("Temperature:");
Serial.print(t);
Serial.print(" *C ");
Serial.println("");
delay(2000);
}
