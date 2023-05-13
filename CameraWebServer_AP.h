/** 
 * @description: Elegoo Smart Robot Car V4.0 [ESP32_CameraServer_AP_20220120 (Dev Module)]
 * @link https://www.elegoo.com/products/elegoo-smart-robot-car-kit-v-4-0
 * @version: ESP32_CameraServer_AP_20220120-> Modified For Arduino IDE 2.1.0
 * @author: Elegoo @LastEditors: Changhua @LastEditTime: 2020-07-23 14:21:48
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

#ifndef _CameraWebServer_AP_H
#define _CameraWebServer_AP_H
#include "esp_camera.h"
#include <WiFi.h>

class CameraWebServer_AP
{

public:
  void CameraWebServer_AP_Init(void);
  void SocketServer_Test(void);
  void FactoryTest(void);
  String wifi_name;

private:
  // const char *ssid = "ESP32_CAM";
  // const char *password = "elegoo2020";
  /* Must Use [const char] to prevent the (verbose)warning msg in the console
   v-warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings]
  char *ssid = "ELEGOO-";
  char *password = "elegoo2020";
  char *password = "";
   ^-warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings]*/
  const char *ssid = "ELEGOO-";
  const char *password = "";
};

#endif
