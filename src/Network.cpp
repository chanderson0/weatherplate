#include "Network.h"

void onWiFiConnect(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Connected to AP successfully!");
}

void onWiFiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.print("WiFi was disconnected: ");
  Serial.println(info.wifi_sta_disconnected.reason);
}

void Network::begin(const char *ssid, const char *password)
{
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  WiFi.onEvent(onWiFiConnect, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(onWiFiDisconnect, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  WiFi.begin(ssid, password);
}

void Network::ensureConnected()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.reconnect();
  }
}

bool Network::isConnected() { return WiFi.status() == WL_CONNECTED; }

void Network::fetchJson(const char *url, JsonDocument &target)
{
  HTTPClient http;

  http.useHTTP10(true);
  http.begin(url);
  http.GET();

  deserializeJson(target, http.getStream());

  http.end();
}
