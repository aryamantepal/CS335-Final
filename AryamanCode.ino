#include <Wire.h>
#include "Adafruit_TCS34725.h"

// ==================== CABIN DEFINITIONS ====================
enum Cabin {
  ARES,           // Red - War, aggression
  APHRODITE,      // Pink - Love, beauty
  POSEIDON,       // Blue - Sea, storms
  ATHENA,         // Gray/Blue - Wisdom, strategy
  APOLLO,         // Yellow/Gold - Sun, music, healing
  ARTEMIS,        // Silver/White - Moon, hunting
  DEMETER,        // Green - Agriculture, nature
  DIONYSUS,       // Purple - Wine, madness
  HADES,          // Black/Dark - Underworld, death
  HEPHAESTUS,     // Orange/Brown - Fire, forge
  HERMES,         // Orange - Travel, thieves (catch-all)
  ZEUS,           // White/Bright - Sky, lightning
  UNDECIDED
};

// ==================== BUTTON SETUP ====================
const int BUTTON_PIN = 2;  // Change this to your button pin
bool lastButtonState = HIGH;
bool scanning = false;
unsigned long scanStartTime = 0;
const unsigned long SCAN_DURATION = 10000;  // 10 seconds

// ==================== COLOR SENSOR SETUP ====================
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Variables for averaging readings
long totalR = 0, totalG = 0, totalB = 0;
int readingCount = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("   CAMP HALF-BLOOD SORTING SYSTEM");
  Serial.println("========================================\n");

  // Setup button pin with internal pullup
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (tcs.begin()) {
    Serial.println("[OK] Color sensor initialized");
  } else {
    Serial.println("[ERROR] TCS34725 not found!");
    Serial.println("Check I2C connections (SDA/SCL)");
    while (1);
  }

  Serial.println("\nReady to classify demigods...");
  Serial.println("Press the button to start scanning!\n");
  delay(1000);
}

Cabin classifyCabin(int r, int g, int b) {
  // Calculate color properties
  int total = r + g + b;
  if (total == 0) return UNDECIDED;
  
  float r_ratio = (float)r / total;
  float g_ratio = (float)g / total;
  float b_ratio = (float)b / total;
  
  int brightness = (r + g + b) / 3;
  
  // Determine dominant color
  int max_val = max(r, max(g, b));
  
  // ===== BLACK/DARK COLORS =====
  if (brightness < 60) {
    return HADES;  // Dark/Black → Hades
  }
  
  // ===== WHITE/BRIGHT COLORS =====
  if (brightness > 200 && abs(r - g) < 30 && abs(g - b) < 30 && abs(r - b) < 30) {
    return ZEUS;  // Bright white/light gray → Zeus
  }
  
  // ===== GRAY =====
  if (abs(r - g) < 25 && abs(g - b) < 25 && abs(r - b) < 25 && brightness >= 60 && brightness <= 200) {
    return ATHENA;  // Gray → Athena
  }
  
  // ===== RED DOMINANT =====
  if (max_val == r && r > g + 30 && r > b + 30) {
    if (b > 100 && r_ratio > 0.35) {
      return APHRODITE;  // Pink (red + blue) → Aphrodite
    }
    return ARES;  // Pure red → Ares
  }
  
  // ===== BLUE DOMINANT =====
  if (max_val == b && b > r + 30 && b > g + 30) {
    if (brightness < 100) {
      return POSEIDON;  // Dark blue → Poseidon
    }
    return ATHENA;  // Light blue → Athena
  }
  
  // ===== GREEN DOMINANT =====
  if (max_val == g && g > r + 30 && g > b + 30) {
    return DEMETER;  // Green → Demeter
  }
  
  // ===== YELLOW/GOLD =====
  if (r > 150 && g > 150 && b < 100) {
    return APOLLO;  // Yellow/Gold → Apollo
  }
  
  // ===== ORANGE =====
  if (r > g && g > b && r > 130 && g > 70 && b < 80) {
    if (r > 180 && g < 120) {
      return HEPHAESTUS;  // Orange-red → Hephaestus
    }
    return HERMES;  // Orange → Hermes
  }
  
  // ===== PURPLE =====
  if (r > 100 && b > 100 && g < r - 20 && g < b - 20) {
    return DIONYSUS;  // Purple → Dionysus
  }
  
  // ===== SILVER/LIGHT =====
  if (brightness > 150 && abs(r - b) < 40 && g < r && g < b) {
    return ARTEMIS;  // Silver/cool white → Artemis
  }
  
  // Default: Hermes (the catch-all cabin)
  return HERMES;
}

