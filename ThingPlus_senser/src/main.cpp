#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <Wire.h>

#include "SparkFun_BNO08x_Arduino_Library.h" // CTRL+Click here to get the library: http://librarymanager/All#SparkFun_BNO08x
BNO08x myIMU;

// WiFiの設定
const char ssid[] = "CPSLAB_WLX";
const char password[] = "6bepa8ideapbu";
const char *mqttHost = "172.16.1.68";
const int mqttPort = 1884;
// ここまで
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

// Here is where you define the sensor outputs you want to receive
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

  if (myIMU.begin() == false)
  {
    Serial.println("BNO08x not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1)
      ;
  }
  Serial.println("BNO08x found!");

  // Wire.setClock(400000); //Increase I2C data rate to 400kHz

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
  // センサーデータをJSON形式に変換
  StaticJsonDocument<capacity> jsonDoc; // ここでは適切なサイズを指定してください

  if (myIMU.wasReset())
  {
    Serial.print("sensor was reset ");
    setReports();
  }

  // Has a new event come in on the Sensor Hub Bus?
  if (myIMU.getSensorEvent() == true)
  {

    // is it the correct sensor data we want?
    if (myIMU.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR)
    {
      float roll = (myIMU.getRoll()) * 180.0 / PI;   // Convert roll to degrees
      float pitch = (myIMU.getPitch()) * 180.0 / PI; // Convert pitch to degrees
      float yaw = (myIMU.getYaw()) * 180.0 / PI;     // Convert yaw / heading to degrees
      Serial.print(roll, 1);
      Serial.print(F(","));
      Serial.print(pitch, 1);
      Serial.print(F(","));
      Serial.print(yaw, 1);

      Serial.println();
      jsonDoc["roll"] = roll;
      jsonDoc["pitch"] = pitch;
      jsonDoc["yaw"] = yaw;
    }
  }

  // JSONを文字列に変換
  serializeJson(jsonDoc, json);

  // MQTTブローカーにメッセージを送信
  mqttClient.publish("topic", json);

  mqttClient.publish(topic, json);
  Serial.println(json);
  delay(1000);

  // MQTTブローカーにメッセージを送信
  if (WiFi.status() == WL_DISCONNECTED)
  {
    connectWiFi();
  }
  mqttloop();
  delay(100);
  // 一定の間隔でセンサーデータを送信
}
