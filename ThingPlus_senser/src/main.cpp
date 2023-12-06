// #include <PubSubClient.h>
// #include <WiFiManager.h>
// #include <WiFi.h>
// #include <Wire.h>
// #include <ArduinoJson.h>
// #include <Arduino.h>
// #include <Wire.h>

// #include "SparkFun_BNO08x_Arduino_Library.h" // CTRL+Click here to get the library: http://librarymanager/All#SparkFun_BNO08x
// BNO08x myIMU;

// const char *mqttHost = "192.168.0.21"; // MQTTブローカーのIPアドレスまたはホスト名
// const int mqttPort = 1883;             // MQTTブローカーのポート
// const char *topic = "iot";

// WiFiClient wifiClient;
// PubSubClient mqttClient(wifiClient);

// void setup()
// {
//   Serial.begin(115200);
//   Serial.println();
//   Serial.println("BNO08x Read Example");

//   Wire.begin();

//   if (myIMU.begin() == false)
//   {
//     Serial.println("BNO08x not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
//     while (1)
//       ;
//   }
//   Serial.println("BNO08x found!");

//   // Wire.setClock(400000); //Increase I2C data rate to 400kHz

//   setReports();

//   Serial.println("Reading events");
//   delay(100);
//   // WiFiManagerのセットアップコード
//   WiFiManager wifiManager;
//   // WiFiManagerのリセット設定（オプション）
//   // wifiManager.resetSettings();
//   if (!wifiManager.autoConnect("AutoConnectAP"))
//   {
//     Serial.println("接続に失敗しました。設定ポータルを開きます。");
//     delay(3000);
//     ESP.restart(); // リセットして再試行するか、ディープスリープモードに移行する
//     delay(5000);
//   }
//   Serial.println("Wi-Fiに接続しました。");
//   mqttClient.setServer(mqttHost, mqttPort);
// }

// // Here is where you define the sensor outputs you want to receive
// void setReports(void)
// {
//   Serial.println("Setting desired reports");
//   if (myIMU.enableRotationVector() == true)
//   {
//     Serial.println(F("Rotation vector enabled"));
//     Serial.println(F("Output in form roll, pitch, yaw"));
//   }
//   else
//   {
//     Serial.println("Could not enable rotation vector");
//   }
// }

// void loop()
// {
//   delay(10);

//   if (myIMU.wasReset())
//   {
//     Serial.print("sensor was reset ");
//     setReports();
//   }

//   // Has a new event come in on the Sensor Hub Bus?
//   if (myIMU.getSensorEvent() == true)
//   {

//     // is it the correct sensor data we want?
//     if (myIMU.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR)
//     {

//       float roll = (myIMU.getRoll()) * 180.0 / PI;   // Convert roll to degrees
//       float pitch = (myIMU.getPitch()) * 180.0 / PI; // Convert pitch to degrees
//       float yaw = (myIMU.getYaw()) * 180.0 / PI;     // Convert yaw / heading to degrees

//       Serial.print(roll, 1);
//       Serial.print(F(","));
//       Serial.print(pitch, 1);
//       Serial.print(F(","));
//       Serial.print(yaw, 1);

//       Serial.println();
//     }
//   }

//   if (!mqttClient.connected())
//   {

//     while (!mqttClient.connected())
//     {
//       Serial.println("Connecting to MQTT...");
//       String clientId = "ESP32-iot_uni";
//       if (mqttClient.connect(clientId.c_str()))
//       {
//         Serial.println("connected");
//       }
//       delay(3000);
//       randomSeed(micros());
//     }
//     mqttClient.loop();
//     // センサーデータをJSON形式に変換
//     DynamicJsonDocument jsonDoc(256); // ここでは適切なサイズを指定してください
//     jsonDoc["roll"] = roll;
//     jsonDoc["pitch"] = pitch;
//     jsonDoc["yaw"] = yaw;

//     // JSONを文字列に変換
//     String jsonString;
//     serializeJson(jsonDoc, jsonString);

//     // MQTTブローカーにメッセージを送信
//     mqttClient.publish("topic", jsonString.c_str());

//     // 一定の間隔でセンサーデータを送信
//     delay(5000); // 5000ミリ秒（5秒）ごとに送信
//   }
// }

#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <Wire.h>
#include <ArduinoJson.h>

// WiFiの設定
const char ssid[] = "CPSLAB_WLX";
const char password[] = "	6bepa8ideapbu";
const char *mqttHost = "192.168.56.1";
const int mqttPort = 1884;
// ここまで
const char *topic = "/test";
char *payload;

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

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  // // WiFiManagerのセットアップコード
  // WiFiManager wifiManager;
  // // WiFiManagerのリセット設定（オプション）
  // // wifiManager.resetSettings();
  // if (!wifiManager.autoConnect("AutoConnectAP"))
  // {
  //   Serial.println("接続に失敗しました。設定ポータルを開きます。");
  //   delay(3000);
  //   ESP.restart(); // リセットして再試行するか、ディープスリープモードに移行する
  //   delay(5000);
  // }
  connectWiFi();
  mqttClient.setServer(mqttHost, mqttPort);
  connectMqtt();
}

void loop()
{

  // センサーデータをJSON形式に変換
  DynamicJsonDocument jsonDoc(256); // ここでは適切なサイズを指定してください
  jsonDoc["sensor_type"] = "temperature";
  jsonDoc["value"] = 25.5;
  jsonDoc["unit"] = "Celsius";

  // JSONを文字列に変換
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  mqttClient.publish(topic, jsonString.c_str());
  delay(1000);

  // MQTTブローカーにメッセージを送信
  if (WiFi.status() == WL_DISCONNECTED)
  {
    connectWiFi();
  }
  mqttloop();
  delay(100);

  // 一定の間隔でセンサーデータを送信
  delay(5000); // 5000ミリ秒（5秒）ごとに送信
}
