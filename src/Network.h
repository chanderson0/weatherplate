#include "Arduino.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#ifndef NETWORK_H
#define NETWORK_H

class Network {
public:
  void begin(const char *ssid, const char *password);
  void ensureConnected();
  bool isConnected();

  void fetchJson(const char *url, JsonDocument &target);

private:
  unsigned long lastDisconnect;
};

#endif
