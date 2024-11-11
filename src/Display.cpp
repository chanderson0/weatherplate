#include "Display.h"

#include "Fonts/meteocons16pt7b.h"
#include "Fonts/meteocons20pt7b.h"
#include "Fonts/meteocons24pt7b.h"
#include "Fonts/meteocons48pt7b.h"

#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

const uint16_t kWidth = 800;
const uint16_t kHeight = 600;
const uint16_t kScreenPadding = 24;

const uint16_t kCol1Width = 275;

const UIRect kTimeRect = {kScreenPadding, kScreenPadding, kCol1Width, 70};
const UIRect kDateRect = {kScreenPadding, kTimeRect.y + kTimeRect.h, kCol1Width,
                          45};

const UIRect kMeteoconRect = {kScreenPadding, kDateRect.y + kDateRect.h + 30,
                              330, 100};
const UIRect kWeatherDescRect = {
    kScreenPadding, kMeteoconRect.y + kMeteoconRect.h + 30, 330, 100};

const char *kLoadingMessage = "Starting...";

void Display::drawDebug(const UIRect &rect)
{
  inkplate.drawRect(rect.x, rect.y, rect.w, rect.h, 1);
}

void Display::drawText(const char *text, int16_t x, int16_t y, uint16_t w,
                       uint16_t h, uint8_t alignX, uint8_t alignY)
{
  UIRect rect;

  // Calculate font baseline
  uint16_t fontHeight;
  inkplate.getTextBounds("A", 0, 0, &rect.x, &rect.y, &rect.w, &fontHeight);

  // Calculate target position
  int16_t cx = x;
  int16_t cy = y;

  // Move to alignment
  if (alignX == 1)
  {
    cx += w / 2;
  }
  else if (alignX == 2)
  {
    cx += w;
  }
  if (alignY == 1)
  {
    cy += h / 2;
  }
  else if (alignY == 2)
  {
    cy += h;
  }

  inkplate.getTextBounds(text, cx, cy, &rect.x, &rect.y, &rect.w, &rect.h);

  // Move to actual position
  if (alignX == 1)
  {
    rect.x = cx - rect.w / 2;
  }
  else if (alignX == 2)
  {
    rect.x = cx - rect.w;
  }
  if (alignY == 1)
  {
    rect.y = cy - rect.h / 2;
  }
  else if (alignY == 2)
  {
    rect.y = cy;
  }

  // Draw text, moving cursor down by the baseline
  inkplate.setCursor(rect.x, rect.y + fontHeight);
  inkplate.print(text);
}

void Display::drawText(const char *text, const UIRect &rect, uint8_t alignX,
                       uint8_t alignY)
{
  drawText(text, rect.x, rect.y, rect.w, rect.h, alignX, alignY);
}

char getMeteoconFromOWMIcon(const char *icon)
{
  if (icon == 0)
  {
    return ')';
  }

  bool isDay = icon[2] == 'd';

  if (strncmp(icon, "01", 2) == 0)
  {
    return isDay ? 'B' : 'C';
  }
  else if (strncmp(icon, "02", 2) == 0)
  {
    return isDay ? 'H' : 'I';
  }
  else if (strncmp(icon, "03", 2) == 0)
  {
    return 'N';
  }
  else if (strncmp(icon, "04", 2) == 0)
  {
    return 'Y';
  }
  else if (strncmp(icon, "09", 2) == 0)
  {
    return 'Q';
  }
  else if (strncmp(icon, "10", 2) == 0)
  {
    return 'R';
  }
  else if (strncmp(icon, "11", 2) == 0)
  {
    return 'P';
  }
  else if (strncmp(icon, "13", 2) == 0)
  {
    return 'W';
  }
  else if (strncmp(icon, "50", 2) == 0)
  {
    return isDay ? 'J' : 'K';
  }
  else
  {
    return ')';
  }
}

void Display::init()
{
  inkplate.begin();

  // Text defaults
  inkplate.setTextColor(BLACK, WHITE);

  // Draw loading message
  inkplate.clearDisplay();
  inkplate.setFont(&FreeSans24pt7b);
  drawText(kLoadingMessage, 0, 0, kWidth, kHeight, 1, 1);
  inkplate.display();
}

void Display::beginFrame()
{
  inkplate.clearDisplay();
  inkplate.setCursor(0, 0);
}

void Display::drawTime(struct tm *timeinfo)
{
  char timeBuf[64];

  strftime(timeBuf, sizeof(timeBuf), "%I:%M %p", timeinfo);
  inkplate.setFont(&FreeSans24pt7b);
  drawText(timeBuf, kTimeRect, 0, 1);

  strftime(timeBuf, sizeof(timeBuf), "%a %F", timeinfo);
  inkplate.setFont(&FreeSans18pt7b);
  drawText(timeBuf, kDateRect, 0, 1);
}

