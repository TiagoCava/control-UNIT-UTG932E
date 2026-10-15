"""
Sistema Integrado: Osciloscopio OWON SDS1102 + Generador UTG962E
Conexión exclusiva por USB para ambos equipos.
Restaurado al 100% de características originales.
"""

import sys
import time
import re
import json
import queue
import traceback
from datetime import datetime
import math

import numpy as np
import usb.core
import usb.util
import pyqtgraph as pg
import pyvisa

from PyQt6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QHBoxLayout, QGridLayout, QPushButton, QLabel, QFrame,
                             QGroupBox, QComboBox, QCheckBox, QSlider, QTabWidget,
                             QSizePolicy, QScrollArea, QLineEdit, QMessageBox)
from PyQt6.QtCore import Qt, QThread, pyqtSignal, QTimer
from PyQt6.QtGui import QFont

# ==============================================================================
# CONSTANTES (OSCILOSCOPIO)
# ==============================================================================
COUNTS_PER_DIV = 400.0
READ_CHUNK = 65536
DEBUG = "--debug" in sys.argv
OFFSET_EN_DIVISIONES = True

VOLT_SCALES = ["2mV", "5mV", "10mV", "20mV", "50mV", "100mV", "200mV", "500mV",
               "1V", "2V", "5V", "10V", "20V", "50V", "100V"]

TIME_SCALES = ["2ns", "5ns", "10ns", "20ns", "50ns", "100ns", "200ns", "500ns",
               "1us", "2us", "5us", "10us", "20us", "50us", "100us", "200us", "500us",
               "1ms", "2ms", "5ms", "10ms", "20ms", "50ms", "100ms", "200ms", "500ms",
               "1s", "2s", "5s", "10s"]

INTERVALOS = [("0.5 s", 0.5), ("1 s", 1.0), ("2 s", 2.0), ("3 s", 3.0), ("5 s", 5.0), ("10 s", 10.0)]
INTERVALO_DEF = 3

MODOS_TRIGGER = [("Auto", "AUTO"), ("Normal", "NORMal"), ("Single", "SINGle")]
PENDIENTES = [("Subida  ↑", "RISE"), ("Bajada  ↓", "FALL")]
FUENTES = [("CH1", "CH1"), ("CH2", "CH2")]

COLOR_CANAL = {"ch1": "#FFFF00", "ch2": "#00D9FF"}
COLOR_TRIG = {"ch1": "#FFC400", "ch2": "#1E90FF"}
COLOR_TRIG_HOVER = {"ch1": "#FFE066", "ch2": "#7CC4FF"}

# ==============================================================================
# ESTILOS UI
# ==============================================================================
ESTILO_BOTON = """
QPushButton {
    background-color: #ffffff; border: 1px solid #b9c0c7; border-radius: 6px;
    padding: 6px 10px; font-size: 13px;
}
QPushButton:hover   { background-color: #eef3f7; }
QPushButton:pressed { background-color: #dde5ec; }
"""

ESTILO_COMBO = """
QComboBox {
    background-color: #ffffff; border: 1px solid #b9c0c7; border-radius: 6px;
    padding: 5px 8px; font-size: 13px;
}
"""

ESTILO_GRUPO = """
QGroupBox {
    font-weight: 600; font-size: 12px; color: #4a5560;
    border: 1px solid #ccd2d8; border-radius: 8px;
    margin-top: 10px; padding-top: 10px; background-color: #fbfcfd;
}
QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }
"""

ESTILO_LINEEDIT = """
QLineEdit {
    background-color: #ffffff; border: 1px solid #b9c0c7; border-radius: 6px;
    padding: 5px 8px; font-size: 13px;
}
"""

# ==============================================================================
# FUNCIONES AUXILIARES (OSCILOSCOPIO)
# ==============================================================================
def pen_trigger(canal, activo):
    c = pg.mkColor(COLOR_TRIG[canal])
    if activo:
        return pg.mkPen(c, width=2, style=Qt.PenStyle.DashLine)
    c.setAlpha(110)
    return pg.mkPen(c, width=1, style=Qt.PenStyle.DotLine)

def log(msg):
    if DEBUG:
        print(f"[{datetime.now().strftime('%H:%M:%S.%f')[:-3]}] {msg}", flush=True)

def es_timeout(e):
    clase = getattr(usb.core, "USBTimeoutError", None)
    if clase is not None and isinstance(e, clase):
        return True
    return getattr(e, "errno", None) in (110, 10060)

def format_val(val, unit="V"):
    if val is None: return f"-- {unit}"
    if val == 0: return f"0 {unit}"
    a = abs(val)
    if a >= 1: return f"{val:.3g} {unit}"
    if a >= 1e-3: return f"{val*1e3:.3g} m{unit}"
    if a >= 1e-6: return f"{val*1e6:.3g} u{unit}"
    if a >= 1e-9: return f"{val*1e9:.3g} n{unit}"
    return f"{val:.2e} {unit}"

def volt_a_comando(volts):
    a = abs(volts)
    if a >= 1 or a == 0: return f"{volts:.3f}V"
    return f"{volts*1000:.1f}mV"

def calcular_mediciones(t_div, v_div, cache, canal="ch1"):
    vdiv = cache.get(f'{canal}_scale', 1.0)
    v = v_div * vdiv
    resultado = {
        'vpp': float(v.max() - v.min()) if len(v) else 0.0,
        'vrms': float(np.sqrt(np.mean(v ** 2))) if len(v) else 0.0,
        'vmed': float(v.mean()) if len(v) else 0.0,
        'vmax': float(v.max()) if len(v) else 0.0,
        'vmin': float(v.min()) if len(v) else 0.0,
        'freq': None, 'periodo': None, 'duty': None,
    }
    n = len(v)
    if n < 4: return resultado
    h_scale = cache.get('h_scale', 1e-3)
    dt = (10.0 * h_scale) / n
    nivel = (resultado['vmax'] + resultado['vmin']) / 2.0
    por_encima = v > nivel
    cruces_subida = np.flatnonzero((~por_encima[:-1]) & por_encima[1:])
    if len(cruces_subida) >= 2:
        periodos_muestras = np.diff(cruces_subida)
        periodo = float(np.median(periodos_muestras) * dt)
        if periodo > 0:
            resultado['periodo'] = periodo
            resultado['freq'] = 1.0 / periodo
        tramo = por_encima[cruces_subida[0]:cruces_subida[-1]]
        if len(tramo):
            resultado['duty'] = float(np.mean(tramo) * 100.0)
    return resultado

# ==============================================================================
# CLASE CONEXIÓN GENERADOR (PyVISA)
# ==============================================================================
class GeneratorBridge:
    def __init__(self):
        self.rm = pyvisa.ResourceManager()
        self.inst = None
        self.conectado = False

    def conectar(self):
        recursos = self.rm.list_resources("USB?*INSTR")
        if not recursos:
            raise ValueError("No se encontraron generadores conectados por USB.")
        
        self.inst = self.rm.open_resource(recursos[0])
        self.inst.timeout = 2000
        self.inst.clear()
        idn = self.inst.query("*IDN?")
        self.conectado = True
        return idn

    def desconectar(self):
        if self.inst:
            self.inst.close()
        self.conectado = False

    def query(self, cmd):
        if not self.conectado: return ""
        return self.inst.query(cmd)

    def write(self, cmd):
        if self.conectado:
            self.inst.write(cmd)

