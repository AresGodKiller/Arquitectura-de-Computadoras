"""
launcher.py  —  Computadora A
─────────────────────────────────────────────────────────────────────────────
- Lee el UID de la tarjeta por Serial (ESP32 + PN532)
- Si la tarjeta NO está registrada → abre ventana de registro
- Si la tarjeta SÍ está registrada → abre el juego directo
- Al cerrar el juego, acumula el puntaje en la base de datos remota

Dependencias:
    pip install pyserial pymssql
─────────────────────────────────────────────────────────────────────────────
"""

import sys
import time
import serial
import serial.tools.list_ports
import pymssql
import tkinter as tk
from tkinter import font as tkfont

from snake_gui import SnakeApp


# ── CONFIGURACIÓN ─────────────────────────────────────────────────────────────

DB_SERVER   = '192.168.1.28'
DB_PORT     = 1433
DB_USER     = 'sa'
DB_PASSWORD = 'C0NTR453N1!4'
DB_NAME     = 'arcade_db'

SERIAL_PORT = 'COM10'
BAUD_RATE   = 115200


# ── Base de datos ─────────────────────────────────────────────────────────────

def listar_puertos():
    puertos = serial.tools.list_ports.comports()
    if puertos:
        print("Puertos COM disponibles:")
        for p in puertos:
            print(f"  {p.device}  —  {p.description}")
    else:
        print("  No se encontraron puertos COM.")
    print()


def conectar_db():
    return pymssql.connect(
        server=DB_SERVER,
        port=DB_PORT,
        user=DB_USER,
        password=DB_PASSWORD,
        database=DB_NAME,
        timeout=5
    )


def verificar_tarjeta(uid: str) -> dict | None:
    """Busca el UID. Devuelve datos del jugador o None si no existe."""
    try:
        conn   = conectar_db()
        cursor = conn.cursor(as_dict=True)
        cursor.execute(
            "SELECT * FROM puntuaciones WHERE ID_RFID = %s", (uid,)
        )
        fila = cursor.fetchone()
        conn.close()

        if fila is None:
            return None

        return {
            'id':        fila['ID'],
            'name_disp': fila['NAME_DISP'],
            'usr':       fila['USR'],
            'score':     fila['SCORE'],
            'id_rfid':   fila['ID_RFID']
        }

    except pymssql.OperationalError as e:
        print(f"  ERROR de conexión a la BD: {e}")
        print("  Verifica IP, puerto y credenciales en CONFIGURACIÓN.")
        return None
    except Exception as e:
        print(f"  ERROR inesperado: {e}")
        return None


def registrar_jugador(uid: str, name_disp: str, usr: str) -> dict | None:
    """Crea un registro nuevo en la BD y devuelve los datos del jugador."""
    try:
        conn   = conectar_db()
        cursor = conn.cursor(as_dict=True)
        cursor.execute(
            """
            INSERT INTO puntuaciones (NAME_DISP, USR, SCORE, ID_RFID)
            VALUES (%s, %s, %d, %s)
            """,
            (name_disp, usr, 0, uid)
        )
        conn.commit()

        # Recuperar el registro recién creado
        cursor.execute(
            "SELECT * FROM puntuaciones WHERE ID_RFID = %s", (uid,)
        )
        fila = cursor.fetchone()
        conn.close()

        print(f"  ✓ Jugador '{name_disp}' registrado exitosamente.")
        return {
            'id':        fila['ID'],
            'name_disp': fila['NAME_DISP'],
            'usr':       fila['USR'],
            'score':     fila['SCORE'],
            'id_rfid':   fila['ID_RFID']
        }

    except Exception as e:
        print(f"  ERROR al registrar jugador: {e}")
        return None


def actualizar_score(id_rfid: str, puntos: int) -> bool:
    try:
        conn   = conectar_db()
        cursor = conn.cursor()
        cursor.execute(
            """
            UPDATE puntuaciones
            SET    SCORE     = SCORE + %d,
                   LAST_GAME = GETDATE()
            WHERE  ID_RFID   = %s
            """,
            (puntos, id_rfid)
        )
        conn.commit()
        conn.close()
        print(f"  ✓ Puntuación actualizada (+{puntos} pts)")
        return True
    except Exception as e:
        print(f"  ERROR al actualizar puntuación: {e}")
        return False


# ── Ventana de registro ───────────────────────────────────────────────────────

