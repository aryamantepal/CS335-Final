#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_HX8357.h>

// ==================== TFT Setup ====================
#define HX8357_NAVY      0x000F
#define HX8357_DARKGREEN 0x03E0
#define HX8357_ORANGE    0xFD20
#define HX8357_DARKGREY  0x7BEF
#define HX8357_MAROON    0x7800
#define HX8357_TEAL      0x0410
#define HX8357_LIGHTGREY 0xC618

#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8

Adafruit_HX8357 tft(TFT_CS, TFT_DC, TFT_RST);

const int16_t SCREEN_W = 480;
const int16_t SCREEN_H = 320;

// ==================== GOD DATA ====================

struct GodProfile {
  const char *name;
  const char *tagline;
  uint16_t bgColor;
  uint16_t accentColor;
};

enum GodId {
  ZEUS, POSEIDON, ATHENA, HADES, APOLLO, ARTEMIS, ARES, HERMES, NUM_GODS
};

GodProfile gods[NUM_GODS] = {
  { "ZEUS",     "Lord of the Skies",              HX8357_NAVY,     HX8357_YELLOW },
  { "POSEIDON", "Master of Seas and Storms",      HX8357_CYAN,     HX8357_BLUE   },
  { "ATHENA",   "Goddess of Wisdom",              HX8357_DARKGREEN,HX8357_WHITE  },
  { "HADES",    "Ruler of the Underworld",        HX8357_BLACK,    HX8357_RED    },
  { "APOLLO",   "Archer of Light",                HX8357_ORANGE,   HX8357_YELLOW },
  { "ARTEMIS",  "Huntress of the Moon",           HX8357_DARKGREY, HX8357_WHITE  },
  { "ARES",     "God of War",                     HX8357_MAROON,   HX8357_ORANGE },
  { "HERMES",   "Messenger & Sneak",              HX8357_TEAL,     HX8357_WHITE  }
};

// ==================== TEXT HELPER ====================

void drawCenteredText(const char *txt, int16_t y, uint8_t size, uint16_t color) {
  tft.setTextSize(size);
  tft.setTextColor(color);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds((char*)txt, 0, y, &x1, &y1, &w, &h);
  tft.setCursor((SCREEN_W - w) / 2, y);
  tft.print(txt);
}

// ==================== SYMBOL DRAWING FUNCTIONS ====================

void drawLightning(int16_t cx, int16_t cy, uint16_t color) {
  int16_t x0 = cx - 20;
  int16_t y0 = cy - 40;
  int16_t x1 = cx + 10;
  int16_t y1 = cy - 10;
  int16_t x2 = cx - 5;
  int16_t y2 = cy + 10;
  int16_t x3 = cx + 20;
  int16_t y3 = cy + 40;

  tft.fillTriangle(x0, y0, x1, y0, x1, y1, color);
  tft.fillTriangle(x1, y1, x2, y1, x2, y2, color);
  tft.fillTriangle(x2, y2, x3, y2, x3, y3, color);
}

void drawTrident(int16_t cx, int16_t cy, uint16_t color) {
  int h = 80;
  int shaftW = 8;
  tft.fillRect(cx - shaftW/2, cy - h/2, shaftW, h, color);

  for (int i = -1; i <= 1; i++) {
    int px = cx + i * 18;
    tft.fillRect(px - 3, cy - h/2 - 30, 6, 30, color);
    tft.fillTriangle(px - 8, cy - h/2 - 30, px + 8, cy - h/2 - 30, px, cy - h/2 - 50, color);
  }
}

void drawOwl(int16_t cx, int16_t cy, uint16_t color) {
  tft.fillRoundRect(cx - 35, cy - 40, 70, 80, 15, color);
  tft.fillCircle(cx - 15, cy - 10, 10, HX8357_BLACK);
  tft.fillCircle(cx + 15, cy - 10, 10, HX8357_BLACK);
  tft.fillCircle(cx - 15, cy - 10, 5, HX8357_WHITE);
  tft.fillCircle(cx + 15, cy - 10, 5, HX8357_WHITE);
  tft.fillTriangle(cx - 5, cy + 5, cx + 5, cy + 5, cx, cy + 15, HX8357_ORANGE);
}

void drawFlame(int16_t cx, int16_t cy, uint16_t color) {
  tft.fillCircle(cx, cy + 20, 30, HX8357_DARKGREY);
  for (int i = 0; i < 3; i++) {
    int offset = i * 10 - 10;
    tft.fillTriangle(cx + offset - 10, cy + 10,
                     cx + offset + 10, cy + 10,
                     cx + offset,     cy - 30,
                     color);
  }
}

void drawSun(int16_t cx, int16_t cy, uint16_t color) {
  tft.fillCircle(cx, cy, 35, color);
  for (int i = 0; i < 12; i++) {
    float angle = i * 3.14159 / 6.0;
    int x1 = cx + cos(angle) * 40;
    int y1 = cy + sin(angle) * 40;
    int x2 = cx + cos(angle) * 60;
    int y2 = cy + sin(angle) * 60;
    tft.drawLine(x1, y1, x2, y2, color);
  }
}