# ==============================================================================
# PUENTE USB OSCILOSCOPIO
# ==============================================================================
class OwonBridge:
    def __init__(self):
        self.dev = None
        self.ep_out = None
        self.ep_in = None
        self.intf_num = None
        self.vid = None
        self.pid = None
        self.head_soportado = True
        self._head_ok_alguna_vez = False
        self._head_fallos_seguidos = 0
        self._desincronizado = False
        self.cache = {
            "ch1_scale": 1.0, "ch1_probe": 1.0, "ch1_offset": 0.0,
            "ch2_scale": 1.0, "ch2_probe": 1.0, "ch2_offset": 0.0,
            "h_scale": 1e-3, "runstatus": "--", "puntos": 0,
        }

    def open(self):
        self.vid, self.pid = self._buscar()
        if self.vid is None:
            raise ValueError("No se detectó el osciloscopio en modo USB.")
        self._conectar()

    def _buscar(self):
        for d in usb.core.find(find_all=True):
            try: fab = str(usb.util.get_string(d, d.iManufacturer)).lower()
            except Exception: fab = ""
            try: prod = str(usb.util.get_string(d, d.iProduct)).lower()
            except Exception: prod = ""
            if "owon" in fab or "owon" in prod or "osc" in prod:
                return d.idVendor, d.idProduct
        d = usb.core.find(idVendor=0x5345)
        if d is not None:
            return d.idVendor, d.idProduct
        return None, None

    def _conectar(self):
        self.dev = usb.core.find(idVendor=self.vid, idProduct=self.pid)
        if self.dev is None: raise IOError("Dispositivo no encontrado en bus USB.")
        try: cfg = self.dev.get_active_configuration()
        except usb.core.USBError:
            self.dev.set_configuration()
            cfg = self.dev.get_active_configuration()
        intf = cfg[(0, 0)]
        self.intf_num = intf.bInterfaceNumber
        try: usb.util.claim_interface(self.dev, self.intf_num)
        except usb.core.USBError: pass
        self.ep_out = usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
        self.ep_in = usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)
        if not self.ep_out or not self.ep_in: raise IOError("Endpoints USB no encontrados.")
        self.flush_input()

    def conectado(self): return self.dev is not None and self.ep_in is not None and self.ep_out is not None
    def _requiere_conexion(self):
        if not self.conectado(): raise IOError("Sin conexión con el osciloscopio")

    def reconectar(self, reset=False):
        if reset and self.dev:
            try: self.dev.reset()
            except Exception: pass
        self.close()
        for intento in range(4):
            time.sleep(0.4 + 0.4 * intento)
            try:
                self._conectar()
                self._desincronizado = False
                return True
            except Exception:
                self.close()
        return False

    def recuperar_endpoints(self):
        if not self.conectado(): return
        for ep in (self.ep_in, self.ep_out):
            if ep:
                try: self.dev.clear_halt(ep.bEndpointAddress)
                except Exception: pass
        self.flush_input()

    def close(self):
        if self.dev:
            try:
                if self.intf_num is not None: usb.util.release_interface(self.dev, self.intf_num)
            except Exception: pass
            try: usb.util.dispose_resources(self.dev)
            except Exception: pass
        self.dev = self.ep_in = self.ep_out = None

    def write(self, command):
        self._requiere_conexion()
        if not command.endswith("\n"): command += "\n"
        self.ep_out.write(command.encode("ascii", errors="ignore"), timeout=1000)
        time.sleep(0.02)

    def flush_input(self, max_intentos=20, timeout=15):
        if not self.ep_in: return
        for _ in range(max_intentos):
            try:
                sobras = self.ep_in.read(READ_CHUNK, timeout=timeout)
                if not sobras: return
            except usb.core.USBError: return

    def drenar_profundo(self):
        self.flush_input(max_intentos=60, timeout=120)
        self._desincronizado = False

    def read_block(self, timeout=2500):
        self._requiere_conexion()
        raw = bytearray()
        raw.extend(self.ep_in.read(READ_CHUNK, timeout=timeout))
        if len(raw) >= 4:
            size = int.from_bytes(raw[:4], byteorder="little")
            if 0 < size < 20_000_000:
                total = 4 + size
                while len(raw) < total:
                    try: sig = self.ep_in.read(READ_CHUNK, timeout=1000)
                    except usb.core.USBError: break
                    if not sig: break
                    raw.extend(sig)
                if len(raw) < total:
                    self._desincronizado = True
                    raise IOError(f"Bloque incompleto ({len(raw)-4}/{size} B)")
                return bytes(raw[4:total])
        return bytes(raw)

    def query_block(self, comando, timeout=2500):
        if self._desincronizado: self.drenar_profundo()
        else: self.flush_input()
        self.write(comando)
        try: return self.read_block(timeout=timeout)
        except usb.core.USBError:
            self._desincronizado = True
            raise

    @staticmethod
    def parse_scaled_value(texto):
        if not texto: return None
        t = str(texto).strip().strip("\x00")
        if ":" in t: t = t.rsplit(":", 1)[-1]
        m = re.search(r"([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)\s*([a-zA-Zµ]*)\s*$", t.strip())
        if not m: return None
        try: valor = float(m.group(1))
        except ValueError: return None
        suf = m.group(2)
        mult = 1.0
        if suf:
            c = suf[0]
            if c in "nN": mult = 1e-9
            elif c in "uUµ": mult = 1e-6
            elif c == "m": mult = 1e-3
            elif c in "kK": mult = 1e3
            elif c == "M": mult = 1e6
        return valor * mult

    def leer_head(self):
        datos = self.query_block(":DATA:WAVE:SCREen:HEAD?")
        if not datos: return None
        texto = datos.decode("utf-8", errors="ignore").strip().strip("\x00")
        i, j = texto.find("{"), texto.rfind("}")
        if i < 0 or j <= i: return None
        try: return json.loads(texto[i:j + 1])
        except Exception: return None

    def _aplicar_head(self, head):
        try:
            tb = head.get("TIMEBASE", {}) or {}
            h = self.parse_scaled_value(tb.get("SCALE"))
            if h: self.cache["h_scale"] = h
            sample = head.get("SAMPLE", {}) or {}
            self.cache["runstatus"] = str(sample.get("RUNSTATUS", "--"))
            for c in (head.get("CHANNEL", []) or []):
                nombre = str(c.get("NAME", "")).upper()
                if nombre not in ("CH1", "CH2"): continue
                pref = "ch1_" if nombre == "CH1" else "ch2_"
                s = self.parse_scaled_value(c.get("SCALE"))
                if s: self.cache[pref + "scale"] = s
                p = self.parse_scaled_value(c.get("PROBE"))
                if p: self.cache[pref + "probe"] = p
                o = c.get("OFFSET")
                if o is not None:
                    try: self.cache[pref + "offset"] = float(o)
                    except ValueError: pass
        except Exception: pass

    def reload_all(self):
        for clave, cmd in (("ch1_scale", ":CH1:SCALE?"), ("ch2_scale", ":CH2:SCALE?"), ("h_scale", ":HORIzontal:SCALe?")):
            try:
                self.flush_input()
                self.write(cmd)
                datos = self.ep_in.read(READ_CHUNK, timeout=800)
                val = self.parse_scaled_value(bytes(datos).decode("ascii", errors="ignore"))
                if val: self.cache[clave] = val
            except usb.core.USBError: continue

    def _leer_canal(self, n):
        if self.head_soportado:
            head = self.leer_head() or self.leer_head()
            if head is None:
                self._head_fallos_seguidos += 1
                if self._head_ok_alguna_vez or self._head_fallos_seguidos < 4:
                    self._desincronizado = True
                    raise IOError("HEAD sin respuesta")
                self.head_soportado = False
                self.reload_all()
            else:
                self._head_ok_alguna_vez = True
                self._head_fallos_seguidos = 0
                self._aplicar_head(head)
        raw = self.query_block(f":DATA:WAVE:SCREen:CH{n}?")
        if not raw or len(raw) < 10: return None
        if len(raw) % 2: raw = raw[:-1]
        return raw

    def adquirir(self, con_ch1=True, con_ch2=False):
        raw1 = self._leer_canal(1) if con_ch1 else None
        raw2 = self._leer_canal(2) if con_ch2 else None
        base = raw1 if raw1 is not None else raw2
        if base is None: return None, None, None, None
        n = len(base) // 2
        self.cache["puntos"] = n
        t = np.linspace(-5, 5, n)
        v1 = np.frombuffer(raw1, dtype=np.int16).astype(float) / COUNTS_PER_DIV if raw1 is not None else None
        v2 = np.frombuffer(raw2, dtype=np.int16).astype(float) / COUNTS_PER_DIV if raw2 is not None else None
        firma = hash((raw1 or b"") + (raw2 or b""))
        return t, v1, v2, firma

