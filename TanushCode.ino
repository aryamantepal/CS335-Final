#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_HX8357.h>

// Extra color definitions (RGB565)
#define HX8357_NAVY      0x000F
#define HX8357_DARKGREEN 0x03E0
#define HX8357_ORANGE    0xFD20
#define HX8357_DARKGREY  0x7BEF
#define HX8357_MAROON    0x7800
#define HX8357_TEAL      0x0410
#define HX8357_LIGHTGREY 0xC618

#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8   // or -1 if you have RST tied to 3V3

Adafruit_HX8357 tft(TFT_CS, TFT_DC, TFT_RST);

// Screen size for rotation(1) on HX8357
const int16_t SCREEN_W = 480;
const int16_t SCREEN_H = 320;

// --- God “profiles” -------------------------------------------------

struct GodProfile {
  const char *name;
  const char *tagline;
  uint16_t bgColor;
  uint16_t accentColor;
};

enum GodId {
  ZEUS,
  POSEIDON,
  ATHENA,
  HADES,
  APOLLO,
  ARTEMIS,
  ARES,
  HERMES,
  NUM_GODS
};

GodProfile gods[NUM_GODS] = {
  // name      , tagline                                  , bg            , accent
  { "ZEUS"     , "Lord of the Skies"                      , HX8357_NAVY   , HX8357_YELLOW },
  { "POSEIDON" , "Master of Seas and Storms"              , HX8357_CYAN   , HX8357_BLUE   },
  { "ATHENA"   , "Goddess of Wisdom and Strategy"         , HX8357_DARKGREEN, HX8357_WHITE },
  { "HADES"    , "Ruler of the Underworld"                , HX8357_BLACK  , HX8357_RED    },
  { "APOLLO"   , "Archer of Light and Prophecy"           , HX8357_ORANGE , HX8357_YELLOW },
  { "ARTEMIS"  , "Huntress of the Moonlit Forest"         , HX8357_DARKGREY, HX8357_WHITE },
  { "ARES"     , "God of War and Chaos"                   , HX8357_MAROON , HX8357_ORANGE },
  { "HERMES"   , "Messenger, Trader, Professional Sneak"  , HX8357_TEAL   , HX8357_WHITE }
};

// --- Small helper: center text --------------------------------------

void drawCenteredText(const char *txt, int16_t y, uint8_t size, uint16_t color) {
  tft.setTextSize(size);
  tft.setTextColor(color);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds((char*)txt, 0, y, &x1, &y1, &w, &h);
  int16_t x = (SCREEN_W - (int16_t)w) / 2;
  tft.setCursor(x, y);
  tft.print(txt);
}

// --- Boot animation: Camp Half-Blood intro --------------------------

void drawBootAnimation() {
  tft.fillScreen(HX8357_BLACK);

  // Starry background
  for (int i = 0; i < 60; i++) {
    int x = random(SCREEN_W);
    int y = random(SCREEN_H / 2);
    tft.drawPixel(x, y, HX8357_WHITE);
  }

  drawCenteredText("Welcome to", 40, 2, HX8357_WHITE);
  drawCenteredText("CAMP HALF-BLOOD", 70, 3, HX8357_ORANGE);

  delay(800);

  // “Forging your destiny…” text
  drawCenteredText("Forging your destiny...", 110, 2, HX8357_YELLOW);
  delay(800);

  // Scanning bar animation
  int barY = 180;
  int barH = 20;
  int margin = 40;
  tft.drawRect(margin, barY, SCREEN_W - 2 * margin, barH, HX8357_WHITE);

  for (int pass = 0; pass < 3; pass++) {
    for (int x = margin + 2; x < SCREEN_W - margin - 2; x += 8) {
      tft.fillRect(margin + 1, barY + 1, SCREEN_W - 2 * margin - 2, barH - 2, HX8357_BLACK);
      tft.fillRect(x, barY + 2, 40, barH - 4, HX8357_ORANGE);
      delay(18);
    }
  }

  drawCenteredText("Consulting the Fates...", 220, 2, HX8357_LIGHTGREY);
  delay(1000);
}

// --- God-specific symbol drawing ------------------------------------

// Helper: simple lightning bolt for Zeus
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

