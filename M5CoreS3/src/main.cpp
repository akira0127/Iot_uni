#include <Arduino.h>
#include <M5CoreS3.h>

void setup()
{
  M5.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.println("hello world");
  M5.Lcd.println("hello world");
  delay(10000);
}

// #include <M5Stack.h>
// #include <Wire.h>
// #include "NineAxesMotion.h"
// #include <ESP32_BME280_I2C.h>
// #include <PubSubClient.h>
// #include <WiFi.h>
// #include <WiFiManager.h>
// #include <DNSServer.h>
// #include <time.h>

// M5Stack lcd;
// NineAxesMotion mySensor;
// unsigned long lastStreamTime = 0;
// const int streamPeriod = 250;
// bool updateSensorData = true;

// // WiFi設定
// const char* wifiSsid = "YOUR_WIFI_SSID";
// const char* wifiPassword = "YOUR_WIFI_PASSWORD";

// // MQTT設定
// const char* mqttHost = "192.168.1.6"; // MQTTのIPかホスト名
// const int mqttPort = 1883;
// WiFiClient wifiClient;
// PubSubClient mqttClient(wifiClient);

// const char* topic = "itoyuNineAxis";
// const char* topicEuler = "itoyuNineAxis/Euler";
// char payload[100];

// const char* ntpServer = "ntp.jst.mfeed.ad.jp";
// const long gmtOffset_sec = 9 * 3600;
// const int daylightOffset_sec = 0;
// struct tm timeInfo;
// char datetime[20];

// void setup() {
//   M5.begin();
//   lcd.begin();
//   lcd.setBrightness(200); // バックライトの明るさ

//   WiFiManager wifiManager;
//   if (!wifiManager.autoConnect("OnDemandAP")) {
//     Serial.println("failed to connect and hit timeout");
//     delay(3000);
//     ESP.restart();
//     delay(5000);
//   }

//   Serial.begin(115200);
//   mySensor.initSensor();
//   mySensor.setOperationMode(OPERATION_MODE_NDOF);
//   mySensor.setUpdateMode(MANUAL);
//   mySensor.updateAccelConfig();
//   updateSensorData = true;
//   uint8_t t_sb = 0;
//   uint8_t filter = 4;
//   uint8_t osrs_t = 2;
//   uint8_t osrs_p = 5;
//   uint8_t osrs_h = 1;
//   uint8_t Mode = 3;
//   bme280i2c.ESP32_BME280_I2C_Init(t_sb, filter, osrs_t, osrs_p, osrs_h, Mode);

//   Serial.println();
//   Serial.println("Default accelerometer configuration settings...");
//   Serial.print("Range: ");
//   Serial.println(mySensor.readAccelRange());
//   Serial.print("Bandwidth: ");
//   Serial.println(mySensor.readAccelBandwidth());
//   Serial.print("Power Mode: ");
//   Serial.println(mySensor.readAccelPowerMode());
//   Serial.println("Streaming in ...");
//   Serial.print("3...");
//   delay(1000);
//   Serial.print("2...");
//   delay(1000);
//   Serial.println("1...");
//   delay(1000);
// }

// void loop() {
//   if (updateSensorData) {
//     payload[0] = '\0'; // Initialize payload
//     mySensor.updateAccel();
//     mySensor.updateEuler();
//     mySensor.updateLinearAccel();
//     mySensor.updateGravAccel();
//     mySensor.updateCalibStatus();
//     updateSensorData = false;
//   }

//   if ((millis() - lastStreamTime) >= streamPeriod) {
//     lastStreamTime = millis();
//     lcd.clear();
//     lcd.setCursor(0, 0);

//     float temperature = (float)bme280i2c.Read_Temperature();
//     float humidity = (float)bme280i2c.Read_Humidity();
//     uint16_t pressure = (uint16_t)round(bme280i2c.Read_Pressure());

//     // Get current time
//     configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
//     getLocalTime(&timeInfo);
//     strftime(datetime, 20, "%Y%m%d%H%M%S", &timeInfo);

//     lcd.print("Time: ");
//     lcd.print(datetime);
//     lcd.println("ms");

//     lcd.print("Temperature: ");
//     lcd.print(temperature);
//     lcd.println(" ℃");

//     lcd.print("Humidity: ");
//     lcd.print(humidity);
//     lcd.println(" %");

//     lcd.print("Pressure: ");
//     lcd.print(pressure);
//     lcd.println(" hPa");

//     String json = "{";
//     json += "\"datetime\":\"";
//     json += datetime;
//     json += "\",";
//     json += "\"ax\":";
//     json += mySensor.readAccelerometer(X_AXIS);
//     json += ",";
//     json += "\"ay\":";
//     json += mySensor.readAccelerometer(Y_AXIS);
//     json += ",";
//     json += "\"az\":";
//     json += mySensor.readAccelerometer(Z_AXIS);
//     json += ",";
//     json += "\"lx\":";
//     json += mySensor.readLinearAcceleration(X_AXIS);
//     json += ",";
//     json += "\"ly\":";
//     json += mySensor.readLinearAcceleration(Y_AXIS);
//     json += ",";
//     json += "\"lz\":";
//     json += mySensor.readLinearAcceleration(Z_AXIS);
//     json += ",";
//     json += "\"gx\":";
//     json += mySensor.readGravAcceleration(X_AXIS);
//     json += ",";
//     json += "\"gy\":";
//     json += mySensor.readGravAcceleration(Y_AXIS);
//     json += ",";
//     json += "\"gz\":";
//     json += mySensor.readGravAcceleration(Z_AXIS);
//     json += "}";

//     lcd.println(json);

//     // MQTT
//     if (!mqttClient.connected()) {
//       mqttClient.setServer(mqttHost, mqttPort);
//       while (!mqttClient.connected()) {
//         Serial.println("Connecting to MQTT...");
//         String clientId = "ESP32-";
//         if (mqttClient.connect(clientId.c_str())) {
//           Serial.println("connected");
//         }
//         delay(3000);
//         randomSeed(micros());
//       }
//     }

//     mqttClient.loop();
//     json.toCharArray(payload, 100);
//     Serial.println(payload);
//     if (!mqttClient.publish(topic, payload)) {
//       Serial.println("failed publish");
//     }
//     mqttClient.publish(topicEuler, "Euler data here"); // Replace with actual Euler data
//     updateSensorData = true;
//   }
// }
