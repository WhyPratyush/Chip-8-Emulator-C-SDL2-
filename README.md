# CHIP-8 Emulator (C++ + SDL2)

A simple CHIP-8 emulator written in C++ using SDL2 for graphics and input.

---

## Features

* Complete CHIP-8 CPU implementation
* 64×32 monochrome display using SDL2
* Keyboard input mapping
* Delay and sound timers (60 Hz)
* ROM loading support
* Tested with multiple ROMs (Pong, Tetris, Space Invaders, etc.)

---

## Controls

Mapped keyboard layout:

```
1 2 3 4      → 1 2 3 C
Q W E R      → 4 5 6 D
A S D F      → 7 8 9 E
Z X C V      → A 0 B F
```

---

## Build Instructions

### Requirements

* C++17 compiler (g++)
* SDL2

### On Linux / WSL:

```bash
sudo apt install libsdl2-dev
g++ main.cpp -o emu -lSDL2 -pthread
```

---

## Run

```bash
./emu <ROM file>
```

Example:

```bash
./emu pong.ch8
```

