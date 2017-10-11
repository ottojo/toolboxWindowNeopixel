#include <Adafruit_NeoPixel.h>
#include <math.h>

#define FADETIME 1000

#define NEOPIXELPIN A5
#define PIXELCOUNT 12

#define toolboxOrange 255,120,0

enum {TOOLBOXSOLID, TOOLBOXFADE, TEST, TOOLBOXMOVING, TOOLBOXSIN, TOOLBOXMDAMAZING};

int ledMode = TOOLBOXMDAMAZING;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELCOUNT, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
extern const uint8_t gamma8[];
const float PROGMEM moveBrightness[] = { 1, 0.8, 0.5, 0.2};

int counter = 0;
float currentCenter = 0;
bool stateSin = true;

void setup() {
  Serial.begin(9600);
  // Init Neopixels
  strip.begin();
  strip.show();
}

void loop() {
  switch (ledMode) {
    case TEST:
      setAllPixels(adjustBrightness(0.5, gammaCorrect(toolboxOrange)));
      break;

    case TOOLBOXFADE:
      if (counter * 10 < FADETIME * 0.5) {       // Brightness rising
        float brightness = (10 * counter) / (float) (FADETIME * 0.5);
        Serial.println(brightness);
        setAllPixels(gammaCorrect(adjustBrightness(brightness, toolboxOrange)));
      } else if (counter * 10 < FADETIME) {      // Brightness falling
        float brightness = -1 * ((10 * counter) / (0.5f * FADETIME) - 2);
        Serial.println(brightness);
        setAllPixels(gammaCorrect(adjustBrightness(brightness, toolboxOrange)));
      } else {
        counter = -1;
      }
      break;

    case TOOLBOXSOLID:
      setAllPixels(gammaCorrect(toolboxOrange));
      break;

    case TOOLBOXMOVING:
      setAllPixels(0, 0, 0);

      currentCenter = fmod(millis() / 100, PIXELCOUNT);
      Serial.println(currentCenter);
      for ( int i = 0; i < PIXELCOUNT; i++) {
        strip.setPixelColor(i, gammaCorrect(adjustBrightness(brightnessAt(fminf(fabsf(currentCenter - i), PIXELCOUNT - fabsf(currentCenter - i))), toolboxOrange)));
      }
      break;

    case TOOLBOXSIN:
      setAllPixels(0, 0, 0);
      for ( int i = 0; i < PIXELCOUNT; i++) {
        
        float b = sinBrightnessAt(i, PIXELCOUNT / 3 , millis() / 300.0f);
        strip.setPixelColor(i, gammaCorrect(adjustBrightness(b, toolboxOrange)));
      }      
      break;

    case TOOLBOXMDAMAZING:
    setAllPixels(0, 0, 0);
      for ( int i = 0; i < PIXELCOUNT; i++) {
        float p = 1;
        if (stateSin) {
          p = (fmod(millis() / 3000.0f , 10) + 6);
        } else {
          p = 16 - (fmod(millis() / 3000.0f , 10) + 6);
        }

        Serial.println(p);

        float b = sinBrightnessAt(i, PIXELCOUNT / p , currentCenter);
        strip.setPixelColor(i, gammaCorrect(adjustBrightness(b, toolboxOrange)));
      }

      // if (currentCenter < PIXELCOUNT) {
      currentCenter += 0.1;
      /*} else {
        currentCenter = 0;
        }*/
      break;
      break;

    default:
      for (int i = 0; i < PIXELCOUNT; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }

      break;
  }
  counter++;
  strip.show();
}

// Gamma Correction (https://learn.adafruit.com/led-tricks-gamma-correction/the-quick-fix)
uint32_t gammaCorrect(uint8_t red, uint8_t green, uint8_t blue) {
  return strip.Color(pgm_read_byte(&gamma8[red]),
                     pgm_read_byte(&gamma8[green]),
                     pgm_read_byte(&gamma8[blue]));
}

uint32_t gammaCorrect(uint32_t color) {
  return gammaCorrect((byte) (color >> 16), (byte) (color >> 8), (byte) color);
}

uint32_t adjustBrightness(float brightness, uint8_t red, uint8_t green, uint8_t blue) {
  if (brightness <= 0) {
    return strip.Color(0, 0, 0);
  }
  return strip.Color((byte) (brightness * red), (byte) (brightness * green), (byte) (brightness * blue));
}

uint32_t adjustBrightness(float brightness, uint32_t color) {
  return adjustBrightness(brightness, (byte) (color >> 16), (byte) (color >> 8), (byte) color);
}

void setAllPixels(uint8_t red, uint8_t green, uint8_t blue) {
  for (int i = 0; i < PIXELCOUNT; i++) {
    strip.setPixelColor(i, red, green, blue);
  }
}

void setAllPixels(uint32_t color) {
  setAllPixels((byte) (color >> 16), (byte) (color >> 8), (byte) color);
}

float brightnessAt(float pos) {
  float m = 0.25;
  return max(0, 1 - pos * m);
}

float sinBrightnessAt(float pos, float period, float phaseShift) {
  return (sin(((2.0f * PI) / period) * pos - phaseShift) + 1) * 0.5f;
}

const uint8_t PROGMEM gamma8[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};