String getCabinName(Cabin cabin) {
  switch(cabin) {
    case ARES: return "ARES";
    case APHRODITE: return "APHRODITE";
    case POSEIDON: return "POSEIDON";
    case ATHENA: return "ATHENA";
    case APOLLO: return "APOLLO";
    case ARTEMIS: return "ARTEMIS";
    case DEMETER: return "DEMETER";
    case DIONYSUS: return "DIONYSUS";
    case HADES: return "HADES";
    case HEPHAESTUS: return "HEPHAESTUS";
    case HERMES: return "HERMES";
    case ZEUS: return "ZEUS";
    default: return "UNDECIDED";
  }
}

String getCabinDescription(Cabin cabin) {
  switch(cabin) {
    case ARES: return "God of War - Fierce and aggressive";
    case APHRODITE: return "Goddess of Love - Charming and beautiful";
    case POSEIDON: return "God of the Sea - Powerful and tempestuous";
    case ATHENA: return "Goddess of Wisdom - Strategic and intelligent";
    case APOLLO: return "God of the Sun - Creative and talented";
    case ARTEMIS: return "Goddess of the Hunt - Independent and precise";
    case DEMETER: return "Goddess of Agriculture - Nurturing and grounded";
    case DIONYSUS: return "God of Wine - Chaotic and free-spirited";
    case HADES: return "God of the Underworld - Dark and mysterious";
    case HEPHAESTUS: return "God of the Forge - Inventive and skilled";
    case HERMES: return "God of Travelers - Quick-witted and adaptable";
    case ZEUS: return "King of the Gods - Powerful and commanding";
    default: return "Unknown parentage";
  }
}

void loop() {
  // Read button state
  bool buttonState = digitalRead(BUTTON_PIN);
  
  // Check for button press (button goes LOW when pressed due to pullup)
  if (buttonState == LOW && lastButtonState == HIGH && !scanning) {
    // Button just pressed - start scanning
    scanning = true;
    scanStartTime = millis();
    totalR = 0;
    totalG = 0;
    totalB = 0;
    readingCount = 0;
    
    Serial.println("\n========================================");
    Serial.println("SCANNING... Please hold steady for 10 seconds");
    Serial.println("========================================");
    delay(200);  // Debounce delay
  }
  
  lastButtonState = buttonState;
  
  // If we're scanning, collect readings
  if (scanning) {
    unsigned long elapsed = millis() - scanStartTime;
    
    if (elapsed < SCAN_DURATION) {
      // Still scanning - take a reading
      float red, green, blue;
      
      tcs.setInterrupt(false);
      delay(60);
      tcs.getRGB(&red, &green, &blue);
      tcs.setInterrupt(true);

      int r = constrain((int)red, 0, 255);
      int g = constrain((int)green, 0, 255);
      int b = constrain((int)blue, 0, 255);

      // Accumulate values
      totalR += r;
      totalG += g;
      totalB += b;
      readingCount++;
      
      // Show progress every second
      if (readingCount % 20 == 0) {  // ~1 second intervals
        int secondsLeft = (SCAN_DURATION - elapsed) / 1000;
        Serial.print("Scanning... ");
        Serial.print(secondsLeft);
        Serial.println(" seconds remaining");
      }
      
      delay(50);  // Small delay between readings
      
    } else {
      // Scanning complete - calculate average and determine cabin
      scanning = false;
      
      int avgR = totalR / readingCount;
      int avgG = totalG / readingCount;
      int avgB = totalB / readingCount;
      
      Cabin detected = classifyCabin(avgR, avgG, avgB);
      
      Serial.println("\n========================================");
      Serial.println("        SORTING COMPLETE!");
      Serial.println("========================================");
      Serial.print("Average RGB: (");
      Serial.print(avgR); Serial.print(", ");
      Serial.print(avgG); Serial.print(", ");
      Serial.print(avgB); Serial.println(")");
      Serial.print("Readings taken: ");
      Serial.println(readingCount);
      Serial.println();
      Serial.print("★ YOUR CABIN: ");
      Serial.print(getCabinName(detected));
      Serial.println(" ★");
      Serial.println(getCabinDescription(detected));
      Serial.println("========================================\n");
      Serial.println("Press button to scan again...\n");
    }
  }
}
