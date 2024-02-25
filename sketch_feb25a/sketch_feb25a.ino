#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library


enum State {
  MainMenu,
  TetMenu,
  TetGame,
  TetGameOver
};

uint16_t colors[] = {TFT_DARKGREY, TFT_CYAN, TFT_BLUE, TFT_ORANGE, TFT_GREEN, TFT_RED, TFT_YELLOW, TFT_MAGENTA};

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
  state = TetMenu;


  for (int i = 2; i < 8; i++) {
    pinMode(i, INPUT_PULLDOWN);
    buttons[i - 2] = false;
  }

  goToTetMenu();
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
<<<<<<< Updated upstream

=======
>>>>>>> Stashed changes
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