class VentanaRegistro(tk.Tk):
    """Ventana gótica para registrar un jugador nuevo."""

    BG          = '#1C1A18'
    BG_INPUT    = '#17150F'
    CLR_GOLD    = '#C8A84B'
    CLR_GOLD_DIM= '#7A6030'
    CLR_PARCH   = '#D4C5A9'
    CLR_DIM     = '#5A5040'
    CLR_BORDER  = '#5C4A1E'
    CLR_RED     = '#8B1A1A'
    CLR_BTN_BG  = '#2A2318'
    CLR_BTN_FG  = '#C8A84B'
    CLR_BTN_BD  = '#5C4A1E'

    def __init__(self, uid: str):
        super().__init__()
        self.uid        = uid
        self.resultado  = None   # se llenará con dict del jugador si registra

        self.title('Nuevo Jugador — Arcade')
        self.resizable(False, False)
        self.configure(bg=self.BG)

        # Centrar ventana
        self.update_idletasks()
        w, h = 420, 380
        x = (self.winfo_screenwidth()  - w) // 2
        y = (self.winfo_screenheight() - h) // 2
        self.geometry(f'{w}x{h}+{x}+{y}')

        self._f_title  = tkfont.Font(family='Georgia', size=16, weight='bold')
        self._f_sub    = tkfont.Font(family='Georgia', size=9,  slant='italic')
        self._f_label  = tkfont.Font(family='Georgia', size=11)
        self._f_input  = tkfont.Font(family='Georgia', size=13)
        self._f_btn    = tkfont.Font(family='Georgia', size=11, weight='bold')
        self._f_err    = tkfont.Font(family='Georgia', size=9)
        self._f_uid    = tkfont.Font(family='Courier',  size=9)

        self._build_ui()
        self.protocol('WM_DELETE_WINDOW', self._cancelar)

    def _build_ui(self):
        # Borde dorado superior
        tk.Frame(self, bg=self.CLR_BORDER, height=2).pack(fill='x')

        # Cabecera
        hdr = tk.Frame(self, bg=self.BG, pady=18)
        hdr.pack(fill='x')
        tk.Label(hdr, text='✦  Nuevo Iniciado  ✦',
                 font=self._f_title,
                 bg=self.BG, fg=self.CLR_GOLD).pack()
        tk.Label(hdr, text='Tarjeta no reconocida — inscríbete para jugar',
                 font=self._f_sub,
                 bg=self.BG, fg=self.CLR_DIM).pack(pady=(2, 0))

        # UID de la tarjeta (solo informativo)
        tk.Label(hdr, text=f'UID: {self.uid}',
                 font=self._f_uid,
                 bg=self.BG, fg=self.CLR_DIM).pack(pady=(6, 0))

        # Divisor
        tk.Frame(self, bg=self.CLR_BORDER, height=1).pack(fill='x', padx=20)

        # Formulario
        form = tk.Frame(self, bg=self.BG, pady=20, padx=30)
        form.pack(fill='x')

        # Nombre para mostrar
        tk.Label(form, text='Nombre en pantalla',
                 font=self._f_label,
                 bg=self.BG, fg=self.CLR_PARCH,
                 anchor='w').pack(fill='x')
        tk.Label(form, text='Así aparecerá en el juego y el marcador',
                 font=self._f_sub,
                 bg=self.BG, fg=self.CLR_DIM,
                 anchor='w').pack(fill='x', pady=(0, 4))

        self._entry_name = tk.Entry(form,
                                    font=self._f_input,
                                    bg=self.BG_INPUT,
                                    fg=self.CLR_PARCH,
                                    insertbackground=self.CLR_GOLD,
                                    relief='flat',
                                    bd=0,
                                    highlightthickness=1,
                                    highlightbackground=self.CLR_BORDER,
                                    highlightcolor=self.CLR_GOLD)
        self._entry_name.pack(fill='x', ipady=7)
        self._entry_name.focus()

        # Usuario / alias
        tk.Label(form, text='Usuario (alias corto)',
                 font=self._f_label,
                 bg=self.BG, fg=self.CLR_PARCH,
                 anchor='w').pack(fill='x', pady=(16, 0))
        tk.Label(form, text='Sin espacios, máximo 20 caracteres',
                 font=self._f_sub,
                 bg=self.BG, fg=self.CLR_DIM,
                 anchor='w').pack(fill='x', pady=(0, 4))

        self._entry_usr = tk.Entry(form,
                                   font=self._f_input,
                                   bg=self.BG_INPUT,
                                   fg=self.CLR_PARCH,
                                   insertbackground=self.CLR_GOLD,
                                   relief='flat',
                                   bd=0,
                                   highlightthickness=1,
                                   highlightbackground=self.CLR_BORDER,
                                   highlightcolor=self.CLR_GOLD)
        self._entry_usr.pack(fill='x', ipady=7)

        # Mensaje de error
        self._lbl_err = tk.Label(form, text='',
                                  font=self._f_err,
                                  bg=self.BG, fg=self.CLR_RED)
        self._lbl_err.pack(pady=(8, 0))

        # Divisor
        tk.Frame(self, bg=self.CLR_BORDER, height=1).pack(fill='x', padx=20)

        # Botones
        bot = tk.Frame(self, bg=self.BG, pady=16)
        bot.pack()

        def _btn(parent, txt, cmd, primary=True):
            fg  = self.CLR_BTN_FG if primary else self.CLR_DIM
            b = tk.Button(parent, text=txt, command=cmd,
                          font=self._f_btn,
                          bg=self.CLR_BTN_BG, fg=fg,
                          activebackground='#3A3020',
                          activeforeground=self.CLR_GOLD,
                          relief='flat', bd=0,
                          highlightthickness=1,
                          highlightbackground=self.CLR_BTN_BD,
                          padx=18, pady=7, cursor='hand2')
            b.pack(side='left', padx=6)

        _btn(bot, 'Inscribirme y jugar', self._registrar)
        _btn(bot, 'Cancelar',            self._cancelar, primary=False)

        # Enter confirma
        self.bind('<Return>', lambda e: self._registrar())

        # Borde dorado inferior
        tk.Frame(self, bg=self.CLR_BORDER, height=2).pack(fill='x', side='bottom')

    def _registrar(self):
        name_disp = self._entry_name.get().strip()
        usr       = self._entry_usr.get().strip().replace(' ', '_')

        # Validaciones
        if not name_disp:
            self._lbl_err.config(text='El nombre en pantalla no puede estar vacío.')
            return
        if len(name_disp) > 50:
            self._lbl_err.config(text='Nombre demasiado largo (máx. 50 caracteres).')
            return
        if not usr:
            self._lbl_err.config(text='El usuario no puede estar vacío.')
            return
        if len(usr) > 20:
            self._lbl_err.config(text='Usuario demasiado largo (máx. 20 caracteres).')
            return

        self._lbl_err.config(text='Registrando...', fg=self.CLR_GOLD)
        self.update()

        jugador = registrar_jugador(self.uid, name_disp, usr)
        if jugador:
            self.resultado = jugador
            self.destroy()
        else:
            self._lbl_err.config(
                text='Error al conectar con la base de datos. Intenta de nuevo.',
                fg=self.CLR_RED
            )

    def _cancelar(self):
        self.resultado = None
        self.destroy()


