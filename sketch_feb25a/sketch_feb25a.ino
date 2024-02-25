#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

//#define GRIDX 80
//#define GRIDY 60
//#define CELLXY 4

#define GRIDX 160
#define GRIDY 106
#define CELLXY 3

#define GEN_DELAY 0

//Current grid
uint8_t grid[GRIDX][GRIDY];

//The new grid for the next generation
uint8_t newgrid[GRIDX][GRIDY];

//Number of generations
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
//0, 1 = I, 2 = L, 3 = J, 4 = Z, 5 = S, 6 = o, 7 = T

int curBlock[0][0];



State state;

bool buttons[6];

void setup(){
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  state = Menu;

  for(int i = 2; i < 8; i++){
    pinMode(i, INPUT_PULLDOWN);
    buttons[i - 2] = false;
  }
}

void loop() {
  for(int i = 2; i < 8; i++){
    digitalRead(i)
  }




  switch(state){
    case State.MainMenu:

      break;
    case State.TetMenu:

      break;
    case State.TetGame:

      break;
    case State.TetGameOver:
      break;
  }
}


void startTetris() {
  level = 0;
  score = 0;

}