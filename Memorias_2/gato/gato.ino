#include <Keypad.h>
#include <LedControl.h>

// --------------------------------------
// Pin declaration
// --------------------------------------

// Led Matrix
int DIN = 12;
int CS = 11;
int CLK = 10;

// Button Matrix
const byte ROWS = 4;
const byte COLS = 4;

byte rowPins[ROWS] = { A2, A3, A4, A5 };
byte colPins[COLS] = { 2, 3, 4, 5 };


// Game status
// Base = 0 / Player = 1 / Comp = 2
char game[3][3] = {
  { '-', '-', '-' },
  { '-', '-', '-' },
  { '-', '-', '-' }
};


// Define the characters on the keypad
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};


// ---------------------------------
// Object creation 
// ---------------------------------

// Create the Keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//Create the board(Led Matrix) Object (?)
LedControl board(DIN, CLK, CS, 0);


// ---------------------------------
// Global variables
// ---------------------------------

int turn = 1;
char winner = 'N';

int randomRow;
int randomCol;


void setup() {
  Serial.begin(9600);

  //Led Matrix settings
  board.shutdown(0, false);
  board.setIntensity(0, 1);

 
}


void loop() {

  winner = checkWinner();

  if (winner == 'X') {
    xWins();
    resetGame();
    delay(500);
    return;
  }

  if (winner == 'O') {
    oWins();
    resetGame();
    delay(500);
    return;
  }

  if (turn > 10) {
    Serial.println("Empate");
    resetGame();
    delay(300);
    return;
  }

  if (turn % 2 != 0) {
    readKeypad();

  } else {
    compTurn();
  }
}

void printGame() {
  Serial.print("Turno:  ");
  Serial.println(turn);
  Serial.println();

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      Serial.print(game[i][j]);
      Serial.print("  ");
    }
    Serial.println();
  }

  Serial.println();
  Serial.println("==============");
  Serial.println();
}

void readKeypad() {
  char playerInput = customKeypad.getKey();

  if (playerInput == '1' && game[0][0] == '-') {
    game[0][0] = 'X';
    board.setLed(0, 0, 0, true);
    board.setLed(0, 1, 1, true);
    printGame();
    turn++;

  } else if (playerInput == '2' && game[0][1] == '-') {
    game[0][1] = 'X';
    board.setLed(0, 0, 3, true);
    board.setLed(0, 1, 4, true);
    printGame();
    turn++;

  } else if (playerInput == '3' && game[0][2] == '-') {
    game[0][2] = 'X';
    board.setLed(0, 0, 6, true);
    board.setLed(0, 1, 7, true);
    printGame();
    turn++;

  } else if (playerInput == '4' && game[1][0] == '-') {
    game[1][0] = 'X';
    board.setLed(0, 3, 0, true);
    board.setLed(0, 4, 1, true);
    printGame();
    turn++;

  } else if (playerInput == '5' && game[1][1] == '-') {
    game[1][1] = 'X';
    board.setLed(0, 3, 3, true);
    board.setLed(0, 4, 4, true);
    printGame();
    turn++;

  } else if (playerInput == '6' && game[1][2] == '-') {
    game[1][2] = 'X';
    board.setLed(0, 3, 6, true);
    board.setLed(0, 4, 7, true);
    printGame();
    turn++;

  } else if (playerInput == '7' && game[2][0] == '-') {
    game[2][0] = 'X';
    board.setLed(0, 6, 0, true);
    board.setLed(0, 7, 1, true);
    printGame();
    turn++;

  } else if (playerInput == '8' && game[2][1] == '-') {
    game[2][1] = 'X';
    board.setLed(0, 6, 3, true);
    board.setLed(0, 7, 4, true);
    printGame();
    turn++;

  } else if (playerInput == '9' && game[2][2] == '-') {
    game[2][2] = 'X';
    board.setLed(0, 6, 6, true);
    board.setLed(0, 7, 7, true);
    printGame();
    turn++;

  } else if (playerInput == 'D') {
    resetGame();
  }
}

