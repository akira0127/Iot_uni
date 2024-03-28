#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <Wire.h>
// Define as -1 to disable these features.
#define BNO08X_INT A4
// #define BNO08X_INT  -1
#define BNO08X_RST A5
// #define BNO08X_RST  -1

#define BNO08X_ADDR 0x4B // SparkFun BNO08x Breakout (Qwiic) defaults to 0x4B
// #define BNO08X_ADDR 0x4A // Alternate address if ADR jumper is closed

#include "SparkFun_BNO08x_Arduino_Library.h"
BNO08x myIMU;

// WiFiの設定
const char ssid[] = "";
const char password[] = "";
const char *mqttHost = "";
const int mqttPort = 1884;
const char *topic = "/test";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void connectWiFi()
{
  WiFi.begin(ssid, password);
  Serial.printf("connecting to %s\n", ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.printf("\nWiFi connected\n");
}

void connectMqtt()
{
  while (!mqttClient.connected())
  {
    Serial.println("connecting to MQTT...");
    String clientID = "M5Stack-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientID.c_str()))
    {
      Serial.println("connected");
    }
    else
    {
      delay(1000);
      randomSeed(micros());
    }
  }
}

void mqttloop()
{
  if (!mqttClient.connected())
  {
    connectMqtt();
  }
  mqttClient.loop();
}

void setReports(void)
{
  Serial.println("Setting desired reports");
  if (myIMU.enableRotationVector() == true)
  {
    Serial.println(F("Rotation vector enabled"));
    Serial.println(F("Output in form roll, pitch, yaw"));
  }
  else
  {
    Serial.println("Could not enable rotation vector");
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO08x Read Example");

  Wire.begin();

  if (myIMU.begin(BNO08X_ADDR, Wire, BNO08X_INT, BNO08X_RST) == false)
  {
    Serial.println("BNO08x not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1)
      ;
  }
  Serial.println("BNO08x found!");

  setReports();

  Serial.println("Reading events");
  delay(100);

  connectWiFi();
  mqttClient.setServer(mqttHost, mqttPort);
  connectMqtt();
}

void loop()
{
  delay(10);
  char json[256];
  const size_t capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> jsonDoc;

  static float prevRoll = 0;
  static float prevPitch = 0;
  static float prevYaw = 0;

  if (myIMU.wasReset())
  {
    Serial.print("sensor was reset ");
    setReports();
  }

  if (myIMU.getSensorEvent() == true)
  {
    if (myIMU.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR)
    {
      float roll = (myIMU.getRoll()) * 180.0 / PI;
      float pitch = (myIMU.getPitch()) * 180.0 / PI;
      float yaw = (myIMU.getYaw()) * 180.0 / PI;

      // Check if the absolute difference between current and previous values is greater than 50
      if (abs(roll - prevRoll) >= 50 || abs(pitch - prevPitch) >= 50 || abs(yaw - prevYaw) >= 50)
      {
        delay(1);
        // Perform additional getSensorEvent() to get new values if needed
        myIMU.getSensorEvent(); // Perform 2nd getSensorEvent() to get new values

        roll = (myIMU.getRoll()) * 180.0 / PI;
        pitch = (myIMU.getPitch()) * 180.0 / PI;
        yaw = (myIMU.getYaw()) * 180.0 / PI;
        if (abs(roll - prevRoll) >= 50 || abs(pitch - prevPitch) >= 50 || abs(yaw - prevYaw) >= 50)
        {
          delay(1);
          // Perform additional getSensorEvent() to get new values if needed
          myIMU.getSensorEvent(); // Perform 2nd getSensorEvent() to get new values

          roll = (myIMU.getRoll()) * 180.0 / PI;
          pitch = (myIMU.getPitch()) * 180.0 / PI;
          yaw = (myIMU.getYaw()) * 180.0 / PI;
        }
      }

      prevRoll = roll;
      prevPitch = pitch;
      prevYaw = yaw;

      Serial.print(roll, 1);
      Serial.print(F(","));
      Serial.print(pitch, 1);
      Serial.print(F(","));
      Serial.print(yaw, 1);

      Serial.println();

      if (!isnan(roll) && !isnan(pitch) && !isnan(yaw))
      {
        jsonDoc["z"] = roll;
        jsonDoc["y"] = pitch;
        jsonDoc["x"] = yaw;

        serializeJson(jsonDoc, json);
        mqttClient.publish(topic, json);
        Serial.println(json);
      }
    }
  }

  if (WiFi.status() == WL_DISCONNECTED)
  {
    connectWiFi();
  }

  mqttloop();
  delay(100);
}
