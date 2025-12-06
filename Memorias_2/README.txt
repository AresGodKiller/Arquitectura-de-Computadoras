Proyecto: Juego de Gato (Tic-Tac-Toe) en Arduino

Fecha
06/12/2025

Integrantes:
+Eduardo Cadengo López - 23151204
+Itzel Citlalli Martell De La Cruz - 23151222

Placa utilizada
Arduino UNO R3 (compatible)

Descripción del proyecto
Este proyecto implementa el clásico juego de **gato (tic-tac-toe) en Arduino utilizando:
- Una matriz de botones (Keypad 4x4) para que el jugador seleccione posiciones.
- Una matriz de LEDs controlada con LedControl para mostrar las jugadas y animaciones de victoria.

El jugador compite contra la computadora. El sistema alterna turnos automáticamente y detecta condiciones de victoria o empate.

Objetivo del código
- Permitir al usuario seleccionar posiciones en el tablero mediante el Keypad.
- Mostrar las jugadas en la matriz de LEDs.
- Alternar turnos entre jugador (X) y computadora (O).
- Detectar victorias horizontales, verticales y diagonales.
- Reiniciar el juego en caso de victoria, empate o al presionar la tecla `D`.

Componentes necesarios
- Arduino UNO R3
- Matriz de botones 4x4 (Keypad)
- Matriz de LEDs controlada por MAX7219 (LedControl)
- Cables de conexión

Conexiones
- Matriz de LEDs (MAX7219):**
  - DIN → Pin 12
  - CS  → Pin 11
  - CLK → Pin 10

- Matriz de botones (Keypad 4x4):**
  - Filas → A2, A3, A4, A5
  - Columnas → 2, 3, 4, 5

Lógica del programa
1. Inicialización:
   - Configura la matriz de LEDs y el Keypad.
   - Limpia el tablero y muestra mensaje inicial en el monitor serial.

2. Bucle principal:
   - Verifica si hay un ganador (`X` o `O`).
   - Si hay empate, reinicia el juego.
   - Alterna turnos: jugador → computadora.

Funciones clave:
   - `readKeypad()`: Detecta la jugada del jugador.
   - `compTurn()`: Genera jugada aleatoria de la computadora.
   - `checkWinner()`: Evalúa condiciones de victoria.
   - `resetGame()`: Reinicia tablero y LEDs.
   - `xWins()` y `oWins()`: Animaciones de victoria en la matriz de LEDs.

Cómo jugar
1. Conecta el Arduino y carga el código.
2. Abre el monitor serial a 9600 baudios.
3. Usa el Keypad para seleccionar posiciones (1–9).
4. El tablero se muestra en la matriz de LEDs.
5. El sistema alterna turnos automáticamente.
6. Si alguien gana o hay empate, se reinicia el juego.
7. Puedes reiniciar manualmente con la tecla `D`.

Características especiales
- Uso de **Keypad** para simplificar el manejo de botones.
- Uso de **LedControl** para controlar la matriz de LEDs con animaciones.
- Mensajes en el monitor serial para depuración y feedback.


Video demostrativo:
https://youtu.be/Wesrx_gWeVU