# ── Serial ────────────────────────────────────────────────────────────────────

def esperar_tarjeta(ser: serial.Serial) -> str:
    print("Acerca tu tarjeta RFID al lector...")
    while True:
        try:
            linea = ser.readline().decode('utf-8', errors='ignore').strip()
        except serial.SerialException:
            print("ERROR: Se perdió la conexión con el ESP32.")
            sys.exit(1)
        if linea.startswith('UID:'):
            uid = linea[4:]
            print(f"  Tarjeta detectada: {uid}")
            return uid


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    print("=" * 52)
    print("    ARCADE RFID  —  Snake")
    print("=" * 52)

    listar_puertos()

    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Conectado al ESP32 en {SERIAL_PORT}\n")
    except serial.SerialException as e:
        print(f"ERROR abriendo {SERIAL_PORT}: {e}")
        sys.exit(1)

    time.sleep(2)
    ser.flushInput()

    while True:
        # 1. Esperar tarjeta
        uid = esperar_tarjeta(ser)

        # 2. Cerrar Serial antes de abrir cualquier ventana tkinter
        ser.close()

        # 3. Buscar en la BD
        print("  Consultando base de datos...")
        jugador = verificar_tarjeta(uid)

        if jugador is None:
            # ── Tarjeta desconocida → ventana de registro ──
            print("  Tarjeta no registrada. Abriendo ventana de registro...")
            ventana = VentanaRegistro(uid)
            ventana.mainloop()
            jugador = ventana.resultado

            if jugador is None:
                # El jugador canceló el registro
                print("  Registro cancelado. Esperando siguiente tarjeta.\n")
                try:
                    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
                    time.sleep(2)
                    ser.flushInput()
                except serial.SerialException as e:
                    print(f"ERROR reabriendo puerto Serial: {e}")
                    sys.exit(1)
                continue

        print(f"  ✓ Bienvenido, {jugador['name_disp']}!")
        print(f"    Puntaje acumulado: {jugador['score']} pts")

        # 4. Lanzar el juego
        print("\nAbriendo Snake...\n")
        app = SnakeApp(jugador=jugador)
        app.mainloop()

        # 5. Guardar puntos
        puntos_sesion = app.score
        print(f"\nSesión terminada. Puntos ganados: {puntos_sesion}")

        if puntos_sesion > 0:
            print("  Guardando en base de datos...")
            actualizar_score(jugador['id_rfid'], puntos_sesion)
        else:
            print("  Sin puntos nuevos, no se actualiza.")

        print("\n" + "=" * 52)
        print("  ¿Otro jugador? Acerca una tarjeta.")
        print("=" * 52 + "\n")

        # 6. Reabrir Serial
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            time.sleep(2)
            ser.flushInput()
        except serial.SerialException as e:
            print(f"ERROR reabriendo puerto Serial: {e}")
            sys.exit(1)


if __name__ == '__main__':
    main()
