#include "PavelESP32_GFX.h"

PavelGFX::PavelGFX() {
}

void PavelGFX::init() {
  // 1. Inicializace I2C sběrnice
  I2C_Init();
  
  // 2. Inicializace TCA expandéru
  TCA9554PWR_Init();

  // Vypnutí bzučáků
  Set_EXIO(EXIO_PIN8, Low);

  // 3. Inicializace podsvícení a samotného LCD panelu
  Backlight_Init();
  Set_Backlight(100);
  LCD_Init();
}
void PavelGFX::buzzer(bool on)
{
  Set_EXIO(EXIO_PIN8, on);
}
void PavelGFX::setBrightness(uint8_t brightness) {
  Set_Backlight(brightness);
}

uint16_t PavelGFX::color888(uint8_t r, uint8_t g, uint8_t b) {
  // Převod 24bit barvy na 16bit RGB565
  return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

void PavelGFX::fillScreen(uint16_t color) {
  // Alokace celého framebufferu v PSRAM
  uint16_t *screen = (uint16_t*)heap_caps_malloc(480 * 640 * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
  if (screen != NULL) {
    for (int i = 0; i < 480 * 640; i++) {
      screen[i] = color;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 480, 640, screen);
    // Uvolnění paměti po odeslání (pokud chceme šetřit PSRAM, ale pro fillScreen se často drží; zde bezpečně uvolníme nebo necháme)
    free(screen);
  }
}

void PavelGFX::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) {
  // Ošetření hranic obrazovky
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if (x + w > 480) w = 480 - x;
  if (y + h > 640) h = 640 - y;
  if (w <= 0 || h <= 0) return;

  size_t pixel_count = w * h;
  uint16_t *buf = (uint16_t*)heap_caps_malloc(pixel_count * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
  if (buf != NULL) {
    for (int i = 0; i < pixel_count; i++) {
      buf[i] = color;
    }
    // Vykreslení pomocí vašeho ověřeného rozhraní (x_end a y_end nejsou inkluzivní)
    esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + w, y + h, buf);
    free(buf);
  }
}

void PavelGFX::drawPixel(int32_t x, int32_t y, uint16_t color) {
  if (x < 0 || x >= 480 || y < 0 || y >= 640) return;
  uint16_t *buf = (uint16_t*)heap_caps_malloc(sizeof(uint16_t), MALLOC_CAP_SPIRAM);
  if (buf != NULL) {
    buf[0] = color;
    esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1, buf);
    free(buf);
  }
}

void PavelGFX::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) {
  fillRect(x, y, w, 1, color);         // Horní čára
  fillRect(x, y + h - 1, w, 1, color); // Dolní čára
  fillRect(x, y, 1, h, color);         // Levá čára
  fillRect(x + w - 1, y, 1, h, color); // Pravá čára
}

void PavelGFX::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) {
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;

  while (true) {
    drawPixel(x0, y0, color);
    if (x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void PavelGFX::drawCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color) {
  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;

  drawPixel(x0, y0 + r, color);
  drawPixel(x0, y0 - r, color);
  drawPixel(x0 + r, y0, color);
  drawPixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void PavelGFX::fillCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color) {
  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;

  fillRect(x0 - r, y0, 2 * r + 1, 1, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    fillRect(x0 - x, y0 + y, 2 * x + 1, 1, color);
    fillRect(x0 - x, y0 - y, 2 * x + 1, 1, color);
    fillRect(x0 - y, y0 + x, 2 * y + 1, 1, color);
    fillRect(x0 - y, y0 - x, 2 * y + 1, 1, color);
  }
}