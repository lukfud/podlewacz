/*
Copyright (C) by lukfud and yellow rubber duck
*/

#ifndef SRC_SUPLA_SENSOR_PODLEWACZ_H_
#define SRC_SUPLA_SENSOR_PODLEWACZ_H_

#ifndef ARDUINO_ARCH_AVR
// Arduino Mega can't establish https connection, so it can't be supported

//#include <WiFiClientSecure.h>
#include <supla/network/client.h>
#include <supla/action_handler.h>
#include <supla/sensor/virtual_binary.h>

#define APIURL_MAX_LENGTH 33
#define DEFAULT_SERVER_REFRESH_RATE 21600

namespace Supla {
namespace Sensor {
class Podlewacz : public VirtualBinary {
 public:
  Podlewacz(const char *apiUrlValue);
  void setServerRefreshRate(unsigned int min);
  void setActionValue(int value);
  void setUrlValue(const char *apiUrlValue);
  void onInit();
  void iterateAlways();
  bool iterateConnected();

 protected:
  //WiFiClientSecure sslClient;
  Supla::Client *sslClient = nullptr;
  String strBuffer;
  uint8_t retryCounter = 0;
  uint16_t httpStatusCode = 0;
  int actionValue = -1;
  bool dataFetchInProgress;
  uint32_t connectionTimeoutMs;
  uint32_t lastServerReadTime;
  uint32_t refreshRateSec;
  char buf[1024];
  char apiUrl[APIURL_MAX_LENGTH] = {};

};
};  // namespace Sensor
};  // namespace Supla

#endif
#endif  // SRC_SUPLA_SENSOR_PODLEWACZ_H_
