README

Fecha:
1/12/2025

Integrantes:
+Eduardo Cadengo López - 23151204
+Itzel Citlalli Martell De La Cruz - 23151222

Placa utilizada:
Arduino UNO R3 (compatible)

Descripción del proyecto:
Este proyecto implementa un juego interactivo en Arduino donde el usuario debe encontrar la posición de una "mina" oculta entre 6 botones. Cada botón está asociado a un LED. El jugador tiene 3 intentos para encontrar la mina. Si falla, el juego se reinicia automáticamente. Además, existe un botón de interrupción que permite reiniciar el juego en cualquier momento.

Objetivo del código:
Generar un número aleatorio entre 0 y 5 para ocultar la mina.
Permitir al usuario seleccionar botones (A0–A5) para adivinar la posición.
Encender el LED correspondiente si el intento es incorrecto.
Mostrar mensajes en el monitor serial para informar el estado del juego.

Reiniciar el juego cuando:
Se encuentra la mina (secuencia de éxito).
Se agotan los intentos.
Se presiona el botón de interrupción.

Componentes necesarios:
Arduino UNO R3
6 LEDs (con resistencias de 220Ω)
6 botones para selección (con resistencias pull-up internas)
1 botón adicional para interrupción
Cables y protoboard

Conexiones:
Botones de selección: A0, A1, A2, A3, A4, A5 (INPUT_PULLUP)
LEDs:
LED1 Pin 5
LED2 Pin 6
LED3 Pin 7
LED4 Pin 8
LED5 Pin 9
LED6 Pin 10
Botón de interrupción Pin 2 (INPUT_PULLUP)

Lógica del programa-

Inicialización:
Configura pines de entrada y salida.
Genera posición aleatoria para la mina.
Muestra el estado inicial en el monitor serial.

Bucle principal (loop):
Verifica si se presionó el botón de interrupción.
Comprueba intentos restantes.
Lee el botón presionado y evalúa:

Si es la mina, muestra mensaje de éxito y ejecuta successSecuence().
Si no, enciende LED correspondiente, reduce intentos y muestra mensaje.

Funciones clave:
readButton(): Detecta qué botón fue presionado.
checkAttempts(): Valida el intento y actualiza el estado.
reset(): Reinicia el juego (mina aleatoria, intentos = 3).
resetSecuence(): Efecto visual al reiniciar.
successSecuence(): Animación cuando se gana.

Características especiales:
Uso de interrupción con attachInterrupt() para reiniciar el juego instantáneamente.
Uso de INPUT_PULLUP para simplificar el cableado de botones.
Mensajes en el monitor serial para depuración y feedback.

Cómo jugar:
Conecta el Arduino y carga el código.
Abre el monitor serial a 9600 baudios.
Presiona uno de los 6 botones para intentar encontrar la mina.
Si fallas, el LED correspondiente se encenderá y perderás un intento.
Si aciertas, se ejecutará una animación de éxito y el juego se reiniciará.
Puedes reiniciar el juego en cualquier momento con el botón de interrupción.

Video demostrativo:
https://youtu.be/Wesrx_gWeVU
