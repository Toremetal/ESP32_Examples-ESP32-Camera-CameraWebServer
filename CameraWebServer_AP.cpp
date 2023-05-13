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

//重要配置：
// #define HTTPD_DEFAULT_CONFIG()             \
//   {                                        \
//     .task_priority = tskIDLE_PRIORITY + 5, \
//     .stack_size = 4096,                    \
//     .server_port = 80,                     \
//     .ctrl_port = 32768,                    \
//     .max_open_sockets = 7,                 \
//     .max_uri_handlers = 8,                 \
//     .max_resp_headers = 8,                 \
//     .backlog_conn = 5,                     \
//     .lru_purge_enable = false,             \
//     .recv_wait_timeout = 5,                \
//     .send_wait_timeout = 5,                \
//     .global_user_ctx = NULL,               \
//     .global_user_ctx_free_fn = NULL,       \
//     .global_transport_ctx = NULL,          \
//     .global_transport_ctx_free_fn = NULL,  \
//     .open_fn = NULL,                       \
//     .close_fn = NULL,                      \
//   }

// Select camera model
// #define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM

#define CAMERA_MODEL_M5STACK_WIDE

//#define CAMERA_MODEL_AI_THINKER
// #include "BLEAdvertisedDevice.h"
// BLEAdvertisedDevice _BLEAdvertisedDevice;

#include "sensor.h"
#include "CameraWebServer_AP.h"
#include "camera_pins.h"
#include "esp_system.h"
WiFiServer server(100);

bool WA_en = false;

void setupLedFlash(int pin);
void startCameraServer();

void CameraWebServer_AP::CameraWebServer_AP_Init(void) {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;        //20000000
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  //init with high specs to pre-allocate larger buffers
  config.frame_size = FRAMESIZE_UXGA;  //FRAMESIZE_SVGA;
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }
#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  sensor_t *s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  //drop down frame size for higher initial frame rate
  //s->set_framesize(s, FRAMESIZE_SXGA); //字节长度采样值:60000                 #9 (画质高)  1280x1024
  s->set_framesize(s, FRAMESIZE_SVGA);  //字节长度采样值:40000                   #7 (画质中)  800x600
  // s->set_framesize(s, FRAMESIZE_QVGA); //字节长度采样值:10000                #4 (画质低)  320x240
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

  s->set_vflip(s, 0);    //图片方向设置（上下）
  s->set_hmirror(s, 0);  //图片方向设置（左右）
#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

  Serial.println("\r\n");

  uint64_t chipid = ESP.getEfuseMac();
  char string[10];
  sprintf(string, "%04X", (uint16_t)(chipid >> 32));
  String mac0_default = String(string);
  sprintf(string, "%08X", (uint32_t)chipid);
  String mac1_default = String(string);
  String url = ssid + mac0_default + mac1_default;
  const char *mac_default = url.c_str();

  Serial.println(":----------------------------:");
  Serial.print("wifi_name:");
  Serial.println(mac_default);
  Serial.println(":----------------------------:");
  wifi_name = mac0_default + mac1_default;

  WiFi.mode(WIFI_AP);
  WiFi.softAP(mac_default, password, 9);
  startCameraServer();
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("' to connect");
  server.begin();
}

void CameraWebServer_AP::SocketServer_Test(void) {
  static bool ED_client = true;
  WiFiClient client = server.available();  //尝试建立客户对象
  if (client)                              //如果当前客户可用
  {
    WA_en = true;
    ED_client = true;
    Serial.println("[Client connected]");
    String readBuff;
    String sendBuff;
    uint8_t Heartbeat_count = 0;
    bool Heartbeat_status = false;
    bool data_begin = true;
    while (client.connected())  //如果客户端处于连接状态
    {
      if (client.available())  //如果有可读数据
      {
        char c = client.read();              //读取一个字节
        Serial.print(c);                     //从串口打印
        if (true == data_begin && c == '{')  //接收到开始字符
        {
          data_begin = false;
        }
        if (false == data_begin && c != ' ')  //去掉空格
        {
          readBuff += c;
        }
        if (false == data_begin && c == '}')  //接收到结束字符
        {
          data_begin = true;
          if (true == readBuff.equals("{Heartbeat}")) {
            Heartbeat_status = true;
          } else {
            Serial2.print(readBuff);
          }
          //Serial2.print(readBuff);
          readBuff = "";
        }
      }
      if (Serial2.available()) {
        char c = Serial2.read();
        sendBuff += c;
        if (c == '}')  //接收到结束字符
        {
          client.print(sendBuff);
          Serial.print(sendBuff);  //从串口打印
          sendBuff = "";
        }
      }

      static unsigned long Heartbeat_time = 0;
      if (millis() - Heartbeat_time > 1000)  //心跳频率
      {
        client.print("{Heartbeat}");
        if (true == Heartbeat_status) {
          Heartbeat_status = false;
          Heartbeat_count = 0;
        } else if (false == Heartbeat_status) {
          Heartbeat_count += 1;
        }
        if (Heartbeat_count > 3) {
          Heartbeat_count = 0;
          Heartbeat_status = false;
          break;
        }
        Heartbeat_time = millis();
      }
      static unsigned long Test_time = 0;
      if (millis() - Test_time > 1000)  //定时检测连接设备
      {
        Test_time = millis();
        //Serial2.println(WiFi.softAPgetStationNum());
        if (0 == (WiFi.softAPgetStationNum()))  //如果连接的设备个数为“0” 则向车模发送停止命令
        {
          Serial2.print("{\"N\":100}");
          break;
        }
      }
    }
    Serial2.print("{\"N\":100}");
    client.stop();  //结束当前连接:
    Serial.println("[Client disconnected]");
  } else {
    if (ED_client == true) {
      ED_client = false;
      Serial2.print("{\"N\":100}");
    }
  }
}
/*作用于测试架*/
void CameraWebServer_AP::FactoryTest(void) {
  static String readBuff;
  String sendBuff;
  if (Serial2.available()) {
    char c = Serial2.read();
    readBuff += c;
    if (c == '}')  //接收到结束字符
    {
      if (true == readBuff.equals("{BT_detection}")) {
        Serial2.print("{BT_OK}");
        Serial.println("Factory...");
      } else if (true == readBuff.equals("{WA_detection}")) {
        Serial2.print("{");
        Serial2.print(wifi_name);  // CameraWebServerAP.wifi_name);
        Serial2.print("}");
        Serial.println("Factory...");
      }
      readBuff = "";
    }
  }
  {
    if ((WiFi.softAPgetStationNum()))  //连接的设备个数不为“0” led指示灯长亮
    {
      if (true == WA_en) {
        digitalWrite(13, LOW);
        Serial2.print("{WA_OK}");
        WA_en = false;
      }
    } else {
      //获取时间戳 timestamp
      static unsigned long Test_time;
      static bool en = true;
      if (millis() - Test_time > 100) {
        if (false == WA_en) {
          Serial2.print("{WA_NO}");
          WA_en = true;
        }
        if (en == true) {
          en = false;
          digitalWrite(13, HIGH);
        } else {
          en = true;
          digitalWrite(13, LOW);
        }
        Test_time = millis();
      }
    }
  }
}