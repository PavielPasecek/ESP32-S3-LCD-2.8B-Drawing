#include <PavelESP32_TCA.h>
#include <PavelESP32_Display.h>
#include <PavelESP32_I2C.h>


void setup()
{
    Serial.begin(115200);
    delay(1000);

    // 1. Inicializace I2C a expandéru
    I2C_Init();
    TCA9554PWR_Init();

    // --- VYPNUTÍ BZUČÁKU ---
    // Bzučák na deskách Waveshare bývá připojený na některý z volných EXIO pinů expandéru.
    // Nastavením Low na piny 4 až 8 bzučák zaručeně vypnete (pokud by pištěl dál, zkuste projít EXIO_PIN1 až 8).
    Set_EXIO(EXIO_PIN4, Low);
    Set_EXIO(EXIO_PIN5, Low);
    Set_EXIO(EXIO_PIN6, Low);
    Set_EXIO(EXIO_PIN7, Low);
    Set_EXIO(EXIO_PIN8, Low);

    // 2. Podsvícení a inicializace LCD
    Backlight_Init();
    Set_Backlight(100);

    LCD_Init();

    // --- Bílá obrazovka (celé rozlišení 480 x 640) ---
    uint16_t *screen = (uint16_t*)heap_caps_malloc(
        480 * 640 * sizeof(uint16_t),
        MALLOC_CAP_SPIRAM
    );

    if (screen != NULL) {
        for(int i = 0; i < 480 * 640; i++)
        {
            screen[i] = 0xFFFF; // Bílá barva v RGB565
        }

        esp_lcd_panel_draw_bitmap(
            panel_handle,
            0,
            0,
            480,
            640,
            screen
        );
    }

    delay(1000);

    // --- Červený čtverec (100 x 100 pixelů) ---
    uint16_t *red = (uint16_t*)heap_caps_malloc(
        100 * 100 * sizeof(uint16_t),
        MALLOC_CAP_SPIRAM
    );

    if (red != NULL) {
        for(int i = 0; i < 100 * 100; i++)
        {
            red[i] = 0xF800; // Červená barva v RGB565
        }

        // Vykreslení čtverce na pozici X: 100..200, Y: 100..200
        // Poznámka: x_end a y_end nejsou inkluzivní (proto 200 znamená vykreslit do 199)
        esp_lcd_panel_draw_bitmap(
            panel_handle,
            100,
            100,
            200,
            200,
            red
        );
    }
}

void loop()
{
    // Náhodná pozice pro čtverec 10x10 (musíme se vejít do rozměrů 480x640)
    int x = random(0, 480 - 10);
    int y = random(0, 640 - 10);

    // Vygenerování náhodné 16bitové barvy (RGB565)
    uint16_t randomColor = random(0x10000);

    // Malý buffer pro čtverec 10x10 pixelů (100 pixelů * 2 bajty = 200 bajtů)
    uint16_t square[10 * 10];
    for (int i = 0; i < 100; i++) {
        square[i] = randomColor;
    }

    // Vykreslení čtverce na vybranou pozici
    // x_end a y_end nejsou inkluzivní, proto přidáváme +10
    esp_lcd_panel_draw_bitmap(
        panel_handle,
         x,
         y,
         x + 10,
         y + 10,
         square
    );

    // Krátká prodleva (v milisekundách) – můžete upravit pro rychlost kreslení
    delay(5);
}
