//array to store the location of the "mine"
int mineLocation[6];
int randomNumber;

//Pin declaration
int Led1 = 5;
int Led2 = 6;
int Led3 = 7;
int Led4 = 8;
int Led5 = 9;
int Led6 = 10;

int interruptionPin = 2;

int pressedButton = -1; // <- -1 = none

volatile bool resetFlag = false;

int attempts = 3; // amount of tries before reseting the game

void setup() {
  Serial.begin(9600);
  pinMode(interruptionPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptionPin), resetButton, FALLING);

  //Set "analog in" pins to read the buttons
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);

  //Set the "digital" pins to send the output of each led
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(Led4, OUTPUT);
  pinMode(Led5, OUTPUT);
  pinMode(Led6, OUTPUT);

  // Begin the first round of the game
  reset();
}

void loop() {
  //check if the interruption button was pressed
  //this will set the flag to true ^^^^^^^^^^^^
  if (resetFlag == true) {
    reset(); // <- And obviously rest the game
  }

  //Verify if you still have remaining attempts
  if (attempts > 0) {
    checkAttempts();
  } else {
    Serial.println("Se acabaron los intentos, Intente de nuevo");
    Serial.println();
    reset();
  }
}

//Method to read the buttons and set pressed button variable :O
int readButton() {
  int buttonState0 = digitalRead(A5);
  int buttonState1 = digitalRead(A4);
  int buttonState2 = digitalRead(A3);
  int buttonState3 = digitalRead(A2);
  int buttonState4 = digitalRead(A1);
  int buttonState5 = digitalRead(A0);

  if (buttonState0 == LOW) {
    pressedButton = 0;
    delay(500);
  } else if (buttonState1 == LOW) {
    pressedButton = 1;
    delay(500);
  } else if (buttonState2 == LOW) {
    pressedButton = 2;
    delay(500);
  } else if (buttonState3 == LOW) {
    pressedButton = 3;
    delay(500);
  } else if (buttonState4 == LOW) {
    pressedButton = 4;
    delay(500);
  } else if (buttonState5 == LOW) {
    pressedButton = 5;
    delay(500);
  }

  return pressedButton;
}

void checkAttempts() {
  // Check the button pressed and set it inside the switch 
  // These prints are merely for debugging and checking if the game worked before the leds 

  switch (readButton()) {
    case 0:
      Serial.println("El boton 0 fue presionado"); 
      if (mineLocation[pressedButton] != 0) {
        Serial.println("Felicidades"); 
        successSecuence();
        reset();
      } else {
        digitalWrite(Led1, HIGH);
        Serial.println("No está aquí  :(");
        attempts -= 1;
        Serial.print("Intentos: ");
        Serial.println(attempts);
        Serial.println();
      }
      break;
    case 1:
      Serial.println("El boton 1 fue presionado");
      if (mineLocation[pressedButton] != 0) {
        Serial.println("Felicidades");
        successSecuence();
        reset();
      } else {
        digitalWrite(Led2, HIGH);
        Serial.println("No está aquí  :(");
        attempts -= 1;
        Serial.print("Intentos: ");
        Serial.println(attempts);
        Serial.println();
      }
      break;
    case 2:
      Serial.println("El boton 2 fue presionado");
      if (mineLocation[pressedButton] != 0) {
        Serial.println("Felicidades");
        successSecuence();
        reset();
      } else {
        digitalWrite(Led3, HIGH);
        Serial.println("No está aquí  :(");
        attempts -= 1;
        Serial.print("Intentos: ");
        Serial.println(attempts);
        Serial.println();
      }
      break;
    case 3:
      Serial.println("El boton 3 fue presionado");
      if (mineLocation[pressedButton] != 0) {
        Serial.println("Felicidades");
        successSecuence();
        reset();
      } else {
        digitalWrite(Led4, HIGH);
        Serial.println("No está aquí  :(");
        attempts -= 1;
        Serial.print("Intentos: ");
        Serial.println(attempts);
        Serial.println();
      }
      break;
    case 4:
      Serial.println("El boton 4 fue presionado");
      if (mineLocation[pressedButton] != 0) {
        Serial.println("Felicidades");
        successSecuence();
        reset();
      } else {
        digitalWrite(Led5, HIGH);
        Serial.println("No está aquí  :(");
        attempts -= 1;
        Serial.print("Intentos: ");
        Serial.println(attempts);
        Serial.println();
      }
      break;
    case 5:
      Serial.println("El boton 5 fue presionado");
      if (mineLocation[pressedButton] != 0) {
        Serial.println("Felicidades");
        successSecuence();
        reset();
      } else {
        digitalWrite(Led6, HIGH);
        Serial.println("No está aquí  :(");
        attempts -= 1;
        Serial.print("Intentos: ");
        Serial.println(attempts);
        Serial.println();
      }
      break;
  }
  pressedButton = -1;
}

// Either after winning or failing the reset secuence must play 
// All roads lead to the resetSecuence
void resetSecuence() {
  // Encender todos los LEDs
  digitalWrite(Led1, HIGH);
  digitalWrite(Led2, HIGH);
  digitalWrite(Led3, HIGH);
  digitalWrite(Led4, HIGH);
  digitalWrite(Led5, HIGH);
  digitalWrite(Led6, HIGH);

  delay(500); // medio segundo encendidos

  // Apagar todos los LEDs
  digitalWrite(Led1, LOW);
  digitalWrite(Led2, LOW);
  digitalWrite(Led3, LOW);
  digitalWrite(Led4, LOW);
  digitalWrite(Led5, LOW);
  digitalWrite(Led6, LOW);
}



void successSecuence() {
  // Apagar todos los LEDs antes de empezar
  digitalWrite(Led1, LOW);
  digitalWrite(Led2, LOW);
  digitalWrite(Led3, LOW);
  digitalWrite(Led4, LOW);
  digitalWrite(Led5, LOW);
  digitalWrite(Led6, LOW);

  // Repetir el efecto varias veces
  for (int rep = 0; rep < 1; rep++) {
    
    for (int i = Led1; i <= Led6; i++) {
      digitalWrite(i, HIGH);
      delay(69);
      digitalWrite(i, LOW);
    }

    // Carrera derecha → izquierda
    for (int i = Led6; i >= Led1; i--) {
      digitalWrite(i, HIGH);
      delay(69);
      digitalWrite(i, LOW);
    }
  }
}


void resetButton() {
  resetFlag = true;
}

void reset() {
  //clear all values inside the array
  for (int i = 0; i < 6; i++) {
    mineLocation[i] = 0;
  }

  //Set a new random seed to generate a new spot for the mine
  randomSeed(micros());
  randomNumber = random(0, 6);

  //""Hide the mine"" on the newly generated position
  mineLocation[randomNumber] = 1;

  //Reset the amount of attempts remaining
  attempts = 3;

  //Indicate the user the game has been reseted
  Serial.println("Se ha reiniciado el juego");  

  //Show the array where the mine is located
  //This is mainly for debugging and kinda breaks the emotion of the game
  //So this can be commented to keep it secret.
  Serial.println("Array con el numero aleatorio:");
  for (int i = 0; i < 6; i++) {
    Serial.print(mineLocation[i]);
    Serial.print(",  ");
  }
  Serial.println();
  Serial.println();



  resetSecuence();
  resetFlag=false;
}
