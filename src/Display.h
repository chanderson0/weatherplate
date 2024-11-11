#include "Arduino.h"
#include "Inkplate.h"
#include "time.h"
#include <ArduinoJson.h>

#ifndef DISPLAY_H
#define DISPLAY_H

typedef struct UIRect {
  int16_t x;
  int16_t y;
  uint16_t w;
  uint16_t h;
} UIRect;

class Display {
public:
  Display(Inkplate &inkplate) : inkplate(inkplate){};
  void init();

  void drawDebug(const UIRect &rect);
  void drawText(const char *text, int16_t x, int16_t y, uint16_t w = 0,
                uint16_t h = 0, uint8_t alignX = 0, uint8_t alignY = 0);
  void drawText(const char *text, const UIRect &rect, uint8_t alignX = 1,
                uint8_t alignY = 1);

  void drawGraph(const float *values, const char *xLabels, const uint16_t len,
                 const int16_t x, const int16_t y, const uint16_t w,
                 const uint16_t h, const float min, const float max);

  void drawTime(struct tm *timeinfo);
  void drawWeather(JsonDocument &weatherInfo);
  void drawConnectionStatus(bool isConnected);

  void beginFrame();
  void render(bool force = false);

  void drawGraphAxes(const UIRect &rect);
  void drawGraphYLabels(const UIRect &rect, const float yMin, const float yMax);
  void drawGraphXLabels(const char *xLabels, const uint16_t len,
                        const UIRect &rect);
  void drawGraphLine(const float *values, const uint16_t len,
                     const UIRect &rect, const float yMin, const float yMax);
  void drawDashedGraphLine(const float *values, const uint16_t len,
                           const UIRect &rect, const float yMin,
                           const float yMax, uint8_t on, uint8_t off);

  void startDash(uint8_t on, uint8_t off);
  void drawDashedPixel(int16_t x, int16_t y, uint16_t color);
  void drawDashedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                      uint16_t color, uint8_t on, uint8_t off);

private:
  Inkplate &inkplate;
  int nPartial = 0;

  uint8_t totalDashLength = 1;
  uint8_t dashOnLength = 1;
  uint8_t dashProgress = 0;
};

#endif
