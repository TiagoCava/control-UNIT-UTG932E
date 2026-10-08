import tkinter as tk
from tkinter import ttk, messagebox
import pyvisa
import math
import random

# --- PALETA DE COLORES ---
BG_PANEL = "#d3d3d3"
BG_SCREEN = "#000000"
FG_CH1 = "#ffff00"       # Amarillo para CH1
FG_CH2 = "#00ffff"       # Cian para CH2
FG_TEXT = "#00ff00"
FG_LABEL = "#ffffff"
FONT_SCREEN = ("Consolas", 12, "bold")
FONT_TITLE = ("Arial", 16, "bold")

class GeneradorDualHMI:
    def __init__(self, root):
        self.root = root
        self.root.title("Panel de Control Dual - UTG962E")
        self.root.geometry("1100x480") # Ventana más ancha para alojar ambos canales
        self.root.configure(bg=BG_PANEL)
        
        self.rm = pyvisa.ResourceManager()
        self.instrument = None
        
        self.crear_interfaz()

    def crear_interfaz(self):
        # --- TOP: ZONA DE CONEXIÓN ---
        frame_top = tk.Frame(self.root, bg=BG_PANEL)
        frame_top.pack(fill="x", padx=10, pady=10)
        
        tk.Label(frame_top, text="Dirección VISA:", bg=BG_PANEL).pack(side="left")
        self.entry_address = ttk.Entry(frame_top, width=30)
        self.entry_address.insert(0, "?") 
        self.entry_address.pack(side="left", padx=5)
        
        ttk.Button(frame_top, text="Conectar", command=self.conectar_equipo).pack(side="left")
        self.lbl_estado = tk.Label(frame_top, text=" Desconectado ", bg="red", fg="white", font=("Arial", 10, "bold"))
        self.lbl_estado.pack(side="right", padx=10)

        # --- CENTRO: PANTALLA Y BOTONES ---
        frame_center = tk.Frame(self.root, bg=BG_PANEL)
        frame_center.pack(fill="both", expand=True, padx=10, pady=5)

        # --- IZQUIERDA: PANTALLA DIGITAL ---
        self.pantalla = tk.Frame(frame_center, bg=BG_SCREEN, bd=4, relief="sunken")
        self.pantalla.pack(side="left", fill="both", expand=True)

        # === CANAL 1 (Columnas 0, 1, 2) ===
        tk.Label(self.pantalla, text="CH1", bg=BG_SCREEN, fg=FG_CH1, font=FONT_TITLE).grid(row=0, column=0, sticky="w", padx=10, pady=10)
        self.lbl_limit_1 = tk.Label(self.pantalla, text="Limit: HighZ", bg=BG_SCREEN, fg="red", font=("Arial", 10))
        self.lbl_limit_1.grid(row=0, column=1, sticky="w")

        self.var_onda_1, self.var_freq_1, self.var_funit_1, self.var_amp_1, self.var_off_1, self.var_duty_1, self.var_load_1 = self.crear_vars()
        self.lbl_duty_1, self.entry_duty_1, self.lbl_dunit_1 = self.construir_columna(1, 0, self.var_onda_1, self.var_freq_1, self.var_funit_1, self.var_amp_1, self.var_off_1, self.var_duty_1, self.var_load_1)

        # === CANAL 2 (Columnas 3, 4, 5) ===
        tk.Label(self.pantalla, text="CH2", bg=BG_SCREEN, fg=FG_CH2, font=FONT_TITLE).grid(row=0, column=3, sticky="w", padx=30, pady=10)
        self.lbl_limit_2 = tk.Label(self.pantalla, text="Limit: HighZ", bg=BG_SCREEN, fg="red", font=("Arial", 10))
        self.lbl_limit_2.grid(row=0, column=4, sticky="w")

        self.var_onda_2, self.var_freq_2, self.var_funit_2, self.var_amp_2, self.var_off_2, self.var_duty_2, self.var_load_2 = self.crear_vars()
        self.lbl_duty_2, self.entry_duty_2, self.lbl_dunit_2 = self.construir_columna(2, 3, self.var_onda_2, self.var_freq_2, self.var_funit_2, self.var_amp_2, self.var_off_2, self.var_duty_2, self.var_load_2, pad_x_title=30)

        # === RASTREO DE EVENTOS ===
        self.var_onda_1.trace_add("write", lambda *a: self.actualizar_ui_canal(1))
        self.var_duty_1.trace_add("write", lambda *a: self.dibujar_ondas())
        self.var_load_1.trace_add("write", lambda *a: self.actualizar_carga(1))
        
        self.var_onda_2.trace_add("write", lambda *a: self.actualizar_ui_canal(2))
        self.var_duty_2.trace_add("write", lambda *a: self.dibujar_ondas())
        self.var_load_2.trace_add("write", lambda *a: self.actualizar_carga(2))

        # --- GRÁFICA DE ONDAS (Columna 6) ---
        self.canvas = tk.Canvas(self.pantalla, width=280, height=180, bg=BG_SCREEN, highlightthickness=1, highlightbackground="#333333")
        self.canvas.grid(row=1, column=6, rowspan=6, padx=20, pady=10)
        self.dibujar_ondas()

        # --- DERECHA: BOTONES FÍSICOS ---
        frame_botones = tk.Frame(frame_center, bg=BG_PANEL)
        frame_botones.pack(side="right", fill="y", padx=20, pady=20)

        style = ttk.Style()
        style.configure("Big.TButton", font=("Arial", 11, "bold"), padding=10)

        ttk.Button(frame_botones, text="⭳ Leer Equipo", style="Big.TButton", command=self.leer_parametros).pack(fill="x", pady=5)
        ttk.Button(frame_botones, text="⭱ Escribir Cambios", style="Big.TButton", command=self.escribir_parametros).pack(fill="x", pady=5)
        
        tk.Frame(frame_botones, height=2, bg="gray").pack(fill="x", pady=15)

        self.btn_salida_1 = tk.Button(frame_botones, text="CH1 OFF", font=("Arial", 12, "bold"), bg="gray", fg="white", command=lambda: self.toggle_salida(1), height=2)
        self.btn_salida_1.pack(fill="x", pady=5)
        self.estado_salida_1 = False

        self.btn_salida_2 = tk.Button(frame_botones, text="CH2 OFF", font=("Arial", 12, "bold"), bg="gray", fg="white", command=lambda: self.toggle_salida(2), height=2)
        self.btn_salida_2.pack(fill="x", pady=5)
        self.estado_salida_2 = False

        self.actualizar_ui_canal(1)
        self.actualizar_ui_canal(2)

    # --- CONSTRUCTORES DE INTERFAZ ---
    def crear_vars(self):
        return (tk.StringVar(value="SIN"), tk.StringVar(value="1"), tk.StringVar(value="kHz"), 
                tk.StringVar(value="1.0"), tk.StringVar(value="0.0"), tk.StringVar(value="50.0"), tk.StringVar(value="HighZ"))

    def construir_columna(self, ch, c_base, v_onda, v_freq, v_funit, v_amp, v_off, v_duty, v_load, pad_x_title=10):
        # 1. Onda
        tk.Label(self.pantalla, text="Wave", bg=BG_SCREEN, fg=FG_LABEL, font=FONT_SCREEN, width=6, anchor="w").grid(row=1, column=c_base, padx=(pad_x_title, 0), pady=5)
        ttk.Combobox(self.pantalla, textvariable=v_onda, values=["SIN", "SQU", "PULS", "RAMP", "ARB", "NOIS", "DC"], state="readonly", width=8, font=FONT_SCREEN).grid(row=1, column=c_base+1, pady=5)
        
        # 2. Frecuencia
        tk.Label(self.pantalla, text="Freq", bg=BG_SCREEN, fg=FG_LABEL, font=FONT_SCREEN, width=6, anchor="w").grid(row=2, column=c_base, padx=(pad_x_title, 0), pady=5)
        tk.Entry(self.pantalla, textvariable=v_freq, bg=BG_SCREEN, fg=FG_TEXT, font=FONT_SCREEN, width=8, insertbackground="white", bd=0, highlightthickness=1).grid(row=2, column=c_base+1, pady=5)
        ttk.Combobox(self.pantalla, textvariable=v_funit, values=["mHz", "Hz", "kHz", "MHz"], state="readonly", width=4, font=FONT_SCREEN).grid(row=2, column=c_base+2, padx=5, pady=5)

        # 3. Amplitud
        tk.Label(self.pantalla, text="Amp", bg=BG_SCREEN, fg=FG_LABEL, font=FONT_SCREEN, width=6, anchor="w").grid(row=3, column=c_base, padx=(pad_x_title, 0), pady=5)
        tk.Entry(self.pantalla, textvariable=v_amp, bg=BG_SCREEN, fg=FG_TEXT, font=FONT_SCREEN, width=8, insertbackground="white", bd=0, highlightthickness=1).grid(row=3, column=c_base+1, pady=5)
        tk.Label(self.pantalla, text="Vpp", bg=BG_SCREEN, fg=FG_LABEL, font=FONT_SCREEN, anchor="w").grid(row=3, column=c_base+2, padx=5, pady=5)

        # 4. Offset
        tk.Label(self.pantalla, text="Offset", bg=BG_SCREEN, fg=FG_LABEL, font=FONT_SCREEN, width=6, anchor="w").grid(row=4, column=c_base, padx=(pad_x_title, 0), pady=5)
        tk.Entry(self.pantalla, textvariable=v_off, bg=BG_SCREEN, fg=FG_TEXT, font=FONT_SCREEN, width=8, insertbackground="white", bd=0, highlightthickness=1).grid(row=4, column=c_base+1, pady=5)
        tk.Label(self.pantalla, text="V", bg=BG_SCREEN, fg=FG_LABEL, font=FONT_SCREEN, anchor="w").grid(row=4, column=c_base+2, padx=5, pady=5)

        # 5. Duty Cycle
        lbl_duty = tk.Label(self.pantalla, text="Duty", bg=BG_SCREEN, fg=FG_LABEL, font=FONT_SCREEN, width=6, anchor="w")
        lbl_duty.grid(row=5, column=c_base, padx=(pad_x_title, 0), pady=5)
        entry_duty = tk.Entry(self.pantalla, textvariable=v_duty, bg=BG_SCREEN, fg=FG_TEXT, font=FONT_SCREEN, width=8, insertbackground="white", bd=0, highlightthickness=1)
        entry_duty.grid(row=5, column=c_base+1, pady=5)
        lbl_dunit = tk.Label(self.pantalla, text="%", bg=BG_SCREEN, fg=FG_LABEL, font=FONT_SCREEN, anchor="w")
        lbl_dunit.grid(row=5, column=c_base+2, padx=5, pady=5)

        # 6. Load (Impedancia)
        tk.Label(self.pantalla, text="Load", bg=BG_SCREEN, fg=FG_LABEL, font=FONT_SCREEN, width=6, anchor="w").grid(row=6, column=c_base, padx=(pad_x_title, 0), pady=5)
        ttk.Combobox(self.pantalla, textvariable=v_load, values=["HighZ", "50", "75"], width=8, font=FONT_SCREEN).grid(row=6, column=c_base+1, pady=5)
        tk.Label(self.pantalla, text="Ω", bg=BG_SCREEN, fg=FG_LABEL, font=FONT_SCREEN, anchor="w").grid(row=6, column=c_base+2, padx=5, pady=5)

        return lbl_duty, entry_duty, lbl_dunit

    # --- LÓGICA DINÁMICA DE LA INTERFAZ ---
    def actualizar_ui_canal(self, ch):
        tipo = self.var_onda_1.get() if ch == 1 else self.var_onda_2.get()
        l_d, e_d, u_d = (self.lbl_duty_1, self.entry_duty_1, self.lbl_dunit_1) if ch == 1 else (self.lbl_duty_2, self.entry_duty_2, self.lbl_dunit_2)
        
        if tipo in ["SQU", "PULS"]:
            l_d.grid(); e_d.grid(); u_d.grid()
        else:
            l_d.grid_remove(); e_d.grid_remove(); u_d.grid_remove()
        self.dibujar_ondas()

    def actualizar_carga(self, ch):
        val = self.var_load_1.get() if ch == 1 else self.var_load_2.get()
        lbl = self.lbl_limit_1 if ch == 1 else self.lbl_limit_2
        if val.upper() == "HIGHZ":
            lbl.config(text="Limit: HighZ")
        else:
            lbl.config(text=f"Limit: {val} Ω")

    def generar_puntos_onda(self, tipo, duty_str, w, cy, amp, fase_offset=0):
        puntos = []
        if tipo == "SIN":
            for x in range(w):
                puntos.extend([x, cy - amp * math.sin((x + fase_offset) * 4 * math.pi / w)])
        elif tipo in ["SQU", "PULS"]:
            try: d = float(duty_str) / 100.0
            except: d = 0.5
            d = max(0, min(1, d))
            for x in range(w):
                mod_x = (x + fase_offset) % (w / 2)
                puntos.extend([x, cy - amp if mod_x < (w / 2) * d else cy + amp])
        elif tipo == "RAMP":
            for x in range(w):
                mod_x = (x + fase_offset) % (w / 2)
                puntos.extend([x, (cy + amp) - (2 * amp * (mod_x / (w / 2)))])
        elif tipo == "DC":
            puntos = [0, cy - amp*0.8, w, cy - amp*0.8]
        elif tipo == "NOIS":
            for x in range(w):
                puntos.extend([x, cy + random.uniform(-amp, amp)])
        else: # ARB
            for x in range(w):
                puntos.extend([x, cy - amp * math.sin((x + fase_offset) * 6 * math.pi / w) * math.cos(x * 2 * math.pi / w)])
        return puntos

    def dibujar_ondas(self):
        self.canvas.delete("all")
        w, h = 280, 180
        cy, amp = h / 2, h * 0.35

        self.canvas.create_line(0, cy, w, cy, fill="#333333", dash=(2, 2))
        
        # Dibujar CH2 primero (Cian, fondo)
        pts_2 = self.generar_puntos_onda(self.var_onda_2.get(), self.var_duty_2.get(), w, cy, amp, fase_offset=20)
        if pts_2: self.canvas.create_line(pts_2, fill=FG_CH2, width=2)

        # Dibujar CH1 encima (Amarillo)
        pts_1 = self.generar_puntos_onda(self.var_onda_1.get(), self.var_duty_1.get(), w, cy, amp)
        if pts_1: self.canvas.create_line(pts_1, fill=FG_CH1, width=2)

    # --- COMUNICACIÓN Y CONVERSIÓN ---
    def conectar_equipo(self):
        try:
            direccion = self.entry_address.get()
            if direccion == "?":
                recursos = self.rm.list_resources()
                if not recursos:
                    messagebox.showwarning("Aviso", "No se encontraron instrumentos.")
                    return
                direccion = recursos[0]
                self.entry_address.delete(0, tk.END)
                self.entry_address.insert(0, direccion)

            self.instrument = self.rm.open_resource(direccion)
            self.instrument.timeout = 2000 
            self.instrument.clear()
            
            idn = self.instrument.query("*IDN?")
            self.lbl_estado.config(text=" Conectado ", bg="green")
            self.leer_parametros()
        except Exception as e:
            messagebox.showerror("Error", f"No se pudo conectar: {str(e)}")
            self.lbl_estado.config(text=" Desconectado ", bg="red")

    def leer_canal(self, ch):
        v_onda = self.var_onda_1 if ch == 1 else self.var_onda_2
        v_freq = self.var_freq_1 if ch == 1 else self.var_freq_2
        v_funit = self.var_funit_1 if ch == 1 else self.var_funit_2
        v_amp = self.var_amp_1 if ch == 1 else self.var_amp_2
        v_off = self.var_off_1 if ch == 1 else self.var_off_2
        v_duty = self.var_duty_1 if ch == 1 else self.var_duty_2
        v_load = self.var_load_1 if ch == 1 else self.var_load_2
        
        try:
            onda = self.instrument.query(f":CHANnel{ch}:BASE:WAVe?").strip().upper()
            if any(onda.startswith(v) for v in ["SIN", "SQU", "PULS", "RAMP", "ARB", "NOIS", "DC"]):
                v_onda.set(onda[:3] if onda[:3] != "SQU" else "SQU")

            freq_hz = float(self.instrument.query(f":CHANnel{ch}:BASE:FREQuency?").strip())
            if freq_hz >= 1e6: v_freq.set(str(freq_hz / 1e6)); v_funit.set("MHz")
            elif freq_hz >= 1e3: v_freq.set(str(freq_hz / 1e3)); v_funit.set("kHz")
            elif 0 < freq_hz < 1: v_freq.set(str(freq_hz * 1e3)); v_funit.set("mHz")
            else: v_freq.set(str(freq_hz)); v_funit.set("Hz")

            amp = self.instrument.query(f":CHANnel{ch}:BASE:AMPLitude?").strip()
            v_amp.set(str(float(amp)))

            offset = self.instrument.query(f":CHANnel{ch}:BASE:OFFSet?").strip()
            v_off.set(str(float(offset)))

            try:
                duty = self.instrument.query(f":CHANnel{ch}:BASE:DUTY?").strip()
                v_duty.set(str(float(duty)))
            except: pass

            try:
                load = float(self.instrument.query(f":CHANnel{ch}:LOAD?").strip())
                if load >= 10000: v_load.set("HighZ")
                else: v_load.set(str(int(load)))
            except: pass
        except Exception as e:
            print(f"Error al leer CH{ch}:", e)

    def leer_parametros(self):
        if not self.instrument: return
        self.leer_canal(1)
        self.leer_canal(2)

    def escribir_canal(self, ch):
        v_onda = self.var_onda_1 if ch == 1 else self.var_onda_2
        v_freq = self.var_freq_1 if ch == 1 else self.var_freq_2
        v_funit = self.var_funit_1 if ch == 1 else self.var_funit_2
        v_amp = self.var_amp_1 if ch == 1 else self.var_amp_2
        v_off = self.var_off_1 if ch == 1 else self.var_off_2
        v_duty = self.var_duty_1 if ch == 1 else self.var_duty_2
        v_load = self.var_load_1 if ch == 1 else self.var_load_2

        # Convertir frecuencia
        val_f = float(v_freq.get()); unit_f = v_funit.get()
        if unit_f == "mHz": hz = val_f * 1e-3
        elif unit_f == "kHz": hz = val_f * 1e3
        elif unit_f == "MHz": hz = val_f * 1e6
        else: hz = val_f

        # Escribir parámetros
        self.instrument.write(f":CHANnel{ch}:BASE:WAVe {v_onda.get()}")
        self.instrument.write(f":CHANnel{ch}:BASE:FREQuency {hz}")
        self.instrument.write(f":CHANnel{ch}:BASE:AMPLitude {v_amp.get()}")
        self.instrument.write(f":CHANnel{ch}:BASE:OFFSet {v_off.get()}")
        if v_onda.get() in ["SQU", "PULS"]:
            self.instrument.write(f":CHANnel{ch}:BASE:DUTY {v_duty.get()}")
        
        # Escribir impedancia
        load_str = v_load.get().strip().upper()
        if load_str == "HIGHZ": load_val = 10000
        else:
            try: load_val = max(1, min(10000, int(load_str)))
            except: return # Ignorar si el usuario tipeó letras inválidas
        self.instrument.write(f":CHANnel{ch}:LOAD {load_val}")

    def escribir_parametros(self):
        if not self.instrument: return
        try:
            self.escribir_canal(1)
            self.escribir_canal(2)
            messagebox.showinfo("Éxito", "Parámetros actualizados en ambos canales.")
        except Exception as e:
            messagebox.showerror("Error de Escritura", f"Revisa los valores.\nDetalle: {e}")

    def toggle_salida(self, ch):
        if not self.instrument: return
        try:
            estado_actual = self.estado_salida_1 if ch == 1 else self.estado_salida_2
            btn = self.btn_salida_1 if ch == 1 else self.btn_salida_2
            color_activo = "green" if ch == 1 else "#00b3b3" # Verde para CH1, Cian oscuro para CH2

            if not estado_actual:
                self.instrument.write(f":CHANnel{ch}:OUTPut ON")
                btn.config(text=f"CH{ch} ON", bg=color_activo, fg="white")
                if ch == 1: self.estado_salida_1 = True
                else: self.estado_salida_2 = True
            else:
                self.instrument.write(f":CHANnel{ch}:OUTPut OFF")
                btn.config(text=f"CH{ch} OFF", bg="gray", fg="white")
                if ch == 1: self.estado_salida_1 = False
                else: self.estado_salida_2 = False
        except Exception as e:
            print(f"Error salida CH{ch}:", e)

if __name__ == "__main__":
    root = tk.Tk()
    app = GeneradorDualHMI(root)
    root.mainloop()