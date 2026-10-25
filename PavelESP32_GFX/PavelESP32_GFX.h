#pragma once
#include <Arduino.h>
#include <PavelESP32_I2C.h>
#include <PavelESP32_TCA.h>
#include <PavelESP32_Display.h>

class PavelGFX {
public:
  PavelGFX();
  
  // Inicializace celé desky (I2C, TCA, bzučák, podsvícení, LCD)
  void init();
  
  // Ovládání podsvícení (0 - 100)
  void setBrightness(uint8_t brightness);
  
  // Grafické příkazy ve stylu LovyanGFX
  void fillScreen(uint16_t color);
  void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
  
  // Pomocná funkce pro převod RGB888 na 16bitovou barvu (RGB565)
  uint16_t color888(uint8_t r, uint8_t g, uint8_t b);

  void buzzer(bool on);

  void drawPixel(int32_t x, int32_t y, uint16_t color);
  void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
  void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color);
  void drawCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color);
  void fillCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color);
};