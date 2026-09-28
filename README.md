# MK Mythologies: Sub-Zero (Game Boy DMG prototype)

This repository contains a lightweight Game Boy Original (DMG) prototype inspired by Mortal Kombat Mythologies: Sub-Zero. It is intentionally simplified to fit the graphical and technical limits of the original hardware.

## Features

- 2D fight arena on a single screen
- Player movement and jumping
- Enemy pursuit AI
- Ice projectile attack
- Health tracking
- Title intro and restart loop
- Built for GBDK-2020

## Controls

- D-pad: move
- A: throw ice blast
- B: jump
- START: return to title screen after game over / victory

## Build

```bash
make
```

The ROM will be generated in `build/`.

## Notes

This is a compact gameplay demo rather than a full conversion. The original Game Boy has no sprite scaling, limited VRAM, and very constrained CPU power, so the project focuses on the feel and iconography of Sub-Zero rather than trying to reproduce the full PS1 game.
