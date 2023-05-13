/** 
 * @description: Elegoo Smart Robot Car V4.0 [ESP32_CameraServer_AP_20220120 (Dev Module)]
 * @link https://www.elegoo.com/products/elegoo-smart-robot-car-kit-v-4-0
 * @version: ESP32_CameraServer_AP_20220120-> Modified For Arduino IDE 2.1.0
 * @author: Elegoo @LastEditors: Changhua @LastEditTime: 2020-09-07 09:40:03
 * @date: 2023-05-12
 * @last_editor: (™T©ReMeTaL-Modified: For Arduino IDE 2.1.0)
 * @last_edit_time: 2023-05-12 21:27:03
 * ------------------------------------
 * @board: ESP32 Dev Module
 * @partition_scheme: Huge APP (3MB No OTA/1MB SPIFFS)
 * @psram: enabled
 * */

// WARNING!!! Make sure that you have selected
//    Board ---> ESP32 Dev Module
//    Partition Scheme ---> Huge APP (3MB No OTA/1MB SPIFFS)
//    PSRAM ---> enabled

#include "CameraWebServer_AP.h"
#include "esp_camera.h"
#include <WiFi.h>

#define RXD2 33
#define TXD2 4
CameraWebServer_AP CameraWebServerAP;

void setup() {
  Serial.begin(9600); // 115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //http://192.168.4.1/control?var=framesize&val=3
  //http://192.168.4.1/Test?var=
  CameraWebServerAP.CameraWebServer_AP_Init();
  delay(100);
  // while (Serial.read() >= 0)
  // {
  //   /*清空串口缓存...*/
  // }
  // while (Serial2.read() >= 0)
  // {
  //   /*清空串口缓存...*/
  // }
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.println("Elegoo-2020...");
  Serial2.print("{Factory}");
}

void loop() {
  CameraWebServerAP.SocketServer_Test();
  CameraWebServerAP.FactoryTest();
}
