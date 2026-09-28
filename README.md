# MK Mythologies: Sub-Zero (Game Boy DMG prototype)

This repository contains a compact Game Boy Original (DMG) tech demo inspired by Mortal Kombat Mythologies: Sub-Zero. It is not a full commercial conversion of the original game; instead, it is a lightweight playable prototype built around the limitations of the original Game Boy hardware.

## What is included

- Side-scrolling arena layout
- Player movement with left/right controls
- Jumping and basic attack behavior
- Enemy AI that advances toward the hero
- Ice projectile attack
- Simple health system
- GBDK-2020 build setup

## Build requirements

- GBDK-2020
- GNU Make
- A Game Boy emulator or flash cart for testing

## Build

```bash
make
```

This produces a DMG ROM in the `build/` folder.

## Controls

- D-pad: move
- A: ice projectile
- B: jump

## Notes

This project is intentionally small and optimized for a Game Boy DMG target. The goal is to preserve the feel of a fast 2D arena fighter on very limited hardware rather than trying to reproduce the entire PS1-era production in a single ROM.

## File layout

```text
src/main.c   Game logic and rendering
Makefile     GBDK build configuration
```