void compTurn() {
  delay(200);
  randomSeed(micros());
  randomRow = random(0, 3);
  randomCol = random(0, 3);

  if (game[randomRow][randomCol] == '-') {
    game[randomRow][randomCol] = 'O';

    if (randomRow == 0 && randomCol == 0) {
      board.setLed(0, 0, 0, true);
      board.setLed(0, 0, 1, true);
      board.setLed(0, 1, 0, true);
      board.setLed(0, 1, 1, true);
    } else if (randomRow == 0 && randomCol == 1) {
      board.setLed(0, 0, 3, true);
      board.setLed(0, 0, 4, true);
      board.setLed(0, 1, 3, true);
      board.setLed(0, 1, 4, true);
    } else if (randomRow == 0 && randomCol == 2) {
      board.setLed(0, 0, 6, true);
      board.setLed(0, 0, 7, true);
      board.setLed(0, 1, 6, true);
      board.setLed(0, 1, 7, true);
    } else if (randomRow == 1 && randomCol == 0) {
      board.setLed(0, 3, 0, true);
      board.setLed(0, 3, 1, true);
      board.setLed(0, 4, 0, true);
      board.setLed(0, 4, 1, true);
    } else if (randomRow == 1 && randomCol == 1) {
      board.setLed(0, 3, 3, true);
      board.setLed(0, 3, 4, true);
      board.setLed(0, 4, 3, true);
      board.setLed(0, 4, 4, true);
    } else if (randomRow == 1 && randomCol == 2) {
      board.setLed(0, 3, 6, true);
      board.setLed(0, 3, 7, true);
      board.setLed(0, 4, 6, true);
      board.setLed(0, 4, 7, true);
    } else if (randomRow == 2 && randomCol == 0) {
      board.setLed(0, 6, 0, true);
      board.setLed(0, 6, 1, true);
      board.setLed(0, 7, 0, true);
      board.setLed(0, 7, 1, true);
    } else if (randomRow == 2 && randomCol == 1) {
      board.setLed(0, 6, 3, true);
      board.setLed(0, 6, 4, true);
      board.setLed(0, 7, 3, true);
      board.setLed(0, 7, 4, true);
    } else if (randomRow == 2 && randomCol == 2) {
      board.setLed(0, 6, 6, true);
      board.setLed(0, 6, 7, true);
      board.setLed(0, 7, 6, true);
      board.setLed(0, 7, 7, true);
    }

    printGame();
    turn++;
  }
}


char checkWinner() {

  //horizontal win
  for (int i = 0; i < 3; i++) {
    if (game[i][0] == game[i][1] && game[i][0] == game[i][2] && game[i][0] != '-') {
      return  game[i][0];
    }
  }

  //vertial wins
  for (int i = 0; i < 3; i++) {
    if (game[0][i] == game[1][i] && game[0][i] == game[2][i] && game[0][i] != '-') {
      return  game[0][i];
    }
  }

  //  D i a g o n   a l l e y
  if (game[0][0] == game[1][1] && game[0][0] == game[2][2] && game[0][0] != '-') {
    return  game[0][0];
  }
  if (game[0][2] == game[1][1] && game[0][2] == game[2][0] && game[0][2] != '-') {
    return  game[0][2];
  }

  return 'N';
}

void resetGame() {
  turn = 1;
  winner = 'N';
  
  board.clearDisplay(0);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      game[i][j] = '-';
    }
  }

  Serial.println("Un nuevo juego ha empezado");
}

void xWins() {
  board.clearDisplay(0);
  Serial.println("Ha ganado el jugador");
  for (int i = 0; i < 2; i++) {
    board.setRow(0, 0, B10000001);
    board.setRow(0, 1, B01000010);
    board.setRow(0, 2, B00100100);
    board.setRow(0, 3, B00011000);
    board.setRow(0, 4, B00011000);
    board.setRow(0, 5, B00100100);
    board.setRow(0, 6, B01000010);
    board.setRow(0, 7, B10000001);
    delay(300);
    board.clearDisplay(0);
    delay(300);
  }
  //un reset game aquí?
}

void oWins() {
  board.clearDisplay(0);
  Serial.println("Ha ganado la computadora");
  for (int i = 0; i < 2; i++) {
    board.setRow(0, 0, B00111100);
    board.setRow(0, 1, B01000010);
    board.setRow(0, 2, B10000001);
    board.setRow(0, 3, B10000001);
    board.setRow(0, 4, B10000001);
    board.setRow(0, 5, B10000001);
    board.setRow(0, 6, B01000010);
    board.setRow(0, 7, B00111100);
    delay(300);
    board.clearDisplay(0);
    delay(300);
  }
}