# ==============================================================================
# HILO DE ADQUISICIÓN OSCILOSCOPIO
# ==============================================================================
class AcquisitionWorker(QThread):
    data_ready = pyqtSignal(np.ndarray, np.ndarray, np.ndarray, dict)
    status = pyqtSignal(str, str)

    def __init__(self, scope, intervalo=3.0):
        super().__init__()
        self.scope = scope
        self.running = True
        self.pausado = False
        self.intervalo = intervalo
        self.reenviar_run = False
        self.reconectar_cada_vez = False
        self.ch1_activo = True
        self.ch2_activo = False
        self.cmd_queue = queue.Queue()
        self._forzar = True
        self._fallos = 0
        self._intentos_reconexion = 0
        self._ultima_firma = None
        self._repetidas = 0
        self._contador = 0

    def send_command(self, cmd, delay=0.0):
        self.cmd_queue.put((cmd, delay))

    def forzar_captura(self): self._forzar = True
    def set_intervalo(self, s): self.intervalo = float(s)
    def set_pausa(self, p):
        self.pausado = bool(p)
        if not p: self._forzar = True

    def set_canal_activo(self, canal, activo):
        activo = bool(activo)
        def _bandera():
            if canal == 1: self.ch1_activo = activo
            else: self.ch2_activo = activo
        cmd = f":CH{canal}:DISPlay {'ON' if activo else 'OFF'}"
        if activo:
            self.send_command(cmd, delay=0.4)
            self.send_command(_bandera)
        else:
            self.send_command(_bandera)
            self.send_command(cmd, delay=0.1)
        self._forzar = True

    def run(self):
        ultima = 0.0
        while self.running:
            hubo = False
            while not self.cmd_queue.empty():
                if not self.scope.conectado():
                    while not self.cmd_queue.empty(): self.cmd_queue.get()
                    self._recuperar()
                    break
                cmd, delay = self.cmd_queue.get()
                try:
                    if callable(cmd): cmd()
                    else: self.scope.write(cmd)
                    if delay > 0: self.msleep(int(delay * 1000))
                    hubo = True
                except Exception as e:
                    self._fallo(f"Error TX: {e}", grave=True)
            if hubo: self._forzar = True
            ahora = time.time()
            if (not self.pausado) and (self._forzar or ahora - ultima >= self.intervalo):
                self._forzar = False
                ultima = time.time()
                self._capturar()
            self.msleep(30)

    def _capturar(self):
        if not (self.ch1_activo or self.ch2_activo):
            self.status.emit("Ambos canales apagados", "#F39C12")
            return
        if not self.scope.conectado():
            self._recuperar()
            return
        try:
            if self.reconectar_cada_vez: self.scope.reconectar()
            if self.reenviar_run: self.scope.write(":RUN")
            t, v1, v2, firma = self.scope.adquirir(con_ch1=self.ch1_activo, con_ch2=self.ch2_activo)
        except Exception as e:
            self._fallo(f"{type(e).__name__}: {e}")
            return
        if t is None:
            self._fallo("Respuesta vacía")
            return
        self._fallos = 0
        self._contador += 1
        self._repetidas = self._repetidas + 1 if firma == self._ultima_firma else 0
        self._ultima_firma = firma
        v1_emit = v1 if v1 is not None else np.array([])
        v2_emit = v2 if v2 is not None else np.array([])
        self.data_ready.emit(t, v1_emit, v2_emit, dict(self.scope.cache))
        hora = datetime.now().strftime("%H:%M:%S")
        rs = self.scope.cache.get("runstatus", "--")
        
        faltan = []
        if self.ch1_activo and v1 is None: faltan.append("CH1")
        if self.ch2_activo and v2 is None: faltan.append("CH2")
        etq_ch2 = f"  ·  {'/'.join(faltan)} sin datos" if faltan else ""
        
        if self._repetidas >= 3:
            self.status.emit(f"Frame repetido x{self._repetidas}  ·  {rs}  ·  {hora}{etq_ch2}", "#F39C12")
        else:
            self.status.emit(f"{rs}  ·  {hora}  ·  #{self._contador}  ·  {len(t)} pts{etq_ch2}", "#2ECC71")

    def _fallo(self, motivo, grave=False):
        self._fallos += 1
        self.status.emit(f"Sin datos ({self._fallos}): {motivo[:50]}", "#E74C3C")
        if grave or self._fallos >= 4: self._recuperar()
        else:
            if self._fallos in (2, 3):
                try: self.scope.recuperar_endpoints()
                except Exception: pass
            self.msleep(300 * self._fallos)
            self._forzar = True

    def _recuperar(self):
        self._intentos_reconexion += 1
        self.status.emit("Reconectando...", "#F39C12")
        usar_reset = self._intentos_reconexion % 3 == 0
        try: ok = self.scope.reconectar(reset=usar_reset)
        except Exception: ok = False
        if ok:
            self._fallos = 0
            self._intentos_reconexion = 0
            self.status.emit("Reconectado", "#2ECC71")
        else:
            self.status.emit("Buscando osciloscopio... (revisá el cable / Modo PC)", "#E74C3C")
            self.msleep(1000)
        self._forzar = True

    def stop(self):
        self.running = False
        self.wait(3000)

# ==============================================================================
# WIDGETS UI GENÉRICOS (OSCILOSCOPIO)
# ==============================================================================
class TarjetaMedicion(QFrame):
    def __init__(self, texto, color_acento="#569CD6", tam_valor=21, parent=None):
        super().__init__(parent)
        self._tam = tam_valor
        self.setObjectName("tarjeta")
        lay = QVBoxLayout(self)
        lay.setContentsMargins(14, 12, 14, 12)
        lay.setSpacing(4)
        self.lbl_titulo = QLabel(texto.upper())
        self.lbl_titulo.setStyleSheet("color:#7E899C; font-size:11px; font-weight:600; font-family:'Segoe UI'; border:none; letter-spacing:1px;")
        self.lbl_valor = QLabel("--")
        self.lbl_valor.setStyleSheet(f"color:#E0E6ED; font-family:'Consolas'; font-size:{self._tam}px; border:none;")
        lay.addWidget(self.lbl_titulo)
        lay.addWidget(self.lbl_valor)
        self.set_acento(color_acento)
    def set_valor(self, texto): self.lbl_valor.setText(texto)
    def set_acento(self, color):
        self.setStyleSheet(f"#tarjeta {{ background-color:#1A1D24; border:1px solid #2C313A; border-left:4px solid {color}; border-radius:8px; }}")

