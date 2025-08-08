# 🌱 Roadmap: Juego de Planta Tamagotchi en C

## 🎯 Objetivo inicial
Crear un juego simple donde el jugador cuida una planta en una maceta:
- La planta tiene niveles de agua, nutrientes y salud.
- El jugador puede regarla, nutrirla, podarla y eventualmente trasplantarla.
- Los valores cambian con el tiempo.

---

## 🗓️ Etapas del desarrollo

### ✅ Milestone 1: Núcleo del juego
- [X] Crear estructura `Planta` con `agua`, `nutrientes`, `salud`.
- [X] Implementar funciones para `regar()`, `nutrir()`, `actualizar()`.
- [X] Crear un ciclo de simulación por "tick" (ej: 1 por segundo).
- [X] Imprimir stats en consola o dibujar en ventana (con `raylib`).

TODO:
- [ ] Mover plant, uiButton y ui a sus propios archivos
- [ ] Crear archivos de cabecera (necesario?)

### ✅ Milestone 2: Interacción del jugador
- [ ] Agregar sistema de input: teclas para regar/nutrir/podar.
- [ ] Dibujar planta en pantalla (circulo y maceta).
- [ ] Mostrar stats visuales (texto o barras). (listo en Milestone 1 pero hagamoslo mas boni)
- [ ] Implementar efectos negativos si no se cuida.

### ✅ Milestone 3: Sistema de eventos y persistencia
- [ ] Implementar sistema de eventos temporales (simular paso del tiempo).
- [ ] Registrar historial de acciones recientes (cola circular).
- [ ] Agregar guardado/carga en archivo (`.txt`, `.json`, etc.).

### ✅ Milestone 4: Evolución y ambiente
- [ ] Agregar estados (`saludable`, `marchita`, `muerta`).
- [ ] Introducir clima aleatorio que afecte la planta (ej: `lluvia` → +agua).
- [ ] Mostrar cambios de estado en pantalla (colores, texto).
- [ ] Agregar retroalimentación visual/auditiva al interactuar.

---

## 🔁 Iteraciones futuras (visión larga)
- [ ] Permitir múltiples plantas con necesidades distintas.
- [ ] Añadir sistema de luz solar y temperatura.
- [ ] Sistema de trasplante a nuevas macetas.
- [ ] Expandir a diseño de jardín con cuadrícula.
- [ ] Simulación más profunda: enfermedades, estaciones, genética.

---

## 📦 Recursos técnicos sugeridos

- **Lenguaje:** C
- **Librería gráfica:** raylib
- **IDE/editor:** Neovim, VSCode, CLion, etc.
- **Compilador:** gcc / clang
- **Sistema de build:** Makefile

---

## 🧠 Sugerencias de aprendizaje paralelo

- Leer: *C Programming: A Modern Approach* – K. N. King
- Practicar: estructuras de datos como listas, colas, árboles simples
- Entender: cómo funciona el heap, stack, y manejo de memoria en C
- Explorar: raylib, ejemplos y herramientas como `raygui`, `raudio`, etc.

---


