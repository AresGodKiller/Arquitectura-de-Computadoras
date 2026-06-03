"""
snake_gui.py  —  Juego de Snake para Arcade RFID
Estética: gótico suave — piedra oscura, dorado envejecido, tipografía serif.
"""

import tkinter as tk
from tkinter import font as tkfont
import random
import math

COLS       = 22
ROWS       = 16
CELL       = 30
FPS_INIT   = 150
FPS_MIN    = 60
SPEED_STEP = 6
PTS_APPLE  = 10
PTS_SKULL  = 50


class SnakeApp(tk.Tk):
    # Paleta gótica suave — piedra, pergamino, dorado, carmesí
    BG_WIN      = '#1C1A18'   # carbón cálido
    BG_BOARD    = '#211F1C'   # piedra oscura
    BG_CELL_ALT = '#252320'   # damero sutil
    BG_HUD      = '#17150F'   # cuero oscuro
    BG_HEADER   = '#17150F'

    CLR_BORDER  = '#5C4A1E'   # dorado envejecido
    CLR_SNAKE_H = '#C8A84B'   # dorado cabeza
    CLR_SNAKE_B = '#9B7D2E'   # dorado cuerpo
    CLR_SNAKE_T = '#6B5520'   # dorado cola

    CLR_APPLE   = '#8B1A1A'   # carmesí oscuro
    CLR_APPLE_S = '#C0392B'   # brillo manzana
    CLR_SKULL   = '#D4C5A9'   # hueso

    CLR_GOLD    = '#C8A84B'
    CLR_GOLD_DIM= '#7A6030'
    CLR_PARCH   = '#D4C5A9'   # pergamino texto
    CLR_DIM     = '#5A5040'
    CLR_DEAD    = '#6B1A1A'
    CLR_MSG_OK  = '#C8A84B'
    CLR_MSG_DIE = '#8B1A1A'

    CLR_BTN_BG  = '#2A2318'
    CLR_BTN_FG  = '#C8A84B'
    CLR_BTN_BD  = '#5C4A1E'

    DIRS = {
        'Up':   (0, -1), 'w': (0, -1),
        'Down': (0,  1), 's': (0,  1),
        'Left': (-1, 0), 'a': (-1, 0),
        'Right':(1,  0), 'd': ( 1, 0),
    }

    def __init__(self, jugador=None):
        super().__init__()
        self.jugador       = jugador or {}
        self._name_disp    = self.jugador.get('name_disp', 'Invitado')
        self._score_previo = self.jugador.get('score', 0)
        self.score = 0

        self.title('Snake — Arcade')
        self.resizable(False, False)
        self.configure(bg=self.BG_WIN)
        self.protocol('WM_DELETE_WINDOW', self._on_close)

        # Fuentes serif — carácter gótico sin exagerar
        self._f_title  = tkfont.Font(family='Georgia', size=20, weight='bold')
        self._f_player = tkfont.Font(family='Georgia', size=10, slant='italic')
        self._f_score  = tkfont.Font(family='Georgia', size=30, weight='bold')
        self._f_label  = tkfont.Font(family='Georgia', size=9)
        self._f_lvl    = tkfont.Font(family='Georgia', size=22, weight='bold')
        self._f_msg    = tkfont.Font(family='Georgia', size=14, weight='bold')
        self._f_btn    = tkfont.Font(family='Georgia', size=10, weight='bold')
        self._f_sub    = tkfont.Font(family='Georgia', size=9, slant='italic')

        self._build_ui()
        self._bind_keys()
        self._init_game()

    def _build_ui(self):
        W = COLS * CELL

        # ── Borde dorado superior ──
        tk.Frame(self, bg=self.CLR_BORDER, height=2).pack(fill='x')

        # ── Header ──
        hdr = tk.Frame(self, bg=self.BG_HEADER, pady=10)
        hdr.pack(fill='x')

        left_hdr = tk.Frame(hdr, bg=self.BG_HEADER)
        left_hdr.pack(side='left', padx=16)
        tk.Label(left_hdr, text='✦ SNAKE ✦',
                 font=self._f_title,
                 bg=self.BG_HEADER, fg=self.CLR_GOLD).pack(anchor='w')
        tk.Label(left_hdr, text='Arcade Arcano',
                 font=self._f_sub,
                 bg=self.BG_HEADER, fg=self.CLR_GOLD_DIM).pack(anchor='w')

        right_hdr = tk.Frame(hdr, bg=self.BG_HEADER)
        right_hdr.pack(side='right', padx=16)
        tk.Label(right_hdr, text=self._name_disp,
                 font=self._f_player,
                 bg=self.BG_HEADER, fg=self.CLR_PARCH).pack(anchor='e')
        tk.Label(right_hdr,
                 text=f'Alma acumulada: {self._score_previo} pts',
                 font=self._f_label,
                 bg=self.BG_HEADER, fg=self.CLR_DIM).pack(anchor='e')

        # Divisor dorado
        tk.Frame(self, bg=self.CLR_BORDER, height=1).pack(fill='x')

        # ── HUD ──
        hud = tk.Frame(self, bg=self.BG_HUD, pady=10)
        hud.pack(fill='x')

        score_col = tk.Frame(hud, bg=self.BG_HUD)
        score_col.pack(side='left', padx=20)
        tk.Label(score_col, text='ALMAS',
                 font=self._f_label, bg=self.BG_HUD,
                 fg=self.CLR_DIM).pack(anchor='w')
        self._lbl_score = tk.Label(score_col, text='0',
                                   font=self._f_score,
                                   bg=self.BG_HUD, fg=self.CLR_GOLD)
        self._lbl_score.pack(anchor='w')

        lvl_col = tk.Frame(hud, bg=self.BG_HUD)
        lvl_col.pack(side='left', padx=16)
        tk.Label(lvl_col, text='CÍRCULO',
                 font=self._f_label, bg=self.BG_HUD,
                 fg=self.CLR_DIM).pack(anchor='w')
        self._lbl_level = tk.Label(lvl_col, text='I',
                                   font=self._f_lvl,
                                   bg=self.BG_HUD, fg=self.CLR_PARCH)
        self._lbl_level.pack(anchor='w')

        rec_col = tk.Frame(hud, bg=self.BG_HUD)
        rec_col.pack(side='right', padx=20)
        tk.Label(rec_col, text='RÉCORD',
                 font=self._f_label, bg=self.BG_HUD,
                 fg=self.CLR_DIM).pack(anchor='e')
        self._lbl_best = tk.Label(rec_col,
                                  text=str(self._score_previo),
                                  font=tkfont.Font(family='Georgia', size=22, weight='bold'),
                                  bg=self.BG_HUD, fg=self.CLR_GOLD_DIM)
        self._lbl_best.pack(anchor='e')

        # Divisor dorado
        tk.Frame(self, bg=self.CLR_BORDER, height=1).pack(fill='x')

        # ── Canvas del tablero ──
        board_frame = tk.Frame(self, bg=self.CLR_BORDER, padx=1, pady=1)
        board_frame.pack()
        self._canvas = tk.Canvas(board_frame,
                                 width=W, height=ROWS * CELL,
                                 bg=self.BG_BOARD, highlightthickness=0)
        self._canvas.pack()

        # Divisor dorado
        tk.Frame(self, bg=self.CLR_BORDER, height=1).pack(fill='x')

        # ── Barra inferior ──
        bot = tk.Frame(self, bg=self.BG_WIN, pady=8)
        bot.pack(fill='x', padx=14)

        self._lbl_msg = tk.Label(bot, text='',
                                 font=self._f_msg,
                                 bg=self.BG_WIN, fg=self.CLR_MSG_DIE)
        self._lbl_msg.pack(side='left')

        btns = tk.Frame(bot, bg=self.BG_WIN)
        btns.pack(side='right')

        def _btn(txt, cmd):
            b = tk.Button(btns, text=txt, command=cmd,
                          font=self._f_btn,
                          bg=self.CLR_BTN_BG, fg=self.CLR_BTN_FG,
                          activebackground='#3A3020',
                          activeforeground=self.CLR_GOLD,
                          relief='flat', bd=0,
                          highlightthickness=1,
                          highlightbackground=self.CLR_BTN_BD,
                          padx=12, pady=5, cursor='hand2')
            b.pack(side='left', padx=5)
        _btn('Nueva partida', self._init_game)
        _btn('Salir', self._on_close)

        tk.Label(self,
                 text='W A S D  ·  ↑ ← ↓ →  ·  R = reiniciar  ·  ESC = salir',
                 font=self._f_sub, bg=self.BG_WIN, fg=self.CLR_DIM
                 ).pack(pady=(2, 8))

        # Borde dorado inferior
        tk.Frame(self, bg=self.CLR_BORDER, height=2).pack(fill='x')

    # Números romanos para el nivel
    def _roman(self, n):
        vals = [(10,'X'),(9,'IX'),(5,'V'),(4,'IV'),(1,'I')]
        r = ''
        for v, s in vals:
            while n >= v:
                r += s; n -= v
        return r

    def _bind_keys(self):
        for key in ('Up','Down','Left','Right','w','a','s','d','W','A','S','D'):
            self.bind(f'<{key}>', self._on_key)
        self.bind('<r>', lambda e: self._init_game())
        self.bind('<R>', lambda e: self._init_game())
        self.bind('<Escape>', lambda e: self._on_close())

    def _init_game(self):
        cx, cy      = COLS // 2, ROWS // 2
        self._snake = [(cx, cy), (cx-1, cy), (cx-2, cy)]
        self._dir   = (1, 0)
        self._next  = (1, 0)
        self._apples= []
        self._tick  = FPS_INIT
        self._alive = True
        self._eaten = 0
        self._level = 1

        # FIX: resetear score de sesión solo si es nueva sesión del mismo jugador
        # El score acumulado de sesión NO se resetea con R — solo al cerrar y reabrir
        # Para el launcher lo que importa es self.score al cerrar la ventana.
        # Con R el jugador sigue jugando y acumula más puntos.
        # Si quieres reset completo comenta la siguiente línea:
        # self.score = 0

        self._lbl_score.config(text=str(self.score), fg=self.CLR_GOLD)
        self._lbl_level.config(text='I')
        self._lbl_msg.config(text='')

        self._spawn_apple()
        self._draw()

        if hasattr(self, '_after_id'):
            self.after_cancel(self._after_id)
        self._loop()

    def _spawn_apple(self):
        occupied = set(self._snake) | {a[0] for a in self._apples}
        free = [(x, y) for x in range(COLS) for y in range(ROWS)
                if (x, y) not in occupied]
        if not free:
            return
        pos  = random.choice(free)
        kind = 'skull' if random.random() < 0.18 else 'apple'
        self._apples.append((pos, kind))

    def _on_key(self, event):
        key = event.keysym.lower() if len(event.keysym) == 1 else event.keysym
        if key in self.DIRS:
            nd = self.DIRS[key]
            if nd[0] != -self._dir[0] or nd[1] != -self._dir[1]:
                self._next = nd

    def _loop(self):
        if self._alive:
            self._step()
            self._after_id = self.after(self._tick, self._loop)

    def _step(self):
        self._dir = self._next
        hx, hy   = self._snake[0]
        nx, ny   = hx + self._dir[0], hy + self._dir[1]

        if not (0 <= nx < COLS and 0 <= ny < ROWS):
            self._die(); return
        if (nx, ny) in self._snake[:-1]:
            self._die(); return

        self._snake.insert(0, (nx, ny))

        eaten = next((a for a in self._apples if a[0] == (nx, ny)), None)
        if eaten:
            self._apples.remove(eaten)
            pts = PTS_SKULL if eaten[1] == 'skull' else PTS_APPLE
            self.score  += pts
            self._eaten += 1
            self._tick   = max(FPS_MIN, self._tick - SPEED_STEP)
            self._level  = self._eaten // 5 + 1

            flash_color = self.CLR_SKULL if eaten[1] == 'skull' else '#D4A017'
            self._lbl_score.config(text=str(self.score), fg=flash_color)
            self.after(250, lambda: self._lbl_score.config(fg=self.CLR_GOLD))
            self._lbl_level.config(text=self._roman(self._level))
            self._spawn_apple()
        else:
            self._snake.pop()

        self._draw()

    def _die(self):
        self._alive = False
        # FIX bug game over: cancelar el loop explícitamente
        if hasattr(self, '_after_id'):
            self.after_cancel(self._after_id)
        self._lbl_msg.config(
            text=f'✝  {self.score} almas cosechadas  —  R para renacer',
            fg=self.CLR_MSG_DIE
        )
        self._draw(dead=True)

    def _draw(self, dead=False):
        c = self._canvas
        c.delete('all')

        # Damero de piedra sutil
        for x in range(COLS):
            for y in range(ROWS):
                if (x + y) % 2 == 0:
                    c.create_rectangle(
                        x*CELL, y*CELL, x*CELL+CELL, y*CELL+CELL,
                        fill=self.BG_CELL_ALT, outline=''
                    )

        # Comida
        for (ax, ay), kind in self._apples:
            cx_ = ax*CELL + CELL//2
            cy_ = ay*CELL + CELL//2
            if kind == 'skull':
                self._draw_skull(c, cx_, cy_)
            else:
                # Manzana carmesí con destello
                x1, y1 = ax*CELL+4, ay*CELL+5
                x2, y2 = ax*CELL+CELL-4, ay*CELL+CELL-3
                c.create_oval(x1, y1, x2, y2,
                              fill=self.CLR_APPLE, outline='#4A0000', width=1)
                c.create_oval(x1+3, y1+2, x1+8, y1+7,
                              fill='#C0392B', outline='', stipple='gray50')
                # tallito
                c.create_line(cx_, y1, cx_-3, y1-5,
                              fill='#3A5A20', width=2)

        # Serpiente
        n = len(self._snake)
        for i, (sx, sy) in enumerate(self._snake):
            x1, y1 = sx*CELL+2, sy*CELL+2
            x2, y2 = sx*CELL+CELL-2, sy*CELL+CELL-2

            if dead:
                fill = '#3A2A1A'
                out  = '#2A1A0A'
            elif i == 0:
                fill = self.CLR_SNAKE_H
                out  = '#A88030'
            elif i < n // 3:
                fill = self.CLR_SNAKE_B
                out  = '#7A5C20'
            else:
                fill = self.CLR_SNAKE_T
                out  = '#4A3A10'

            c.create_rectangle(x1, y1, x2, y2,
                               fill=fill, outline=out, width=1)

            # Escamas — línea diagonal sutil en cada segmento
            if not dead and i > 0:
                c.create_line(x1+4, y1+4, x2-4, y2-4,
                              fill=out, width=1)

            # Ojos en la cabeza
            if i == 0 and not dead:
                dx, dy = self._dir
                cx__ = sx*CELL + CELL//2
                cy__ = sy*CELL + CELL//2
                ox, oy = -dy, dx
                for sign in (1, -1):
                    ex = cx__ + ox*5*sign + dx*6
                    ey = cy__ + oy*5*sign + dy*6
                    c.create_oval(ex-3, ey-3, ex+3, ey+3,
                                  fill='#1A1008', outline='')
                    # pupila roja
                    c.create_oval(ex-1, ey-1, ex+1, ey+1,
                                  fill='#8B1A1A', outline='')

    def _draw_skull(self, c, cx, cy):
        r = 9
        # Cráneo
        c.create_oval(cx-r, cy-r, cx+r, cy+r//2,
                      fill=self.CLR_SKULL, outline='#9A8A74', width=1)
        # Mandíbula
        c.create_rectangle(cx-r+3, cy, cx+r-3, cy+r-2,
                            fill=self.CLR_SKULL, outline='#9A8A74', width=1)
        # Ojos
        for ox in (-4, 4):
            c.create_oval(cx+ox-3, cy-5, cx+ox+3, cy+1,
                          fill=self.BG_BOARD, outline='')
        # Dientes
        for tx in (-5, -1, 3):
            c.create_rectangle(cx+tx, cy+2, cx+tx+3, cy+r-3,
                               fill=self.BG_BOARD, outline='')

    def _on_close(self):
        if hasattr(self, '_after_id'):
            self.after_cancel(self._after_id)
        self.destroy()


if __name__ == '__main__':
    app = SnakeApp()
    app.mainloop()
