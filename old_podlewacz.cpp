/*
Copyright (C) by lukfud and yellow rubber duck
*/

#ifndef ARDUINO_ARCH_AVR

#include "podlewacz.h"
#include <supla/log_wrapper.h>
#include "supla/events.h"

using namespace Supla;
using namespace Sensor;

Podlewacz::Podlewacz(const char *apiUrlValue) :
      buf{},
      retryCounter(0),
      dataFetchInProgress(false),
      connectionTimeoutMs(0)
{
  refreshRateSec = 6*60*60; // refresh every 6h

  int len = strlen(apiUrlValue);
  if (len > APIURL_MAX_LENGTH) {
    len = APIURL_MAX_LENGTH;
  }
  strncpy(apiUrl, apiUrlValue, len);
}

void Podlewacz::setUrlValue(const char *apiUrlValue) {
  if (strcmp(apiUrl, apiUrlValue) != 0) {
    memset(apiUrl, 0, sizeof(apiUrl));
    int len = strlen(apiUrlValue);
    if (len > APIURL_MAX_LENGTH) {
      len = APIURL_MAX_LENGTH;
    }
    strncpy(apiUrl, apiUrlValue, len);
    SUPLA_LOG_DEBUG("# podlewa.cz - saved url: %s", apiUrl);
  }
}

void Podlewacz::iterateAlways() {
  VirtualBinary::iterateAlways();
  if (dataFetchInProgress) {
    if (millis() - connectionTimeoutMs > 30000) {
      SUPLA_LOG_DEBUG("# podlewa.cz - connection timeout, "
                                              "remote host is not responding");
      sslClient.stop();
      dataFetchInProgress = false;
      return;
    }
    if (!sslClient.connected()) {
      SUPLA_LOG_DEBUG("# podlewa.cz - fetch completed");
      dataFetchInProgress = false;
    }
    if (sslClient.available()) {
      SUPLA_LOG_DEBUG("# podlewa.cz - reading data: %d",
                                                        sslClient.available());
    }
    while (sslClient.available()) {
      strBuffer = sslClient.readStringUntil('\n');
      if (strBuffer.startsWith("HTTP")) {
        sscanf(strBuffer.c_str(), "HTTP/%*d.%*d %d", &httpStatusCode);
      }
      if (httpStatusCode != 200) {
        SUPLA_LOG_DEBUG("# podlewa.cz - request status code: %d, "
                                        "sprinklers unlocked", httpStatusCode);
        state = 0;
        setActionValue(2);
        sslClient.stop();
        break;
      } else {
        if (strBuffer.length() == 1) {
          strBuffer = sslClient.readStringUntil('\n');
          if ((strBuffer == "0") || (strBuffer == "1")) {
            SUPLA_LOG_DEBUG("# podlewa.cz - sprinklers status: %s", strBuffer);
            state = !strBuffer.toInt();
            setActionValue(state);
          } else {
            SUPLA_LOG_DEBUG(
                          "# podlewa.cz - unknown error, sprinklers unlocked");
            state = 0;
            setActionValue(state);
          }
        }
      }
    }
    if (!sslClient.connected()) {
      sslClient.stop();
    }
  }
/*
  if (!dataFetchInProgress) {
    if (lastServerReadTime == 0 || millis() - lastServerReadTime > 
                             (retryCounter > 0 ? 5000 : refreshRateSec*1000)) {
      lastServerReadTime = millis();
#ifdef ARDUINO_ARCH_ESP32
      const char* root_ca = \
      "-----BEGIN CERTIFICATE-----\n" \
      "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\n" \
      "-----END CERTIFICATE-----\n";
      sslClient.setCACert(root_ca);
      sslClient.setHandshakeTimeout(120);
#endif
#ifdef ARDUINO_ARCH_ESP8266
      sslClient.stop();
      sslClient.setBufferSizes(2048, 512);
      sslClient.setInsecure();
#endif
      int returnCode = sslClient.connect("podlewa.cz", 443);
      if (returnCode) {
        retryCounter = 0;
        dataFetchInProgress = true;
        connectionTimeoutMs = lastServerReadTime;
        SUPLA_LOG_DEBUG("# podlewa.cz - succesful connect");
        char buf[200];
        strcpy(buf, "GET /status/");

        strcat(buf, apiUrl);
        //strcat(buf, "/");

        Serial.print(F("# podlewa.cz - query: "));
        Serial.println(buf);

        strcat(buf, " HTTP/1.1");

        sslClient.println(buf);
        sslClient.println(F("Host: podlewa.cz"));
        sslClient.println(F("Connection: close"));
        sslClient.println();

      } else {  // if connection wasn't successful, try few times
        SUPLA_LOG_DEBUG("# failed to connect to podlewa.cz api, "
                                                "return code: %d", returnCode);
        retryCounter++;
		state = 0;
		setActionValue(2);
      }
    }
  }
*/
}

void Podlewacz::setActionValue(int value) {
  if (value != actionValue) {
    switch(value) {
      default:
	  case 0:
        runAction(Supla::ON_EVENT_1);
        break;
      case 1:
        runAction(Supla::ON_EVENT_2);
        break;
      case 2:
        runAction(Supla::ON_ERROR);
        break;
    }
    actionValue = value;
  }
}

bool Podlewacz::iterateConnected() {
  if (!dataFetchInProgress) {
    if (lastServerReadTime == 0 || millis() - lastServerReadTime > 
                             (retryCounter > 0 ? 5000 : refreshRateSec*1000)) {
      lastServerReadTime = millis();
#ifdef ARDUINO_ARCH_ESP32
      const char* root_ca = \
      "-----BEGIN CERTIFICATE-----\n" \
      "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\n" \
      "-----END CERTIFICATE-----\n";
      sslClient.setCACert(root_ca);
      sslClient.setHandshakeTimeout(120);
#endif
      //SUPLA_LOG_DEBUG("# podlewa.cz - connecting ...");
#ifdef ARDUINO_ARCH_ESP8266
      sslClient.stop();
      sslClient.setBufferSizes(2048, 512);
      sslClient.setInsecure();
#endif
      int returnCode = sslClient.connect("podlewa.cz", 443);
      if (returnCode) {
        retryCounter = 0;
        dataFetchInProgress = true;
        connectionTimeoutMs = lastServerReadTime;
        SUPLA_LOG_DEBUG("# podlewa.cz - succesful connect");
        char buf[200];
        strcpy(buf, "GET /status/");

        strcat(buf, apiUrl);
        //strcat(buf, "/");

        Serial.print(F("# podlewa.cz - query: "));
        Serial.println(buf);

        strcat(buf, " HTTP/1.1");

        sslClient.println(buf);
        sslClient.println(F("Host: podlewa.cz"));
        sslClient.println(F("Connection: close"));
        sslClient.println();

      } else {  // if connection wasn't successful, try few times
        SUPLA_LOG_DEBUG("# failed to connect to podlewa.cz api, "
                                                "return code: %d", returnCode);
        retryCounter++;
        state = 0;
        setActionValue(2);
      }
    }
  }
  return Element::iterateConnected();
}

void Podlewacz::setServerRefreshRate(unsigned int min) {
  refreshRateSec = min*60;
  if (refreshRateSec == 0) {
    refreshRateSec = DEFAULT_SERVER_REFRESH_RATE;
  }
  SUPLA_LOG_DEBUG("# podlewa.cz - saved refresh rate: %d min.", min);
}
#endif
