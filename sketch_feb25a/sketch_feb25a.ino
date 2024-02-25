#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library

#include <LittleFileSystem.h>
#define FileSys LittleFileSystem
#include <PNGdec.h>
#define MAX_IMAGE_WIDTH 240

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library
PNG bhxLogoPng;
PNG rickyPng;

enum State {
  MainMenu,
  TetMenu,
  TetGame,
  TetGameOver
};

void * pngOpen(const char *filename, int32_t *size) {
  Serial.printf("Attempting to open %s\n", filename);
  pngfile = FileSys.open(filename, "r");
  *size = pngfile.size();
  return &pngfile;
}

void pngClose(void *handle) {
  File pngfile = *((File*)handle);
  if (pngfile) pngfile.close();
}

int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length) {
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.read(buffer, length);
}

int32_t pngSeek(PNGFILE *page, int32_t position) {
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.seek(position);
}

#define BHXLOGO_X 0
#define BHXLOGO_Y 0
#define RICKY_X 0
#define RICKY_Y 0

void bhxLogoDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  bhxLogoPng.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(BHXLOGO_X, BHXLOGO_Y + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

void rickyDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  rickyPng.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(RICKY_X, RICKY_Y + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

void goToMainMenu() {
  File bhxLogo = LittleFS.open("/bhxlogo.png");
  File ricky = LittleFS.open("/ricky.png");

  int16_t bhxLogoRC = bhxLogoPng.open(file.name().c_str(), pngOpen, pngClose, pngRead, pngSeek, bhxLogoDraw);
  int16_t rickyRC = rickyPng.open(file.name().c_str(), pngOpen, pngClose, pngRead, pngSeek, rickyDraw);

  tft.fillScreen(TFT_BLACK);

  delay(1000);

  if (bhxLogoRC == PNG_SUCCESS && rickyRC == PNG_SUCCESS) {
    tft.startWrite();

    if (bhxLogoPng.getWidth() > MAX_IMAGE_WIDTH) {
      Serial.println("bhx was too big");
    } else {
      bhxLogoRC = bhxLogoPng.decode(NULL, 0);
      bhxLogoPng.close();
    }
    if (rickyPng.getWidth() > MAX_IMAGE_WIDTH) {
      Serial.println("ricky was too big");
    } else {
      rickyRC = rickyPng.decode(NULL, 0);
      rickyPng.close();
    }

    tft.endWrite();
  }

  delay(5000);

  tft.fillScreen(TFT_BLACK);

  delay(1000);

  goToTetMenu();
}

uint16_t colors[] = {TFT_DARKGREY, TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_BLUE, TFT_CYAN, TFT_MAGENTA};

int level = 0;
int score = 0;
int blocks[20][10];
// 0, 1 = I, 2 = L, 3 = J, 4 = Z, 5 = S, 6 = o, 7 = T

int fallingBlock = 0;
int pos[2];
int fallingBlockCoords[4][2];
int rot;

State state;

bool buttons[6];

void setup() {
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_DARKGREY);
  state = MainMenu;

  if (!FileSys.begin()) {
    Serial.println("LittleFS initialisation failed!");
    while (1) yield();
  }

  for (int i = 2; i < 8; i++) {
    pinMode(i, INPUT_PULLDOWN);
    buttons[i - 2] = false;
  }

  goToMainMenu();
}

int f = 0;

void loop() {
  f++;

  for(int i = 2; i < 8; i++){
    buttons[i - 2] = digitalRead(i) == HIGH;
  }

  switch (state) {
    case 0:

      break;
    case 1: //TetMenu
      //Update
      if(buttons[2]){
        state = TetGame;
        goToTetGame();
      }
      break;
    case 2: //TetGame
      //Update
      if(fallingBlock == 0){
        fallingBlock = (rand()%7)+1;
        calcBlockCoords(fallingBlock, rot);
        pos[0] = 0;
        pos[1] = 4;
        rot = 0;
      }
      if(f % 4){ //fall
        if(!tryFall()){//lands
          for(int i = 0; i < 4; i++){
            blocks[pos[0] + fallingBlockCoords[i][1]][pos[1] + fallingBlockCoords[i][0]] = fallingBlock;
            //DIE???
          }
          fallingBlock = 0;
        }
      }


      if(buttons[3] || buttons[5]){
        tryMoveSide(buttons[3]);
      }

      if(buttons[0]){
        tryRotate();
      }

      //Draw

      for(int r = 0; r < 20; r++){
        for(int c = 0; c < 10; c++){
          tft.fillRect(80 + c * 16, 80 + r * 16, 16, 16, colors[blocks[r][c]]);
        }
      }

      for(int i = 0; i < 4; i++){
        int sy = pos[0] + fallingBlockCoords[i][1];
        int sx = pos[1] + fallingBlockCoords[i][0];

        tft.fillRect(80 + sx * 16, 80 + sy * 16, 16, 16, colors[fallingBlock]);
        tft.println(i + fallingBlockCoords[i][0]*10);
      }

      break;
    case 3:
      break;
  }
}

bool tryFall(){
  for(int i = 0; i < 4; i++){
    if((pos[0] + fallingBlockCoords[i][1] + 1) > 20 || blocks[pos[0] + fallingBlockCoords[i][1] + 1][pos[1] + fallingBlockCoords[i][0]] != 0){
      return false;
    }
  }

  pos[0]++;
  return true;
}

bool tryMoveSide(bool right){
  int dir = -1;
  if(right){
    dir = 1;
  }

  for(int i = 0; i < 4; i++){
    if((pos[1] + fallingBlockCoords[i][0] + dir) >= 10 || (pos[1] + fallingBlockCoords[i][0] + dir) < 0 || blocks[pos[0] + fallingBlockCoords[i][1] + 1][pos[1] + fallingBlockCoords[i][0] + dir] != 0){
      return false;
    }
  }

  pos[1]+=dir;
  return true;
}

bool tryRotate(){

}

void goToTetMenu() {

  tft.fillScreen(TFT_DARKGREY);
  tft.setCursor(30, 30);
  tft.setTextSize(3);
  tft.print("Press A to Begin");
  tft.setTextSize(2);
  state = TetMenu;
}

void goToTetGame() {

  tft.fillScreen(TFT_DARKGREY);
  //20x10 = 320x160 -> 80
  tft.drawRect(79, 79, 162, 322, TFT_WHITE);
  fallingBlock = 0;
  level = 0;
  score = 0;

  for(int r = 0; r < 20; r++){
    for(int c = 0; c < 10; c++){
      blocks[r][c] = 0;
      tft.fillRect(80 + c * 16, 80 + r * 16, 16, 16, colors[rand()%8]);
    }
  }
}

void calcBlockCoords(int block, int rot){// 0, 1 = I, 2 = L, 3 = J, 4 = Z, 5 = S, 6 = o, 7 = T
  switch(block){
    case 1: // |
      switch(rot){
        case 0:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = 0;
          fallingBlockCoords[1][0] = -1;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 1;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 2;
          fallingBlockCoords[3][1] = 0;
          break;
        case 1:
          fallingBlockCoords[0][0] = 1;
          fallingBlockCoords[0][1] = -2;
          fallingBlockCoords[1][0] = 1;
          fallingBlockCoords[1][1] = -1;
          fallingBlockCoords[2][0] = 1;
          fallingBlockCoords[2][1] = 1;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 2;
          break;
        case 2:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = 1;
          fallingBlockCoords[1][0] = -1;
          fallingBlockCoords[1][1] = 1;
          fallingBlockCoords[2][0] = 1;
          fallingBlockCoords[2][1] = 1;
          fallingBlockCoords[3][0] = 2;
          fallingBlockCoords[3][1] = 1;
          break;
        case 3:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = -2;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = -1;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 1;
          fallingBlockCoords[3][0] = 0;
          fallingBlockCoords[3][1] = 2;
          break;
      }
      break;
    case 2: // L
      switch(rot){
        case 0:
          fallingBlockCoords[0][0] = -1;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = -1;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 0;
          break;
        case 1:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = 1;
          fallingBlockCoords[1][1] = -1;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 0;
          fallingBlockCoords[3][1] = 1;
          break;
        case 2:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = 0;
          fallingBlockCoords[1][0] = -1;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 1;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 1;
          break;
        case 3:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = 0;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = -1;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 1;
          fallingBlockCoords[3][0] = -1;
          fallingBlockCoords[3][1] = 1;
          break;
      }
      break;
    case 3: // J
      switch(rot){
        case 0:
          fallingBlockCoords[0][0] = 1;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = -1;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 0;
          break;
        case 1:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = 1;
          fallingBlockCoords[1][1] = 1;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 0;
          fallingBlockCoords[3][1] = 1;
          break;
        case 2:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = 0;
          fallingBlockCoords[1][0] = -1;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 1;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = -1;
          fallingBlockCoords[3][1] = 1;
          break;
        case 3:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = 0;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = -1;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 1;
          fallingBlockCoords[3][0] = -1;
          fallingBlockCoords[3][1] = -1;
          break;
      }
      break;
    case 4: // Z
      switch(rot){
        case 0:
          fallingBlockCoords[0][0] = -1;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = -1;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 0;
          break;
        case 1:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 1;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 1;
          break;
        case 2:
          fallingBlockCoords[0][0] = -1;
          fallingBlockCoords[0][1] = 1;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = 1;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 0;
          break;
        case 3:
          fallingBlockCoords[0][0] = -1;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = -1;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 0;
          fallingBlockCoords[3][1] = 1;
          break;
      }
      break;
    case 5: // S
      switch(rot){
        case 0:
          fallingBlockCoords[0][0] = -1;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = -1;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 0;
          break;
        case 1:
          fallingBlockCoords[0][0] = 1;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = 1;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 0;
          fallingBlockCoords[3][1] = 1;
          break;
        case 2:
          fallingBlockCoords[0][0] = -1;
          fallingBlockCoords[0][1] = 0;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 1;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 1;
          break;
        case 3:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 1;
          fallingBlockCoords[2][1] = 0;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = -1;
          break;
      }
      break;
    case 6: // o
      fallingBlockCoords[0][0] = 0;
      fallingBlockCoords[0][1] = 0;
      fallingBlockCoords[1][0] = 0;
      fallingBlockCoords[1][1] = 1;
      fallingBlockCoords[2][0] = 1;
      fallingBlockCoords[2][1] = 1;
      fallingBlockCoords[3][0] = 1;
      fallingBlockCoords[3][1] = 0;
      break;
    case 7: // T
      switch(rot){
        case 0:
          fallingBlockCoords[0][0] = -1;
          fallingBlockCoords[0][1] = 0;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = -1;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 0;
          break;
        case 1:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 1;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 0;
          break;
        case 2:
          fallingBlockCoords[0][0] = -1;
          fallingBlockCoords[0][1] = 0;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 1;
          fallingBlockCoords[3][0] = 1;
          fallingBlockCoords[3][1] = 0;
          break;
        case 3:
          fallingBlockCoords[0][0] = 0;
          fallingBlockCoords[0][1] = -1;
          fallingBlockCoords[1][0] = 0;
          fallingBlockCoords[1][1] = 0;
          fallingBlockCoords[2][0] = 0;
          fallingBlockCoords[2][1] = 1;
          fallingBlockCoords[3][0] = -1;
          fallingBlockCoords[3][1] = 0;
          break;
      }
      break;
  }
}