void Display::drawConnectionStatus(bool isConnected)
{
  inkplate.println(isConnected ? "Connected" : "Not connected");
}

void Display::drawWeather(JsonDocument &weatherDoc)
{
  char buf[64];

  const int16_t kMeteoconMargin = 108;
  const int16_t kMeteoconMagicHeight = 40;

  // Temperature
  const float temp = weatherDoc["current"]["temp"];
  sprintf(buf, "%.0f F", temp);
  inkplate.setFont(&FreeSans24pt7b);
  drawText(buf, kMeteoconRect.x + kMeteoconMargin, kMeteoconRect.y,
           kMeteoconRect.w - kMeteoconMargin, kMeteoconRect.h, 0, 1);

  // Icon
  const char *icon = weatherDoc["current"]["weather"][0]["icon"];
  const char meteocon = getMeteoconFromOWMIcon(icon);
  inkplate.setFont(&meteocons48pt7b);
  inkplate.drawChar(kMeteoconRect.x,
                    kMeteoconRect.y + kMeteoconRect.h / 2 +
                        kMeteoconMagicHeight,
                    meteocon, 1, 0, 1);

  // Description
  const char *desc = weatherDoc["current"]["weather"][0]["description"];
  if (desc == 0)
  {
    desc = "Unknown conditions";
  }
  inkplate.setFont(&FreeSans12pt7b);
  drawText(desc, kWeatherDescRect, 0, 0);

  // Graph
  float hourlyTemp[24];
  float precipProb[24];
  char labels[24][64];
  float minTemp = 1000;
  float maxTemp = -1000;
  struct tm timeinfo;
  for (uint8_t i = 0; i < 24; ++i)
  {
    time_t hourTime = weatherDoc["hourly"][i]["dt"];
    localtime_r(&hourTime, &timeinfo);
    if (timeinfo.tm_hour == 0 || timeinfo.tm_hour == 12 ||
        timeinfo.tm_hour == 6 || timeinfo.tm_hour == 18)
    {
      strftime(labels[i], sizeof(labels[i]), "%I%p", &timeinfo);
    }
    else
    {
      labels[i][0] = 0;
    }

    hourlyTemp[i] = weatherDoc["hourly"][i]["temp"];
    precipProb[i] = weatherDoc["hourly"][i]["pop"];

    maxTemp = max(maxTemp, hourlyTemp[i]);
    minTemp = min(minTemp, hourlyTemp[i]);
  }
  float range = maxTemp - minTemp;
  float yMin = minTemp - range * 0.1f;
  float yMax = maxTemp + range * 0.1f;
  UIRect rect = {kCol1Width + kScreenPadding + 30, kScreenPadding + 20, 425,
                 250};

  drawGraphAxes(rect);
  drawGraphYLabels(rect, minTemp, maxTemp);
  drawGraphXLabels(labels[0], 24, rect);
  drawGraphLine(hourlyTemp, 24, rect, yMin, yMax);
  drawDashedGraphLine(precipProb, 24, rect, 0, 1, 6, 2);

  // Days
  uint16_t dayWidth = (kWidth - 2 * kScreenPadding) / 7;
  uint16_t dayHeight = 180;
  char timeBuf[64];
  for (uint8_t i = 0; i < 7; ++i)
  {
    time_t dayStart = weatherDoc["daily"][i]["dt"];
    localtime_r(&dayStart, &timeinfo);
    strftime(timeBuf, sizeof(timeBuf), "%a", &timeinfo);

    const char *icon = weatherDoc["daily"][i]["weather"][0]["icon"];
    const char *desc = weatherDoc["daily"][i]["weather"][0]["main"];
    if (desc == 0)
    {
      desc = "?";
    }
    const float high = weatherDoc["daily"][i]["temp"]["max"];
    const float low = weatherDoc["daily"][i]["temp"]["min"];
    const float pop = weatherDoc["daily"][i]["pop"];

    UIRect dayRect = {kScreenPadding + dayWidth * i,
                      kHeight - kScreenPadding - dayHeight, dayWidth,
                      dayHeight};

    inkplate.setFont(&FreeSans18pt7b);
    drawText(timeBuf, dayRect, 0, 0);

    const char meteocon = getMeteoconFromOWMIcon(icon);
    inkplate.setFont(&meteocons24pt7b);
    inkplate.drawChar(dayRect.x, dayRect.y + 64, meteocon, 1, 1, 1);

    inkplate.setFont(&FreeSans12pt7b);
    drawText(desc, dayRect.x, dayRect.y + 80, dayRect.w, 0, 0, 2);

    sprintf(buf, "%.0fF high", high);
    drawText(buf, dayRect.x, dayRect.y + 110, dayRect.w, 0, 0, 2);
    sprintf(buf, "%.0fF low", low);
    drawText(buf, dayRect.x, dayRect.y + 135, dayRect.w, 0, 0, 2);

    sprintf(buf, "%.0f%% rain", pop * 100);
    drawText(buf, dayRect.x, dayRect.y + 165, dayRect.w, 0, 0, 2);
  }
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Display::drawGraphAxes(const UIRect &rect)
{
  inkplate.drawThickLine(rect.x, rect.y, rect.x, rect.y + rect.h, 1, 2);
  inkplate.drawThickLine(rect.x, rect.y + rect.h, rect.x + rect.w,
                         rect.y + rect.h, 1, 2);
}

void Display::drawGraphYLabels(const UIRect &rect, const float yMin,
                               const float yMax)
{
  char buf[64];

  inkplate.setFont(&FreeSans12pt7b);
  sprintf(buf, "%.0f", yMin);
  drawText(buf, rect.x - 8, rect.y + rect.h, 0, 0, 2, 0);
  sprintf(buf, "%.0f", yMax);
  drawText(buf, rect.x - 8, rect.y, 0, 0, 2, 2);
}

void Display::drawGraphXLabels(const char *xLabels, const uint16_t len,
                               const UIRect &rect)
{
  int16_t ptX;
  for (uint16_t i = 0; i < len; ++i)
  {
    ptX = (int16_t)map((float)i, 0, (float)len - 1, (float)rect.x,
                       (float)rect.x + (float)rect.w);

    const char *label = (xLabels + i * 64);
    if (label[0] != 0)
    {
      drawText(label, ptX, rect.y + rect.h + 5, 0, 0, 1, 2);
    }
  }
}

void Display::drawGraphLine(const float *values, const uint16_t len,
                            const UIRect &rect, const float yMin,
                            const float yMax)
{
  int16_t prevPtX = -1, prevPtY = -1;
  int16_t ptX, ptY;
  for (uint16_t i = 0; i < len; ++i)
  {
    ptX = (int16_t)map((float)i, 0, (float)len - 1, (float)rect.x,
                       (float)rect.x + (float)rect.w);
    ptY = (int16_t)map(values[i], yMin, yMax, (float)rect.y + (float)rect.h,
                       (float)rect.y);

    if (prevPtX >= 0)
    {
      inkplate.drawThickLine(prevPtX, prevPtY, ptX, ptY, 1, 2);
    }

    prevPtX = ptX;
    prevPtY = ptY;
  }
}

void Display::drawDashedGraphLine(const float *values, const uint16_t len,
                                  const UIRect &rect, const float yMin,
                                  const float yMax, uint8_t on, uint8_t off)
{
  int16_t prevPtX = -1, prevPtY = -1;
  int16_t ptX, ptY;
  for (uint16_t i = 0; i < len; ++i)
  {
    ptX = (int16_t)map((float)i, 0, (float)len - 1, (float)rect.x,
                       (float)rect.x + (float)rect.w);
    ptY = (int16_t)map(values[i], yMin, yMax, (float)rect.y + (float)rect.h,
                       (float)rect.y);

    if (prevPtX >= 0)
    {
      drawDashedLine(prevPtX, prevPtY, ptX, ptY, 1, on, off);
    }

    prevPtX = ptX;
    prevPtY = ptY;
  }
}

void Display::render(bool force)
{
  if (!force && nPartial < 10)
  {
    inkplate.partialUpdate();
    nPartial++;
  }
  else
  {
    inkplate.display();
    nPartial = 0;
  }
}

void Display::startDash(uint8_t on, uint8_t off)
{
  dashOnLength = on;
  totalDashLength = on + off;
  dashProgress = 0;
}

void Display::drawDashedPixel(int16_t x, int16_t y, uint16_t color)
{
  if (dashProgress < dashOnLength)
  {
    inkplate.drawPixel(x, y, color);
  }

  dashProgress++;

  if (dashProgress >= totalDashLength)
  {
    dashProgress = 0;
  }
}

void Display::drawDashedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                             uint16_t color, uint8_t on, uint8_t off)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep)
  {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1)
  {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx >> 1;
  int16_t ystep;

  if (y0 < y1)
    ystep = 1;
  else
    ystep = -1;

  startDash(on, off);
  for (; x0 <= x1; x0++)
  {
    if (steep)
      drawDashedPixel(y0, x0, color);
    else
      drawDashedPixel(x0, y0, color);

    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}
