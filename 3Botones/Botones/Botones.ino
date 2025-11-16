/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://docs.arduino.cc/hardware/

  modified 8 May 2014
  by Scott Fitzgeralds
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/Blink/
*/

// the setup function runs once when you press reset or power the board
#define PIN_BUTTON_SOS 6
#define PIN_BUTTON_BLINK 7
#define PIN_BUTTON_PULSE 8
#define PIN_LED 9
#define DELAY_SLOW 100
#define DELAY_FAST 200

void setup() {
  pinMode(PIN_BUTTON_SOS, INPUT_PULLUP);
  pinMode(PIN_BUTTON_BLINK, INPUT_PULLUP);
  pinMode(PIN_BUTTON_PULSE, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
}

void loop() {
  if (digitalRead(PIN_BUTTON_SOS) == LOW) {
    writeBlink();
    return;
  }

  if (digitalRead(PIN_BUTTON_BLINK) == LOW) {
    writeBlink();
    return;
  }

  if (digitalRead(PIN_BUTTON_PULSE) == LOW) {
      writePulse();
      return;
  }
}

void writeSOS() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(DELAY_SLOW);
    digitalWrite(PIN_LED, LOW);
    delay(DELAY_SLOW);
  }
  
  delay(DELAY_SLOW);

  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(DELAY_FAST);
    digitalWrite(PIN_LED, LOW);
    delay(DELAY_FAST);
  }

  delay(DELAY_SLOW);

  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(DELAY_SLOW);
    digitalWrite(PIN_LED, LOW);
    delay(DELAY_SLOW);
  }

  digitalWrite(PIN_LED, 0);
}

void writeBlink() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(DELAY_FAST);
    digitalWrite(PIN_LED, LOW);
    delay(DELAY_FAST);
  }

  digitalWrite(PIN_LED, 0);
}

void writePulse() {
  float seconds = 1.5;
  float increment = 1.0 / seconds;

  for (float value = 0.0; value < 255.0; value += increment) {
    analogWrite(PIN_LED, (int)value);
    delay(1000 / 255);
  }

  for (float value = 255.0; value > 0.0; value -= increment) {
    analogWrite(PIN_LED, (int)value);
    delay(1000 / 255);
  }

  analogWrite(PIN_LED,0.0);
}