// Helper: trident for Poseidon
void drawTrident(int16_t cx, int16_t cy, uint16_t color) {
  int h = 80;
  int shaftW = 8;
  tft.fillRect(cx - shaftW/2, cy - h/2, shaftW, h, color);

  // Three prongs
  for (int i = -1; i <= 1; i++) {
    int px = cx + i * 18;
    tft.fillRect(px - 3, cy - h/2 - 30, 6, 30, color);
    tft.fillTriangle(px - 8, cy - h/2 - 30, px + 8, cy - h/2 - 30, px, cy - h/2 - 50, color);
  }
}

// Helper: owl-ish symbol for Athena
void drawOwl(int16_t cx, int16_t cy, uint16_t color) {
  tft.fillRoundRect(cx - 35, cy - 40, 70, 80, 15, color);
  tft.fillCircle(cx - 15, cy - 10, 10, HX8357_BLACK);
  tft.fillCircle(cx + 15, cy - 10, 10, HX8357_BLACK);
  tft.fillCircle(cx - 15, cy - 10, 5, HX8357_WHITE);
  tft.fillCircle(cx + 15, cy - 10, 5, HX8357_WHITE);
  tft.fillTriangle(cx - 5, cy + 5, cx + 5, cy + 5, cx, cy + 15, HX8357_ORANGE);
}

// Helper: underworld flame for Hades
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

// Helper: sun for Apollo
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

// Helper: moon & arrow for Artemis
void drawMoonArrow(int16_t cx, int16_t cy, uint16_t color) {
  tft.fillCircle(cx - 10, cy, 25, color);
  tft.fillCircle(cx, cy, 25, HX8357_DARKGREY); // cut-out to make crescent
  // arrow
  tft.drawLine(cx + 20, cy + 25, cx + 60, cy - 15, color);
  tft.drawLine(cx + 60, cy - 15, cx + 50, cy - 20, color);
  tft.drawLine(cx + 60, cy - 15, cx + 55, cy - 5, color);
}

// Helper: shield for Ares
void drawShield(int16_t cx, int16_t cy, uint16_t color) {
  tft.fillCircle(cx, cy, 45, color);
  tft.drawCircle(cx, cy, 40, HX8357_BLACK);
  tft.drawLine(cx - 25, cy, cx + 25, cy, HX8357_BLACK);
  tft.drawLine(cx, cy - 25, cx, cy + 25, HX8357_BLACK);
}

// Helper: winged sandals for Hermes
void drawWings(int16_t cx, int16_t cy, uint16_t color) {
  for (int i = 0; i < 3; i++) {
    int dy = i * 12;
    tft.fillTriangle(cx - 40, cy + dy,
                     cx - 10, cy + dy - 10,
                     cx - 10, cy + dy + 10,
                     color);
  }
  tft.fillRect(cx - 5, cy - 10, 40, 20, color); // sandal
}

// --- Reveal a random god --------------------------------------------

void revealGod(GodId id) {
  GodProfile &g = gods[id];

  // Background wipe animation
  for (int y = 0; y < SCREEN_H; y += 8) {
    tft.fillRect(0, y, SCREEN_W, 8, g.bgColor);
    delay(5);
  }

  // Draw symbol in the middle
  int cx = SCREEN_W / 2;
  int cy = SCREEN_H / 2;

  switch (id) {
    case ZEUS:     drawLightning(cx, cy, g.accentColor); break;
    case POSEIDON: drawTrident(cx, cy, g.accentColor);   break;
    case ATHENA:   drawOwl(cx, cy, g.accentColor);       break;
    case HADES:    drawFlame(cx, cy, g.accentColor);     break;
    case APOLLO:   drawSun(cx, cy, g.accentColor);       break;
    case ARTEMIS:  drawMoonArrow(cx, cy, g.accentColor); break;
    case ARES:     drawShield(cx, cy, g.accentColor);    break;
    case HERMES:   drawWings(cx, cy, g.accentColor);     break;
  }

  // Text overlay
  drawCenteredText("Your immortal parent is...", 20, 2, HX8357_WHITE);
  drawCenteredText(g.name, 50, 4, g.accentColor);
  drawCenteredText(g.tagline, SCREEN_H - 40, 2, HX8357_WHITE);
}

// ===================================================================

void setup() {
  // Randomness for the god selection
  randomSeed(analogRead(A0));

  tft.begin(HX8357D);
  tft.setRotation(1);

  drawBootAnimation();

  // Pick a random god and reveal
  GodId chosen = (GodId) random(NUM_GODS);
  revealGod(chosen);
}

void loop() {
  // For now, do nothing in loop.
  // If you want a button to reshuffle, you can check it here and call revealGod again.
}
