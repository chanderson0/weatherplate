#include "Chrono.h"
#include "Display.h"
#include "Network.h"
#include <ArduinoJson.h>

#include "secrets.h"

const char *kWifiSSID = WIFI_SSID;
const char *kWifiPassword = WIFI_PASSWORD;

const char *kTzInfo = "EST5EDT";
const char *kNtpServer = "pool.ntp.org";

const char *kWeatherApi = "https://"
                          "api.openweathermap.org"
                          "/data/3.0/onecall"
                          "?lat=" LOC_LAT
                          "&lon=" LOC_LON
                          "&units=imperial"
                          "&exclude=alerts,minutely"
                          "&appid=" OPENWEATHER_APP_ID;

const int kTimeCheckIntervalMs = 100;
const int kWeatherRefreshIntervalMs = 10 * 60 * 1000;

Network network;

Inkplate inkplate(INKPLATE_1BIT);
Display display(inkplate);

// Weather!
StaticJsonDocument<64000> weatherDoc;

// Time!
time_t now;
struct tm timeinfo;
int lastMin = -1;

// Timers!
Chrono cCheckTime;
Chrono cScreenRefresh;
Chrono cNetworkConnection;
Chrono cFetchData;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting up!");

  display.init();
  network.begin(kWifiSSID, kWifiPassword);

  // Configure Time
  configTime(0, 0, kNtpServer);
  setenv("TZ", kTzInfo, 1);
  tzset();

  // Force the weather check to start right up
  cFetchData.add(kWeatherRefreshIntervalMs);
}

void loop()
{
  bool shouldRender = false;

  if (cCheckTime.hasPassed(kTimeCheckIntervalMs, true))
  {
    time(&now);
    localtime_r(&now, &timeinfo);

    if (lastMin != timeinfo.tm_min)
    {
      shouldRender = true;
      lastMin = timeinfo.tm_min;
    }
  }

  if (cFetchData.hasPassed(kWeatherRefreshIntervalMs))
  {
    if (network.isConnected())
    {
      Serial.println("Checking the weather");
      network.fetchJson(kWeatherApi, weatherDoc);
      shouldRender = true;
      cFetchData.restart();
    }
    else
    {
      // Force retry on connect
      cFetchData.restart(kWeatherRefreshIntervalMs);
    }
  }

  if (shouldRender)
  {
    display.beginFrame();
    display.drawTime(&timeinfo);
    // display.drawConnectionStatus(network.isConnected());
    display.drawWeather(weatherDoc);
    display.render();
  }

  if (cNetworkConnection.hasPassed(10000, true))
  {
    network.ensureConnected();
  }
}
