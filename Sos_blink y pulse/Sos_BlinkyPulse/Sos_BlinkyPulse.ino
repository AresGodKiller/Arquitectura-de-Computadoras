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
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/Blink/
*/

// the setup function runs once when you press reset or power the board
int ledPin = 9; // LED externo en pin PWM

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Primer SOS
  letraS();
  delay(1000);
  letraO();
  delay(1000);
  letraS();
  delay(3000);

  // Blink 7 parpadeos)
  blink();
  delay(3000);

  // Segundo SOS
  letraS();
  delay(1000);
  letraO();
  delay(1000);
  letraS();
  delay(3000);

  // Pulse 7 pulsos
  pulse();
  delay(3000);
}

void letraS() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200); // Corto
    digitalWrite(ledPin, LOW);
    delay(200); // Espacio entre puntos
  }
}

void letraO() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(800); // Largo
    digitalWrite(ledPin, LOW);
    delay(200); // Espacio entre rayas
  }
}

void blink() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100); // Parpadeo rápido
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}

void pulse() {
  for (int w = 0; w < 7; w++) {
    for (int i = 0; i <= 255; i++) {
      analogWrite(ledPin, i);
      delay(10);
    }

    for (int i = 255; i >= 0; i--) {
      analogWrite(ledPin, i);
      delay(10);
    }
  }
}


