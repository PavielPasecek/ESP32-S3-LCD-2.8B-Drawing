#include <PavelESP32_GFX.h>

// Vytvoření instance displeje (podobně jako v LovyanGFX)
PavelGFX lcd;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Jediný řádek inicializuje I2C, expandér, bzučák, podsvícení i LCD
  lcd.init();
  lcd.setBrightness(100);

  // Bílá obrazovka
  lcd.fillScreen(0xFFFF);
  delay(1000);

  // Červený čtverec
  lcd.fillRect(100, 100, 100, 100, 0xF800);
}

void loop() {
  // Náhodná pozice pro čtverec 10x10
  int x = random(0, 470);
  int y = random(0, 630);

  // Použití funkce color888 pro generování barev
  uint16_t randomColor = lcd.color888(random(256), random(256), random(256));

  // Vykreslení čtverce
  lcd.fillRect(x, y, 10, 10, randomColor);

  delay(5);
}
