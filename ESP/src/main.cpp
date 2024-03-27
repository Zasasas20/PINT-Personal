#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Zaid";
const char* pass = "Holdonbro";

const char* mqtt_server = "80.115.248.174";

WiFiClient espClient;
PubSubClient client(espClient);
TFT_eSPI tft=TFT_eSPI();

bool LEDS = false;

void response(String reply){
  client.publish("Test/Reply", reply.c_str());
}

void Switch(){
  LEDS = !LEDS;
  digitalWrite(27, LEDS);
}

void callBack(char* topic, byte* message, unsigned int length){

  String str = "Message from: ";

  tft.fillScreen(TFT_BLACK);
  tft.drawString(str.c_str(), 0, 100);
  tft.drawString(topic, 0, 110);
  String messageTemp;

  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  
  if ((String) topic == "Light/New"){
    client.publish("Light/Chip", LEDS ? "True" : "False");
  }
  else {
    Switch();
    client.publish("Light/Chip", LEDS ? "True" : "False");
  }
  delay(100);
}

void setupWifi(){
  String str = "Connecting";
  tft.fillScreen(TFT_BLACK);
  tft.drawString(str.c_str(), 30, 100);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    str += ".";
    tft.fillScreen(TFT_BLACK);
    tft.drawString(str.c_str(), 0, 100);
  }

  tft.fillScreen(TFT_BLACK);
  tft.drawString("Connected to Internet", 0, 100);
}

void setupMQTT(){
  client.setServer(mqtt_server, 1883);
  client.setCallback(callBack);
}

void reconnecting(){
  String str = "Reconnecting";
  while(!client.connected()){
    tft.fillScreen(TFT_BLACK);
    tft.drawString(str.c_str(), 0, 100);

    if (client.connect("TheEsp")){
      tft.fillScreen(TFT_BLACK);
      tft.drawString("Connected", 30, 100, 7);
      client.subscribe("Light/New");
      client.subscribe("Light/Control");
    }
    else{
      str += ".";
      delay(1000);
    }
  }
}

void setup() {

  tft.init();

  pinMode(27, OUTPUT);
  setupWifi();
  delay(100);
  setupMQTT();
  delay(100);
}

void loop() {
  if (!client.connected()){
    reconnecting();
  }
  client.loop();
}