void drawMoonArrow(int16_t cx, int16_t cy, uint16_t color) {
  tft.fillCircle(cx - 10, cy, 25, color);
  tft.fillCircle(cx, cy, 25, HX8357_DARKGREY);
  tft.drawLine(cx + 20, cy + 25, cx + 60, cy - 15, color);
  tft.drawLine(cx + 60, cy - 15, cx + 50, cy - 20, color);
  tft.drawLine(cx + 60, cy - 15, cx + 55, cy - 5, color);
}

void drawShield(int16_t cx, int16_t cy, uint16_t color) {
  tft.fillCircle(cx, cy, 45, color);
  tft.drawCircle(cx, cy, 40, HX8357_BLACK);
  tft.drawLine(cx - 25, cy, cx + 25, cy, HX8357_BLACK);
  tft.drawLine(cx, cy - 25, cx, cy + 25, HX8357_BLACK);
}

void drawWings(int16_t cx, int16_t cy, uint16_t color) {
  for (int i = 0; i < 3; i++) {
    int dy = i * 12;
    tft.fillTriangle(cx - 40, cy + dy,
                     cx - 10, cy + dy - 10,
                     cx - 10, cy + dy + 10,
                     color);
  }
  tft.fillRect(cx - 5, cy - 10, 40, 20, color);
}

// ==================== BOOT ANIMATION ====================

void drawBootAnimation() {
  tft.fillScreen(HX8357_BLACK);

  for (int i = 0; i < 60; i++) {
    int x = random(SCREEN_W);
    int y = random(SCREEN_H / 2);
    tft.drawPixel(x, y, HX8357_WHITE);
  }

  drawCenteredText("Welcome to", 40, 2, HX8357_WHITE);
  drawCenteredText("CAMP HALF-BLOOD", 70, 3, HX8357_ORANGE);
  delay(200);

  drawCenteredText("Forging your destiny...", 110, 2, HX8357_YELLOW);
  delay(300);

  int barY = 180;
  int barH = 20;
  int margin = 40;
  tft.drawRect(margin, barY, SCREEN_W - 2 * margin, barH, HX8357_WHITE);

  for (int x = margin + 2; x < SCREEN_W - margin - 40; x += 12) {
  tft.fillRect(x, barY + 2, 40, barH - 4, HX8357_ORANGE);
  delay(6);
  }


  drawCenteredText("Consulting the Fates...", 220, 2, HX8357_LIGHTGREY);
  delay(1000);
}

// ==================== REVEAL GOD ====================

void revealGod(GodId id) {
  GodProfile &g = gods[id];

  for (int y = 0; y < SCREEN_H; y += 8) {
    tft.fillRect(0, y, SCREEN_W, 8, g.bgColor);
    delay(5);
  }

  int cx = SCREEN_W / 2;
  int cy = SCREEN_H / 2;

  switch (id) {
    case ZEUS: drawLightning(cx, cy, g.accentColor); break;
    case POSEIDON: drawTrident(cx, cy, g.accentColor); break;
    case ATHENA: drawOwl(cx, cy, g.accentColor); break;
    case HADES: drawFlame(cx, cy, g.accentColor); break;
    case APOLLO: drawSun(cx, cy, g.accentColor); break;
    case ARTEMIS: drawMoonArrow(cx, cy, g.accentColor); break;
    case ARES: drawShield(cx, cy, g.accentColor); break;
    case HERMES: drawWings(cx, cy, g.accentColor); break;
  }

  drawCenteredText("Your immortal parent is...", 20, 2, HX8357_WHITE);
  drawCenteredText(g.name, 50, 4, g.accentColor);
  drawCenteredText(g.tagline, SCREEN_H - 40, 2, HX8357_WHITE);
  drawCenteredText("Press button to try again", SCREEN_H - 18, 1, HX8357_LIGHTGREY);
}

// ==================== I2C RECEIVE HANDLER ====================

char cabinBuffer[20];

GodId cabinNameToId(const char *name) {
  for (int i = 0; i < NUM_GODS; i++) {
    if (strcmp(name, gods[i].name) == 0) return (GodId)i;
  }
  return HERMES;  // fallback
}

void receiveEvent(int count) {
  int len = Wire.read();         // first byte = length
  int idx = 0;

  while (Wire.available() && idx < len) {
    cabinBuffer[idx++] = Wire.read();
  }
  cabinBuffer[idx] = '\0';

  Serial.print("Received cabin: ");
  Serial.println(cabinBuffer);

  GodId id = cabinNameToId(cabinBuffer);
  revealGod(id);
}


// ==================== SETUP ====================

void setup() {
  Serial.begin(9600);
  Wire.begin(0x04);
  Wire.onReceive(receiveEvent);

  tft.begin(HX8357D);
  tft.setRotation(1);
  drawBootAnimation();

  drawCenteredText("Press button on scanner...", 260, 2, HX8357_WHITE);
}

// ==================== LOOP ====================

void loop() {}