class VentanaMediciones(QWidget):
    canal_cambiado = pyqtSignal(str)
    ORDEN = ['vpp', 'vrms', 'freq', 'vmed', 'periodo', 'duty', 'vmax_min']
    ETIQUETAS = {'vpp': "Vpp", 'vrms': "Vrms", 'freq': "Frecuencia", 'vmed': "Vmed (DC)", 'periodo': "Período", 'duty': "Duty", 'vmax_min': "Vmax / Vmin"}
    CLAVES_BASE = ['vpp', 'vrms', 'freq', 'vmed']
    COLOR_FORMAL = {"ch1": "#D4B856", "ch2": "#569CD6"}

    def __init__(self, parent=None):
        super().__init__(parent, Qt.WindowType.Window)
        self.setWindowTitle("OWON SDS1102 · Mediciones")
        self.resize(480, 480)
        self.setStyleSheet("QWidget {background-color:#111318; color:#E0E6ED; font-family:'Segoe UI';}")
        
        lay = QVBoxLayout(self)
        lay.setContentsMargins(18, 18, 18, 18)
        lay.setSpacing(16)
        
        fila = QHBoxLayout()
        fila.setSpacing(12)
        lbl_canal = QLabel("Fuente de medición:")
        lbl_canal.setStyleSheet("font-size: 13px; font-weight: 500; color: #A0ABB8;")
        fila.addWidget(lbl_canal)
        
        self.cmb_canal = QComboBox()
        self.cmb_canal.addItems(["CH1", "CH2"])
        fila.addWidget(self.cmb_canal, stretch=1)
        lay.addLayout(fila)
        
        self.grid = QGridLayout()
        self.grid.setSpacing(12)
        self.grid.setColumnStretch(0, 1)
        self.grid.setColumnStretch(1, 1)
        lay.addLayout(self.grid)
        lay.addStretch(1)
        
        self.tarjetas = {c: TarjetaMedicion(self.ETIQUETAS[c], self.COLOR_FORMAL["ch1"], tam_valor=21) for c in self.ORDEN}
        
        g = QGroupBox("Parámetros visibles")
        gl = QGridLayout(g)
        gl.setSpacing(10)
        self.chk = {}
        for i, c in enumerate(self.ORDEN):
            self.chk[c] = QCheckBox(self.ETIQUETAS[c])
            self.chk[c].setChecked(c in self.CLAVES_BASE)
            self.chk[c].toggled.connect(lambda _v: self._reacomodar())
            gl.addWidget(self.chk[c], i // 2, i % 2)
        lay.addWidget(g)
        
        nota = QLabel("Valores calculados sobre la forma de onda descargada.")
        nota.setStyleSheet("font-size: 11px; color: #5C6370; font-style: italic;")
        nota.setAlignment(Qt.AlignmentFlag.AlignCenter)
        lay.addWidget(nota)
        
        self.cmb_canal.currentIndexChanged.connect(self._on_canal)
        self._reacomodar()

    def canal(self): return "ch1" if self.cmb_canal.currentIndex() == 0 else "ch2"
    def _on_canal(self, _indice):
        color = self.COLOR_FORMAL[self.canal()]
        for t in self.tarjetas.values(): t.set_acento(color)
        self.canal_cambiado.emit(self.canal())

    def _reacomodar(self):
        while self.grid.count(): self.grid.takeAt(0)
        visibles = [c for c in self.ORDEN if self.chk[c].isChecked()]
        for c in self.ORDEN: self.tarjetas[c].setVisible(c in visibles)
        fila = col = 0
        for c in visibles:
            if c == 'vmax_min':
                if col:
                    fila += 1
                    col = 0
                self.grid.addWidget(self.tarjetas[c], fila, 0, 1, 2)
                fila += 1
            else:
                self.grid.addWidget(self.tarjetas[c], fila, col)
                col += 1
                if col == 2:
                    col = 0
                    fila += 1

    def actualizar(self, m):
        if m is None:
            for t in self.tarjetas.values(): t.set_valor("--")
            return
        self.tarjetas['vpp'].set_valor(format_val(m['vpp']))
        self.tarjetas['vrms'].set_valor(format_val(m['vrms']))
        self.tarjetas['vmed'].set_valor(format_val(m['vmed']))
        self.tarjetas['freq'].set_valor(format_val(m['freq'], 'Hz') if m['freq'] else "--")
        self.tarjetas['periodo'].set_valor(format_val(m['periodo'], 's') if m['periodo'] else "--")
        self.tarjetas['duty'].set_valor(f"{m['duty']:.1f} %" if m['duty'] is not None else "--")
        self.tarjetas['vmax_min'].set_valor(f"{format_val(m['vmax'])} / {format_val(m['vmin'])}")

class SelectorPasos(QWidget):
    cambiado = pyqtSignal(int)
    def __init__(self, opciones, indice=0, parent=None):
        super().__init__(parent)
        lay = QHBoxLayout(self)
        lay.setContentsMargins(0, 0, 0, 0)
        lay.setSpacing(4)
        self.btn_menos = QPushButton("−"); self.btn_menos.setFixedWidth(32); self.btn_menos.setStyleSheet(ESTILO_BOTON)
        self.combo = QComboBox(); self.combo.addItems(opciones); self.combo.setCurrentIndex(indice); self.combo.setStyleSheet(ESTILO_COMBO)
        self.btn_mas = QPushButton("+"); self.btn_mas.setFixedWidth(32); self.btn_mas.setStyleSheet(ESTILO_BOTON)
        lay.addWidget(self.btn_menos); lay.addWidget(self.combo, stretch=1); lay.addWidget(self.btn_mas)
        self.btn_menos.clicked.connect(lambda: self.combo.setCurrentIndex(max(0, self.combo.currentIndex()-1)))
        self.btn_mas.clicked.connect(lambda: self.combo.setCurrentIndex(min(self.combo.count()-1, self.combo.currentIndex()+1)))
        self.combo.currentIndexChanged.connect(self.cambiado.emit)
    def indice(self): return self.combo.currentIndex()

class ControlDeslizante(QWidget):
    cambiado = pyqtSignal(float)
    def __init__(self, minimo, maximo, paso=0.02, texto_reset="Centro", parent=None):
        super().__init__(parent)
        self._paso = paso
        lay = QVBoxLayout(self)
        lay.setContentsMargins(0, 0, 0, 0)
        lay.setSpacing(3)
        fila = QHBoxLayout()
        fila.setSpacing(4)
        self.slider = QSlider(Qt.Orientation.Horizontal)
        self.slider.setRange(int(minimo / paso), int(maximo / paso))
        self.btn_reset = QPushButton(texto_reset); self.btn_reset.setStyleSheet(ESTILO_BOTON); self.btn_reset.setFixedWidth(70)
        fila.addWidget(self.slider, stretch=1); fila.addWidget(self.btn_reset)
        self.lbl = QLabel("0"); self.lbl.setAlignment(Qt.AlignmentFlag.AlignCenter); self.lbl.setStyleSheet("font-family: Consolas; font-size: 12px; color:#34495E;")
        lay.addLayout(fila); lay.addWidget(self.lbl)
        self.slider.valueChanged.connect(lambda v: self.cambiado.emit(v * self._paso))
        self.btn_reset.clicked.connect(lambda: self.slider.setValue(0))
    def valor(self): return self.slider.value() * self._paso
    def set_valor(self, v, silencioso=False):
        if silencioso: self.slider.blockSignals(True)
        self.slider.setValue(int(round(v / self._paso)))
        if silencioso: self.slider.blockSignals(False)
    def set_texto(self, texto): self.lbl.setText(texto)

# ==============================================================================
# PANTALLA PRINCIPAL (OSCILOSCOPIO RESTAURADO AL 100%)
# ==============================================================================
class OscilloscopeScreen(QWidget):
    trigger_arrastrado = pyqtSignal(str, float)
    cero_arrastrado = pyqtSignal(float)
    cero2_arrastrado = pyqtSignal(float)

    def __init__(self, parent=None):
        super().__init__(parent)
        lay = QVBoxLayout(self)
        lay.setContentsMargins(0, 0, 0, 0)
        lay.setSpacing(0)

        self.canal_trig = "ch1"

        # --- barra superior[cite: 1]
        self.top_bar = QFrame()
        self.top_bar.setStyleSheet("background-color:#161c24; border-top-left-radius:8px; border-top-right-radius:8px;")
        top = QHBoxLayout(self.top_bar)
        top.setContentsMargins(12, 6, 12, 6)
        self.lbl_status = QLabel("Iniciando...")
        self.lbl_status.setStyleSheet("color:#F39C12; font-weight:bold; font-family:Consolas; font-size:13px;")
        self.lbl_time = QLabel("M: --")
        self.lbl_time.setStyleSheet("color:#ECF0F1; font-weight:bold; font-family:Consolas; font-size:13px;")
        top.addWidget(self.lbl_status)
        top.addStretch()
        top.addWidget(self.lbl_time)

        # --- gráfico[cite: 1]
        pg.setConfigOption('background', '#0b1426')
        pg.setConfigOption('foreground', 'w')
        pg.setConfigOptions(antialias=False)

        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setXRange(-5, 5, padding=0)
        self.plot_widget.setYRange(-4, 4, padding=0)
        self.plot_widget.setLimits(xMin=-5, xMax=5, yMin=-4, yMax=4)
        self.plot_widget.setMouseEnabled(x=False, y=False)
        self.plot_widget.hideButtons()
        self.plot_widget.getAxis('bottom').setStyle(showValues=False)
        self.plot_widget.getAxis('left').setStyle(showValues=False)

        # CUADRICULA RESTAURADA[cite: 1]
        pen_grid = pg.mkPen(color=(255, 255, 255, 35), width=1, style=Qt.PenStyle.DotLine)
        pen_center = pg.mkPen(color=(255, 255, 255, 75), width=1)
        for x in range(-5, 6):
            self.plot_widget.addItem(pg.InfiniteLine(pos=x, angle=90, pen=pen_grid if x else pen_center))
        for y in range(-4, 5):
            self.plot_widget.addItem(pg.InfiniteLine(pos=y, angle=0, pen=pen_grid if y else pen_center))

        self.ch1_line = self.plot_widget.plot(pen=pg.mkPen('#FFFF00', width=2))
        self.ch2_line = self.plot_widget.plot(pen=pg.mkPen('#00D9FF', width=2))
        self.ch2_line.setVisible(False)

        # líneas de trigger restauradas con hoverPen y labelOpts[cite: 1]
        self.lineas_trig = {}
        for canal, pos_lbl, txt in (("ch1", 0.97, "T1"), ("ch2", 0.93, "T2")):
            ln = pg.InfiniteLine(
                pos=0, angle=0, movable=True,
                pen=pen_trigger(canal, canal == "ch1"),
                hoverPen=pg.mkPen(COLOR_TRIG_HOVER[canal], width=3),
                label=txt, labelOpts={'position': pos_lbl, 'color': COLOR_TRIG[canal]})
            self.plot_widget.addItem(ln)
            ln.sigPositionChangeFinished.connect(
                lambda l, c=canal: self.trigger_arrastrado.emit(c, float(l.value())))
            self.lineas_trig[canal] = ln
        self.lineas_trig["ch2"].setVisible(False)

        # marca de cero de CH1 restaurada con hoverPen y label[cite: 1]
        self.linea_cero = pg.InfiniteLine(
            pos=0, angle=0, movable=True,
            pen=pg.mkPen('#FFFF00', width=1, style=Qt.PenStyle.DashDotLine),
            hoverPen=pg.mkPen('#FFFFAA', width=3),
            label="⏵0", labelOpts={'position': 0.02, 'color': '#FFFF00'})
        self.plot_widget.addItem(self.linea_cero)

        # marca de cero de CH2 restaurada con hoverPen y label[cite: 1]
        self.linea_cero2 = pg.InfiniteLine(
            pos=0, angle=0, movable=True,
            pen=pg.mkPen('#00D9FF', width=1, style=Qt.PenStyle.DashDotLine),
            hoverPen=pg.mkPen('#9BEBFF', width=3),
            label="⏵0", labelOpts={'position': 0.06, 'color': '#00D9FF'})
        self.linea_cero2.setVisible(False)
        self.plot_widget.addItem(self.linea_cero2)

        self.linea_cero.sigPositionChangeFinished.connect(
            lambda ln: self.cero_arrastrado.emit(float(ln.value())))
        self.linea_cero2.sigPositionChangeFinished.connect(
            lambda ln: self.cero2_arrastrado.emit(float(ln.value())))

        # --- fila de estado inferior restaurada[cite: 1]
        self.info_bar = QFrame()
        self.info_bar.setStyleSheet(
            "background-color:#161c24; border-bottom-left-radius:8px; border-bottom-right-radius:8px;")
        info = QHBoxLayout(self.info_bar)
        info.setContentsMargins(12, 3, 12, 3)
        self.lbl_ch1 = QLabel("CH1: --")
        self.lbl_ch1.setStyleSheet("color:#FFFF00; font-weight:bold; font-family:Consolas; font-size:12px;")
        self.lbl_ch2 = QLabel("CH2: --")
        self.lbl_ch2.setStyleSheet("color:#00D9FF; font-weight:bold; font-family:Consolas; font-size:12px;")
        self.lbl_ch2.setVisible(False)
        self.lbl_trig = QLabel("Trig: --")
        self.lbl_trig.setStyleSheet(
            f"color:{COLOR_TRIG['ch1']}; font-weight:bold; font-family:Consolas; font-size:12px;")
        info.addWidget(self.lbl_ch1)
        info.addSpacing(18)
        info.addWidget(self.lbl_ch2)
        info.addSpacing(18)
        info.addWidget(self.lbl_trig)
        info.addStretch()

        lay.addWidget(self.top_bar)
        lay.addWidget(self.plot_widget, stretch=1)
        lay.addWidget(self.info_bar)

    @staticmethod
    def _eje(t, v): return t if len(t) == len(v) else np.linspace(-5, 5, len(v))

    def actualizar_pantalla(self, t, ch1, ch2, cache):
        self.lbl_time.setText(f"M: {format_val(cache.get('h_scale', 1e-3), 's')}/div")
        if ch1 is not None and len(ch1):
            self.ch1_line.setData(self._eje(t, ch1), ch1)
            v_div = cache.get('ch1_scale', 1.0)
            probe = cache.get('ch1_probe', 1.0)
            self.lbl_ch1.setText(f"CH1 {format_val(v_div, 'V')}/div · probe {probe:g}X")
        else:
            self.ch1_line.setData([], [])
        if ch2 is not None and len(ch2):
            self.ch2_line.setData(self._eje(t, ch2), ch2)
            v2_div = cache.get('ch2_scale', 1.0)
            probe2 = cache.get('ch2_probe', 1.0)
            self.lbl_ch2.setText(f"CH2 {format_val(v2_div, 'V')}/div · probe {probe2:g}X")
        else:
            self.ch2_line.setData([], [])

    def set_ch1_visible(self, visible):
        self.ch1_line.setVisible(visible); self.lbl_ch1.setVisible(visible); self.linea_cero.setVisible(visible); self.lineas_trig["ch1"].setVisible(visible)
        if not visible: self.ch1_line.setData([], [])
    def set_ch2_visible(self, visible):
        self.ch2_line.setVisible(visible); self.lbl_ch2.setVisible(visible); self.linea_cero2.setVisible(visible); self.lineas_trig["ch2"].setVisible(visible)
        if not visible: self.ch2_line.setData([], [])
    def set_status(self, texto, color):
        self.lbl_status.setText(texto)
        self.lbl_status.setStyleSheet(f"color:{color}; font-weight:bold; font-family:Consolas; font-size:13px;")
    def set_triggers(self, pos, canal_activo, texto):
        self.canal_trig = canal_activo
        for canal, ln in self.lineas_trig.items():
            activo = canal == canal_activo
            ln.setPen(pen_trigger(canal, activo))
            ln.blockSignals(True); ln.setValue(pos[canal]); ln.blockSignals(False)
        self.lbl_trig.setText(texto)
        self.lbl_trig.setStyleSheet(f"color:{COLOR_TRIG[canal_activo]}; font-weight:bold; font-family:Consolas; font-size:12px;")
    def set_linea_cero(self, divisiones): self.linea_cero.blockSignals(True); self.linea_cero.setValue(divisiones); self.linea_cero.blockSignals(False)
    def set_linea_cero2(self, divisiones): self.linea_cero2.blockSignals(True); self.linea_cero2.setValue(divisiones); self.linea_cero2.blockSignals(False)


# ==============================================================================
# WIDGET CANAL GENERADOR (UI Portada a PyQt6)
# ==============================================================================
class CanalGeneradorWidget(QGroupBox):
    def __init__(self, ch_num, color_hex, puente_gen):
        super().__init__(f"Canal {ch_num} (CH{ch_num})")
        self.setStyleSheet(f"""
            QGroupBox {{
                font-weight: bold; border: 1px solid #ccd2d8; border-radius: 8px;
                margin-top: 10px; padding-top: 15px; background-color: #fbfcfd;
            }}
            QGroupBox::title {{ color: {color_hex}; subcontrol-origin: margin; left: 10px; }}
        """)
        self.ch_num = ch_num
        self.puente_gen = puente_gen
        self.estado_salida = False
        self.color_hex = color_hex

        lay = QGridLayout(self)
        
        self.cmb_onda = QComboBox(); self.cmb_onda.setStyleSheet(ESTILO_COMBO)
        self.cmb_onda.addItems(["SIN", "SQU", "PULS", "RAMP", "ARB", "NOIS", "DC"])
        
        self.txt_freq = QLineEdit("1"); self.txt_freq.setStyleSheet(ESTILO_LINEEDIT)
        self.cmb_funit = QComboBox(); self.cmb_funit.setStyleSheet(ESTILO_COMBO)
        self.cmb_funit.addItems(["mHz", "Hz", "kHz", "MHz"]); self.cmb_funit.setCurrentText("kHz")
        
        self.txt_amp = QLineEdit("1.0"); self.txt_amp.setStyleSheet(ESTILO_LINEEDIT)
        self.txt_off = QLineEdit("0.0"); self.txt_off.setStyleSheet(ESTILO_LINEEDIT)
        self.txt_duty = QLineEdit("50.0"); self.txt_duty.setStyleSheet(ESTILO_LINEEDIT)
        
        self.cmb_load = QComboBox(); self.cmb_load.setStyleSheet(ESTILO_COMBO)
        self.cmb_load.addItems(["HighZ", "50", "75"])

        lay.addWidget(QLabel("Onda:"), 0, 0)
        lay.addWidget(self.cmb_onda, 0, 1, 1, 2)

        lay.addWidget(QLabel("Frecuencia:"), 1, 0)
        lay.addWidget(self.txt_freq, 1, 1)
        lay.addWidget(self.cmb_funit, 1, 2)

        lay.addWidget(QLabel("Amplitud (Vpp):"), 2, 0)
        lay.addWidget(self.txt_amp, 2, 1, 1, 2)

        lay.addWidget(QLabel("Offset (V):"), 3, 0)
        lay.addWidget(self.txt_off, 3, 1, 1, 2)

        self.lbl_duty = QLabel("Duty (%):")
        lay.addWidget(self.lbl_duty, 4, 0)
        lay.addWidget(self.txt_duty, 4, 1, 1, 2)

        lay.addWidget(QLabel("Carga (Ω):"), 5, 0)
        lay.addWidget(self.cmb_load, 5, 1, 1, 2)

        self.btn_salida = QPushButton(f"CH{self.ch_num} OFF")
        self.btn_salida.setStyleSheet("background-color: gray; color: white; font-weight: bold; padding: 10px; border-radius: 6px;")
        lay.addWidget(self.btn_salida, 6, 0, 1, 3)

        self.cmb_onda.currentTextChanged.connect(self._toggle_duty)
        self.btn_salida.clicked.connect(self.toggle_output)
        self._toggle_duty(self.cmb_onda.currentText())

    def _toggle_duty(self, onda):
        visible = onda in ["SQU", "PULS"]
        self.lbl_duty.setVisible(visible)
        self.txt_duty.setVisible(visible)

    def actualizar_desde_equipo(self):
        if not self.puente_gen.conectado: return
        try:
            ch = self.ch_num
            onda = self.puente_gen.query(f":CHANnel{ch}:BASE:WAVe?").strip().upper()
            if any(onda.startswith(v) for v in ["SIN", "SQU", "PULS", "RAMP", "ARB", "NOIS", "DC"]):
                self.cmb_onda.setCurrentText(onda[:3] if onda[:3] != "SQU" else "SQU")
            
            freq_hz = float(self.puente_gen.query(f":CHANnel{ch}:BASE:FREQuency?").strip())
            if freq_hz >= 1e6: self.txt_freq.setText(str(freq_hz / 1e6)); self.cmb_funit.setCurrentText("MHz")
            elif freq_hz >= 1e3: self.txt_freq.setText(str(freq_hz / 1e3)); self.cmb_funit.setCurrentText("kHz")
            elif 0 < freq_hz < 1: self.txt_freq.setText(str(freq_hz * 1e3)); self.cmb_funit.setCurrentText("mHz")
            else: self.txt_freq.setText(str(freq_hz)); self.cmb_funit.setCurrentText("Hz")

            self.txt_amp.setText(str(float(self.puente_gen.query(f":CHANnel{ch}:BASE:AMPLitude?").strip())))
            self.txt_off.setText(str(float(self.puente_gen.query(f":CHANnel{ch}:BASE:OFFSet?").strip())))
            
            try: self.txt_duty.setText(str(float(self.puente_gen.query(f":CHANnel{ch}:BASE:DUTY?").strip())))
            except: pass
            
            try:
                load = float(self.puente_gen.query(f":CHANnel{ch}:LOAD?").strip())
                self.cmb_load.setCurrentText("HighZ" if load >= 10000 else str(int(load)))
            except: pass
        except Exception as e:
            print(f"Error leyendo CH{self.ch_num}: {e}")

    def enviar_a_equipo(self):
        if not self.puente_gen.conectado: return
        try:
            ch = self.ch_num
            val_f = float(self.txt_freq.text()); unit_f = self.cmb_funit.currentText()
            hz = val_f * {'mHz':1e-3, 'Hz':1, 'kHz':1e3, 'MHz':1e6}[unit_f]
            
            self.puente_gen.write(f":CHANnel{ch}:BASE:WAVe {self.cmb_onda.currentText()}")
            self.puente_gen.write(f":CHANnel{ch}:BASE:FREQuency {hz}")
            self.puente_gen.write(f":CHANnel{ch}:BASE:AMPLitude {self.txt_amp.text()}")
            self.puente_gen.write(f":CHANnel{ch}:BASE:OFFSet {self.txt_off.text()}")
            if self.cmb_onda.currentText() in ["SQU", "PULS"]:
                self.puente_gen.write(f":CHANnel{ch}:BASE:DUTY {self.txt_duty.text()}")
            
            load_str = self.cmb_load.currentText().strip().upper()
            self.puente_gen.write(f":CHANnel{ch}:LOAD {10000 if load_str == 'HIGHZ' else int(load_str)}")
        except Exception as e:
            print(f"Error escribiendo CH{self.ch_num}: {e}")

    def toggle_output(self):
        if not self.puente_gen.conectado: return
        self.estado_salida = not self.estado_salida
        if self.estado_salida:
            self.puente_gen.write(f":CHANnel{self.ch_num}:OUTPut ON")
            self.btn_salida.setText(f"CH{self.ch_num} ON")
            self.btn_salida.setStyleSheet(f"background-color: {self.color_hex}; color: black; font-weight: bold; padding: 10px; border-radius: 6px;")
        else:
            self.puente_gen.write(f":CHANnel{self.ch_num}:OUTPut OFF")
            self.btn_salida.setText(f"CH{self.ch_num} OFF")
            self.btn_salida.setStyleSheet("background-color: gray; color: white; font-weight: bold; padding: 10px; border-radius: 6px;")

# ==============================================================================
# INTERFAZ UNIFICADA
# ==============================================================================
class IntegratedGUI(QMainWindow):
    def __init__(self, scope, generator):
        super().__init__()
        self.scope = scope
        self.generator = generator
        self._ultimo = None
        self._niveles = {"ch1": 0.0, "ch2": 0.0}
        
        self.setWindowTitle("Sistema de Laboratorio Integrado (OWON SDS1102 + UTG962E)")
        self.setStyleSheet("QMainWindow, QWidget { background-color:#f4f6f8; color:#2c3e50; }")

        pantalla = QApplication.primaryScreen().availableGeometry()
        self.resize(max(1180, pantalla.width() - 40), max(760, pantalla.height() - 40))
        self.move(pantalla.center() - self.rect().center())

        central = QWidget()
        self.setCentralWidget(central)
        main = QHBoxLayout(central)
        main.setContentsMargins(14, 14, 14, 14)
        main.setSpacing(14)

        self.ventana_med = VentanaMediciones(self)
        self.ventana_med.canal_cambiado.connect(lambda _c: self._actualizar_mediciones())

        # Pantalla central (Osciloscopio)
        self.screen = OscilloscopeScreen()
        self.screen.setMinimumWidth(500)
        main.addWidget(self.screen, stretch=4)

        # Panel derecho con Pestañas
        self.tabs = QTabWidget()
        self.tabs.setFixedWidth(340)
        self.tabs.setStyleSheet("""
            QTabWidget::pane { border: 1px solid #ccd2d8; border-radius: 5px; background: white; }
            QTabBar::tab { background: #eef3f7; border: 1px solid #ccd2d8; padding: 8px 15px; border-top-left-radius: 4px; border-top-right-radius: 4px; }
            QTabBar::tab:selected { background: white; font-weight: bold; }
        """)
        
        # Pestaña 1: Osciloscopio
        scroll_osc = QScrollArea(); scroll_osc.setWidgetResizable(True); scroll_osc.setFrameShape(QFrame.Shape.NoFrame)
        scroll_osc.setStyleSheet("QScrollArea { background: transparent; border: none; }")
        scroll_osc.setWidget(self._construir_panel_osciloscopio())
        self.tabs.addTab(scroll_osc, "Osciloscopio")

        # Pestaña 2: Generador
        scroll_gen = QScrollArea(); scroll_gen.setWidgetResizable(True); scroll_gen.setFrameShape(QFrame.Shape.NoFrame)
        scroll_gen.setStyleSheet("QScrollArea { background: transparent; border: none; }")
        scroll_gen.setWidget(self._construir_panel_generador())
        self.tabs.addTab(scroll_gen, "Generador")

        main.addWidget(self.tabs)

        # Timers Scope
        self.t_escala_v = self._timer(self._enviar_escala_v)
        self.t_escala_h = self._timer(self._enviar_escala_h)
        self.t_cero = self._timer(self._enviar_cero, 250)
        self.t_nivel = self._timer(self._enviar_nivel_trigger, 250)
        self.t_cero2 = self._timer(lambda: self._enviar_cero("ch2"), 250)
        self.t_escala_v2 = self._timer(self._enviar_escala_ch2)

        # Hilo Osciloscopio
        self.worker = AcquisitionWorker(self.scope, INTERVALOS[INTERVALO_DEF][1])
        self.worker.data_ready.connect(self._on_datos)
        self.worker.status.connect(self.screen.set_status)
        self.worker.start()

        self._refrescar_etiquetas()
        QTimer.singleShot(600, self._enviar_config_trigger)

    def _timer(self, slot, ms=400):
        t = QTimer(self); t.setSingleShot(True); t.setInterval(ms)
        t.timeout.connect(slot)
        return t

    def _titulo(self, texto):
        l = QLabel(texto); l.setStyleSheet("font-size:11px; color:#7f8c8d; font-weight:600;")
        return l

    # ========================== UI OSCILOSCOPIO RESTAURADA ==========================
    def _construir_panel_osciloscopio(self):
        panel = QFrame(); panel.setStyleSheet("QFrame { background-color:#ffffff; border:1px solid #e1e5e9; border-radius:10px; }")
        r = QVBoxLayout(panel); r.setContentsMargins(14, 14, 14, 14); r.setSpacing(10)

        titulo = QLabel("OWON SDS1102")
        titulo.setFont(QFont("Segoe UI", 14, QFont.Weight.Bold))
        titulo.setAlignment(Qt.AlignmentFlag.AlignCenter)
        r.addWidget(titulo)

        self.btn_pausa = QPushButton("⏸  Pausar"); self.btn_pausa.setCheckable(True); self.btn_pausa.setMinimumHeight(38)
        self.btn_pausa.setStyleSheet("QPushButton { background-color:#27AE60; color:white; font-weight:bold; border:none; border-radius:6px; font-size:13px; } QPushButton:checked { background-color:#E67E22; }")
        r.addWidget(self.btn_pausa)
        
        self.btn_med = QPushButton("📊  Ventana de mediciones"); self.btn_med.setMinimumHeight(34); self.btn_med.setStyleSheet(ESTILO_BOTON)
        r.addWidget(self.btn_med)

        # Restaurado checkbox ":RUN"[cite: 1]
        g_ref = QGroupBox("Refresco"); g_ref.setStyleSheet(ESTILO_GRUPO); l_ref = QVBoxLayout(g_ref); l_ref.setSpacing(6)
        self.sel_intervalo = SelectorPasos([e for e, _ in INTERVALOS], INTERVALO_DEF)
        l_ref.addWidget(self.sel_intervalo)
        self.btn_ahora = QPushButton("Actualizar ahora"); self.btn_ahora.setStyleSheet(ESTILO_BOTON)
        l_ref.addWidget(self.btn_ahora)
        self.chk_run = QCheckBox("Enviar :RUN antes de capturar"); self.chk_run.setStyleSheet("font-size:12px;")
        l_ref.addWidget(self.chk_run)
        r.addWidget(g_ref)

        g_v = QGroupBox("Vertical  ·  CH1"); g_v.setStyleSheet(ESTILO_GRUPO); l_v = QVBoxLayout(g_v); l_v.setSpacing(6)
        self.chk_ch1 = QCheckBox("Activar CH1"); self.chk_ch1.setChecked(True); self.chk_ch1.setStyleSheet("font-size:12px;")
        l_v.addWidget(self.chk_ch1); l_v.addWidget(self._titulo("Escala (V/div)"))
        self.sel_volt = SelectorPasos(VOLT_SCALES, 8); l_v.addWidget(self.sel_volt)
        l_v.addWidget(self._titulo("Cero (posición vertical)")); self.ctl_cero = ControlDeslizante(-4.0, 4.0, 0.04, "Centro"); l_v.addWidget(self.ctl_cero)
        r.addWidget(g_v)

        # Restaurados avisos en CH2[cite: 1]
        g_v2 = QGroupBox("Canal 2 (CH2)"); g_v2.setStyleSheet(ESTILO_GRUPO); l_v2 = QVBoxLayout(g_v2); l_v2.setSpacing(6)
        self.chk_ch2 = QCheckBox("Activar CH2"); self.chk_ch2.setStyleSheet("font-size:12px;")
        l_v2.addWidget(self.chk_ch2)
        aviso_ch2 = QLabel("Pide un bloque de datos extra al equipo\npor cada captura. Si notás capturas\nlentas o repetidas, subí el intervalo\nde refresco.")
        aviso_ch2.setStyleSheet("font-size:10px; color:#95a5a6;"); aviso_ch2.setWordWrap(True)
        l_v2.addWidget(aviso_ch2)
        l_v2.addWidget(self._titulo("Escala CH2 (V/div)"))
        self.sel_volt_ch2 = SelectorPasos(VOLT_SCALES, 8); self.sel_volt_ch2.setEnabled(False); l_v2.addWidget(self.sel_volt_ch2)
        l_v2.addWidget(self._titulo("Cero CH2 (posición vertical)")); self.ctl_cero2 = ControlDeslizante(-4.0, 4.0, 0.04, "Centro"); self.ctl_cero2.setEnabled(False); l_v2.addWidget(self.ctl_cero2)
        r.addWidget(g_v2)

        g_h = QGroupBox("Horizontal"); g_h.setStyleSheet(ESTILO_GRUPO); l_h = QVBoxLayout(g_h); l_h.setSpacing(6)
        l_h.addWidget(self._titulo("Base de tiempo (s/div)")); self.sel_time = SelectorPasos(TIME_SCALES, 17); l_h.addWidget(self.sel_time)
        r.addWidget(g_h)

        g_t = QGroupBox("Trigger"); g_t.setStyleSheet(ESTILO_GRUPO); l_t = QVBoxLayout(g_t); l_t.setSpacing(6)
        f1 = QHBoxLayout(); f1.setSpacing(6)
        self.cmb_fuente = QComboBox(); self.cmb_fuente.addItems([e for e, _ in FUENTES]); self.cmb_fuente.setStyleSheet(ESTILO_COMBO)
        self.cmb_pendiente = QComboBox(); self.cmb_pendiente.addItems([e for e, _ in PENDIENTES]); self.cmb_pendiente.setStyleSheet(ESTILO_COMBO)
        f1.addWidget(self.cmb_fuente); f1.addWidget(self.cmb_pendiente); l_t.addLayout(f1)
        self.cmb_modo = QComboBox(); self.cmb_modo.addItems([e for e, _ in MODOS_TRIGGER]); self.cmb_modo.setStyleSheet(ESTILO_COMBO)
        l_t.addWidget(self.cmb_modo)
        self.lbl_nivel = self._titulo("Nivel (respecto al cero de CH1)"); l_t.addWidget(self.lbl_nivel)
        self.ctl_nivel = ControlDeslizante(-4.0, 4.0, 0.04, "50%"); l_t.addWidget(self.ctl_nivel)
        r.addWidget(g_t)
        
        # Restaurado texto de ayuda[cite: 1]
        ayuda = QLabel("Podés arrastrar las líneas de trigger (T1 ámbar / T2 azul)\ny las de cero (amarilla CH1 / cian CH2) directo en la pantalla.\nAl agarrar el trigger del otro canal, pasa a ser la fuente.")
        ayuda.setStyleSheet("font-size:11px; color:#95a5a6;"); ayuda.setWordWrap(True)
        r.addWidget(ayuda)
        
        r.addStretch()

        self.btn_pausa.toggled.connect(self._on_pausa)
        self.btn_med.clicked.connect(self.ventana_med.show)
        self.btn_ahora.clicked.connect(lambda: self.worker.forzar_captura())
        self.sel_intervalo.cambiado.connect(lambda i: self.worker.set_intervalo(INTERVALOS[i][1]))
        self.chk_run.toggled.connect(lambda v: setattr(self.worker, "reenviar_run", v))
        self.sel_volt.cambiado.connect(lambda _: self.t_escala_v.start()); self.sel_time.cambiado.connect(lambda _: self.t_escala_h.start())
        self.ctl_cero.cambiado.connect(lambda _: self._refrescar_etiquetas() or self.t_cero.start())
        self.ctl_nivel.cambiado.connect(lambda v: self._niveles.update({self._canal_trigger(): v}) or self._refrescar_etiquetas() or self.t_nivel.start())
        self.cmb_fuente.currentIndexChanged.connect(lambda _: self.ctl_nivel.set_valor(self._niveles[self._canal_trigger()], silencioso=True) or self._enviar_config_trigger())
        self.cmb_pendiente.currentIndexChanged.connect(lambda _: self._enviar_config_trigger())
        self.cmb_modo.currentIndexChanged.connect(lambda _: self._enviar_config_trigger())
        self.screen.trigger_arrastrado.connect(self._on_trigger_arrastrado); self.screen.cero_arrastrado.connect(self._on_cero_arrastrado)
        self.chk_ch2.toggled.connect(self._on_ch2_toggled); self.chk_ch1.toggled.connect(self._on_ch1_toggled)
        self.sel_volt_ch2.cambiado.connect(lambda _: self.t_escala_v2.start())
        self.ctl_cero2.cambiado.connect(lambda _: self._refrescar_etiquetas() or self.t_cero2.start())
        self.screen.cero2_arrastrado.connect(self._on_cero2_arrastrado)
        self.ctl_nivel.btn_reset.clicked.connect(self._nivel_al_50)
        
        return panel

    def _on_pausa(self, pausado):
        self.worker.set_pausa(pausado)
        self.btn_pausa.setText("▶  Reanudar" if pausado else "⏸  Pausar")
        if pausado: self.screen.set_status("PAUSA", "#E67E22")

    def _nivel_al_50(self):
        linea = self.screen.ch1_line if self._canal_trigger() == "ch1" else self.screen.ch2_line
        datos = linea.getData()
        if datos and datos[1] is not None and len(datos[1]):
            medio = float((datos[1].max() + datos[1].min()) / 2.0)
            self.ctl_nivel.set_valor(max(-4.0, min(4.0, medio - self._cero_trigger())))

    # ========================== UI GENERADOR ==========================
    def _construir_panel_generador(self):
        panel = QFrame(); panel.setStyleSheet("QFrame { background-color:#ffffff; border:1px solid #e1e5e9; border-radius:10px; }")
        r = QVBoxLayout(panel); r.setContentsMargins(14, 14, 14, 14); r.setSpacing(10)

        titulo = QLabel("UTG962E")
        titulo.setFont(QFont("Segoe UI", 14, QFont.Weight.Bold))
        titulo.setAlignment(Qt.AlignmentFlag.AlignCenter)
        r.addWidget(titulo)

        # Estado Conexión Generador
        conn_layout = QHBoxLayout()
        self.btn_conn_gen = QPushButton("Conectar USB")
        self.btn_conn_gen.setStyleSheet("background-color: #3498db; color: white; padding: 8px; border-radius: 6px; font-weight: bold;")
        self.lbl_estado_gen = QLabel("Desconectado")
        self.lbl_estado_gen.setStyleSheet("color: #e74c3c; font-weight: bold;")
        conn_layout.addWidget(self.btn_conn_gen); conn_layout.addWidget(self.lbl_estado_gen)
        r.addLayout(conn_layout)

        # Widgets de los Canales
        self.ch1_gen = CanalGeneradorWidget(1, "#d4b856", self.generator)
        self.ch2_gen = CanalGeneradorWidget(2, "#569cd6", self.generator)
        
        r.addWidget(self.ch1_gen)
        r.addWidget(self.ch2_gen)

        # Botones de Acción (Lectura/Escritura)
        self.btn_leer_gen = QPushButton("⭳ Leer desde Equipo")
        self.btn_leer_gen.setStyleSheet("background-color: #95a5a6; color: white; padding: 10px; border-radius: 6px; font-weight: bold;")
        self.btn_escribir_gen = QPushButton("⭱ Escribir Cambios")
        self.btn_escribir_gen.setStyleSheet("background-color: #27ae60; color: white; padding: 10px; border-radius: 6px; font-weight: bold;")
        
        r.addWidget(self.btn_leer_gen)
        r.addWidget(self.btn_escribir_gen)
        r.addStretch()

        self.btn_conn_gen.clicked.connect(self._conectar_generador)
        self.btn_leer_gen.clicked.connect(self._leer_generador)
        self.btn_escribir_gen.clicked.connect(self._escribir_generador)

        return panel

    # ===================== LOGICA OSCILOSCOPIO =====================
    def _v_div_actual(self, canal="ch1"): return self.scope.cache.get(f"{canal}_scale", 1.0) or 1.0
    def _canal_trigger(self): return "ch1" if self.cmb_fuente.currentIndex() == 0 else "ch2"
    def _cero_canal(self, canal): return self.ctl_cero.valor() if canal == "ch1" else self.ctl_cero2.valor()
    def _cero_trigger(self): return self._cero_canal(self._canal_trigger())
    
    def _refrescar_etiquetas(self):
        vdiv1 = self._v_div_actual("ch1"); vdiv2 = self._v_div_actual("ch2")
        canal_trig = self._canal_trigger(); vdiv_trig = vdiv1 if canal_trig == "ch1" else vdiv2
        nivel_div = self._niveles[canal_trig]; cero1 = self.ctl_cero.valor(); cero2 = self.ctl_cero2.valor()
        
        self.ctl_nivel.set_texto(f"{nivel_div:+.2f} div  =  {format_val(nivel_div * vdiv_trig)}")
        self.lbl_nivel.setText(f"Nivel (respecto al cero de {canal_trig.upper()})")
        self.lbl_nivel.setStyleSheet(f"font-size:11px; font-weight:600; color:{COLOR_TRIG[canal_trig]};")
        self.ctl_cero.set_texto(f"{cero1:+.2f} div  =  {format_val(cero1 * vdiv1)}")
        self.ctl_cero2.set_texto(f"{cero2:+.2f} div  =  {format_val(cero2 * vdiv2)}")
        
        pos = {"ch1": cero1 + self._niveles["ch1"], "ch2": cero2 + self._niveles["ch2"]}
        self.screen.set_triggers(pos, canal_trig, f"Trig {canal_trig.upper()} {format_val(nivel_div * vdiv_trig)}")
        self.screen.set_linea_cero(cero1); self.screen.set_linea_cero2(cero2)

    def _on_datos(self, t, v, v2, cache):
        self._ultimo = (t, v, v2, cache)
        self.screen.actualizar_pantalla(t, v, v2, cache)
        self._actualizar_mediciones(); self._refrescar_etiquetas()

    def _actualizar_mediciones(self):
        if self._ultimo is None: return
        t, v1, v2, cache = self._ultimo
        canal = self.ventana_med.canal()
        datos = v1 if canal == "ch1" else v2
        if datos is not None and len(datos):
            self.ventana_med.actualizar(calcular_mediciones(OscilloscopeScreen._eje(t, datos), datos, cache, canal))
        else: self.ventana_med.actualizar(None)

    def _on_ch1_toggled(self, activo):
        self.worker.set_canal_activo(1, activo); self.screen.set_ch1_visible(activo)
        self.sel_volt.setEnabled(activo); self.ctl_cero.setEnabled(activo)
        self._ajustar_fuentes()
        
    def _on_ch2_toggled(self, activo):
        self.worker.set_canal_activo(2, activo); self.screen.set_ch2_visible(activo)
        self.sel_volt_ch2.setEnabled(activo); self.ctl_cero2.setEnabled(activo)
        if activo: self._enviar_escala_ch2()
        self._ajustar_fuentes()

    def _ajustar_fuentes(self):
        ch1, ch2 = self.chk_ch1.isChecked(), self.chk_ch2.isChecked()
        if ch1 == ch2: return
        unico = 0 if ch1 else 1
        if self.ventana_med.cmb_canal.currentIndex() != unico: self.ventana_med.cmb_canal.setCurrentIndex(unico)
        if self.cmb_fuente.currentIndex() != unico: self.cmb_fuente.setCurrentIndex(unico)

    def _enviar_escala_v(self): self.worker.send_command(f":CH1:SCALE {VOLT_SCALES[self.sel_volt.indice()]}", delay=0.3); self._refrescar_etiquetas()
    def _enviar_escala_h(self): self.worker.send_command(f":HORIzontal:SCALE {TIME_SCALES[self.sel_time.indice()]}", delay=0.3)
    def _enviar_escala_ch2(self): self.worker.send_command(f":CH2:SCALE {VOLT_SCALES[self.sel_volt_ch2.indice()]}", delay=0.3); self._refrescar_etiquetas()

    def _on_cero_arrastrado(self, divisiones):
        self.ctl_cero.set_valor(max(-4.0, min(4.0, divisiones)), silencioso=True)
        self._refrescar_etiquetas(); self.t_cero.start()
    def _on_cero2_arrastrado(self, divisiones):
        self.ctl_cero2.set_valor(max(-4.0, min(4.0, divisiones)), silencioso=True)
        self._refrescar_etiquetas(); self.t_cero2.start()
    def _on_trigger_arrastrado(self, canal, divisiones):
        relativo = max(-4.0, min(4.0, max(-4.0, min(4.0, divisiones)) - self._cero_canal(canal)))
        self._niveles[canal] = relativo
        if canal == self._canal_trigger():
            self.ctl_nivel.set_valor(relativo, silencioso=True); self._refrescar_etiquetas(); self.t_nivel.start()
        else: self.cmb_fuente.setCurrentIndex(0 if canal == "ch1" else 1)

    def _enviar_cero(self, canal="ch1"):
        div = self.ctl_cero.valor() if canal == "ch1" else self.ctl_cero2.valor()
        cmd = f":{canal.upper()}:OFFSet {div:.2f}" if OFFSET_EN_DIVISIONES else f":{canal.upper()}:OFFSet {volt_a_comando(div * self._v_div_actual(canal))}"
        self.worker.send_command(cmd)

    def _enviar_nivel_trigger(self):
        self.worker.send_command(f":TRIGger:SINGle:EDGe:LEVel {volt_a_comando(self.ctl_nivel.valor() * self._v_div_actual(self._canal_trigger()))}")
    
    def _enviar_config_trigger(self):
        self.worker.send_command(":TRIGger:SINGle EDGe")
        self.worker.send_command(f":TRIGger:SINGle:EDGe:SOURce {FUENTES[self.cmb_fuente.currentIndex()][1]}")
        self.worker.send_command(f":TRIGger:SINGle:EDGe:SLOPe {PENDIENTES[self.cmb_pendiente.currentIndex()][1]}")
        self.worker.send_command(f":TRIGger:SINGle:MODE {MODOS_TRIGGER[self.cmb_modo.currentIndex()][1]}")
        self._enviar_nivel_trigger(); self._refrescar_etiquetas()

    # ===================== LOGICA GENERADOR =====================
    def _conectar_generador(self):
        try:
            idn = self.generator.conectar()
            self.lbl_estado_gen.setText("Conectado USB")
            self.lbl_estado_gen.setStyleSheet("color: #27ae60; font-weight: bold;")
            self._leer_generador()
        except Exception as e:
            QMessageBox.warning(self, "Error Generador", f"No se pudo conectar el generador USB:\n{e}")
            self.lbl_estado_gen.setText("Desconectado")
            self.lbl_estado_gen.setStyleSheet("color: #e74c3c; font-weight: bold;")

    def _leer_generador(self):
        if not self.generator.conectado: return
        self.ch1_gen.actualizar_desde_equipo()
        self.ch2_gen.actualizar_desde_equipo()

    def _escribir_generador(self):
        if not self.generator.conectado:
            QMessageBox.warning(self, "Error", "El generador no está conectado.")
            return
        self.ch1_gen.enviar_a_equipo()
        self.ch2_gen.enviar_a_equipo()

    def closeEvent(self, event):
        self.ventana_med.close()
        self.worker.stop()
        self.scope.close()
        self.generator.desconectar()
        event.accept()

# ==============================================================================
# ENTRY POINT
# ==============================================================================
if __name__ == '__main__':
    app = QApplication(sys.argv)
    scope_bridge = OwonBridge()
    generator_bridge = GeneratorBridge()
    try:
        scope_bridge.open()
        try: scope_bridge.write(":RUN")
        except Exception: pass
        
        w = IntegratedGUI(scope_bridge, generator_bridge)
        w.show()
        sys.exit(app.exec())
    except Exception as e:
        print(f"Error crítico al iniciar: {e}")
        traceback.print_exc()
        scope_bridge.close()