# Weatherplate

Built for an [Inkplate 6](https://inkplate.readthedocs.io/en/latest/features.html#inkplate-6) using [Arduino API](https://inkplate.readthedocs.io/en/latest/arduino.html).

## Building

Ensure there is a `src/secrets.h` with the following:

```cpp
#define WIFI_SSID "<ssid>"
#define WIFI_PASSWORD "<password>"
#define OPENWEATHER_APP_ID "<api_key>"
#define LOC_LAT "<lat>"
#define LOC_LON "<lon>"
```

## Credits

- [Meteocons](https://demo.alessioatzeni.com/meteocons/)
