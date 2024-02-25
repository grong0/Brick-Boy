#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library

#define FileSys LittleFS

#include <PNGdec.h>

PNG png;
#define MAX_IMAGE_WIDTH 240;

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

// #define GRIDX 80
// #define GRIDY 60
// #define CELLXY 4

#define GRIDX 160
#define GRIDY 106
#define CELLXY 3

#define GEN_DELAY 0

// Current grid
uint8_t grid[GRIDX][GRIDY];

// The new grid for the next generation
uint8_t newgrid[GRIDX][GRIDY];

// Number of generations
#define NUMGEN 600

uint16_t genCount = 0;
enum State {
  MainMenu,
  TetMenu,
  TetGame,
  TetGameOver
};

int level = 0;
int score = 0;
int blocks[20][10];
// 0, 1 = I, 2 = L, 3 = J, 4 = Z, 5 = S, 6 = o, 7 = T

int curBlock[0][0];

State state;

bool buttons[6];

void createLogo() {
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
}

void createX(int16_t x, int16_t y, ) {
  tft.fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);
  tft.fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
  tft.fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
  tft.fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
  tft.fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
}

void setup() {
  if (!FileSys.begin()) {
    Serial.println("LittleFS initialization failed!");
    while (1) yield();  // Stay here waiting
  }

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  state = MainMenu;

  for (int i = 2; i < 8; i++) {
    pinMode(i, INPUT_PULLDOWN);
    buttons[i - 2] = false;
  }

  Serial.println("\r\nInitialisation done.");
}

void loop() {
  File root = LittleFS.open("/", "r");

  for (int i = 2; i < 8; i++) {
    buttons[i - 2] = digitalRead(i) == HIGH;
  }

  switch (state) {
    case 0:

      break;
    case 1:
      // Update
      if (buttons[0]) state = TetGame;
      // Draw
      tft.setTextSize(2);
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(40, 5);
      tft.println(F("Arduino"));
      break;
    case 2:
      // Upd
      break;
    case 3:
      break;
  }
}

void startTetris() {
  level = 0;
  score = 0;
}
