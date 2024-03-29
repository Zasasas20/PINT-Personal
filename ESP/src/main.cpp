#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

#define EEPROM_SIZE 2
#define LEDPIN 27

const char* ssid = "TP-Link_7B03";
const char* pass = "58015742";

//const char* ssid = "Zaid";
//const char* pass = "Holdonbro";

const char* mqtt_server = "80.115.248.174";

hw_timer_t *SwitchTimer = NULL;

unsigned long lastMillis = 0;

WiFiClient espClient;
PubSubClient client(espClient);
TFT_eSPI tft=TFT_eSPI();

bool LEDS = false;
bool Auto = false;

void Switch(){
  LEDS = !LEDS;
  digitalWrite(LEDPIN, LEDS);

  EEPROM.write(0, LEDS);
  EEPROM.commit();
}

void TimerEnable(){
  timerAlarmEnable(SwitchTimer);
}

void TimerDisable(){
  timerAlarmDisable(SwitchTimer);
}

void UpdateAuto(){
  if (Auto){
    TimerEnable();
  }
  else {
    TimerDisable();
  }
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
    if (messageTemp == "AUTO"){
      Auto = !Auto;
      EEPROM.write(1, Auto);
      EEPROM.commit();
      UpdateAuto();
      client.publish("Light/ChipA", Auto ? "True" : "False");
    }
    else{
      Switch();
      client.publish("Light/Chip", LEDS ? "True" : "False");
    }
  }
  delay(100);
}

void setupWifi(){
  String str = "Connecting";
  tft.fillScreen(TFT_BLACK);
  tft.drawString(str.c_str(), 0, 100);
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

void Send(){
  client.publish("Light/Chip", LEDS ? "True" : "False");
  client.publish("Light/ChipA", Auto ? "True" : "False");
}

void IRAM_ATTR switchLight(){
  Switch();
}

void TimerSetup(){
  SwitchTimer = timerBegin(1, 80, true);
  timerAttachInterrupt(SwitchTimer, &switchLight ,true);
  timerAlarmWrite(SwitchTimer, 5000000, true);

  TimerEnable();
}

void EEPROMSetup(){
  EEPROM.begin(EEPROM_SIZE);
  LEDS = EEPROM.read(0);
  Auto = EEPROM.read(1);

  digitalWrite(LEDPIN, LEDS);
}

void setup() {

  tft.init();

  EEPROMSetup();

  pinMode(LEDPIN, OUTPUT);
  setupWifi();
  delay(100);
  setupMQTT();
  delay(100);

  TimerSetup();
  UpdateAuto();
}

void loop() {
  if (!client.connected()){
    TimerDisable();
    reconnecting();
    TimerEnable();
    UpdateAuto();
  }

  if (WiFi.status() != WL_CONNECTED){
    WiFi.disconnect();
    WiFi.reconnect();
  }

  client.loop();

    if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    Send();
  }
}