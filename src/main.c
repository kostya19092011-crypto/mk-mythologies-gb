#include <gb/gb.h>
#include <stdint.h>

#define PLAYER_SPRITE 0
#define ENEMY_SPRITE 1
#define PROJECTILE_SPRITE 2
#define ARENA_W 20
#define ARENA_H 18
#define MAX_HEALTH 10
#define GROUND_Y 96

#define TITLE_STATE 0
#define PLAYING_STATE 1
#define WIN_STATE 2
#define LOSE_STATE 3

typedef struct {
    UINT8 x;
    UINT8 y;
    UINT8 w;
    UINT8 h;
    UINT8 dir;
    UINT8 health;
    UINT8 jumping;
    UINT8 jumpTimer;
    UINT8 attackCooldown;
} Actor;

UINT8 arenaMap[ARENA_W * ARENA_H];
UINT8 projectileX;
UINT8 projectileY;
UINT8 projectileDir;
UINT8 projectileActive;
UINT8 gameState;
UINT8 frameCounter;

const UINT8 spriteTiles[3 * 16] = {
    /* hero */
    0x00, 0x00, 0x00, 0x00, 0x3C, 0x3C, 0x7E, 0x7E,
    0x7E, 0x18, 0x3C, 0x3C, 0x3C, 0x18, 0x18, 0x00,

    /* enemy */
    0x00, 0x00, 0x00, 0x00, 0x3C, 0x3C, 0x1E, 0x7E,
    0x3C, 0x18, 0x3C, 0x1E, 0x1E, 0x18, 0x18, 0x00,

    /* ice shard */
    0x00, 0x00, 0x04, 0x0E, 0x1F, 0x0E, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x0E, 0x1F, 0x0E, 0x04
};

const UINT8 bgTiles[3 * 16] = {
    /* tile 0: empty */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* tile 1: ground */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    /* tile 2: stone */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55
};

void initArena(void) {
    UINT8 x;
    UINT8 y;

    for (y = 0; y < ARENA_H; ++y) {
        for (x = 0; x < ARENA_W; ++x) {
            if (y < 12) {
                arenaMap[y * ARENA_W + x] = 2;
            } else if (y < 17) {
                arenaMap[y * ARENA_W + x] = 1;
            } else {
                arenaMap[y * ARENA_W + x] = 0;
            }
        }
    }

    set_bkg_data(0, 3, bgTiles);
    set_bkg_tiles(0, 0, ARENA_W, ARENA_H, arenaMap);
}

void resetRound(Actor *player, Actor *enemy) {
    player->x = 20;
    player->y = GROUND_Y;
    player->dir = 1;
    player->health = MAX_HEALTH;
    player->jumping = 0;
    player->jumpTimer = 0;
    player->attackCooldown = 0;

    enemy->x = 120;
    enemy->y = GROUND_Y;
    enemy->dir = 0;
    enemy->health = MAX_HEALTH;
    enemy->jumping = 0;
    enemy->jumpTimer = 0;
    enemy->attackCooldown = 0;

    hideProjectile();
}

void hideProjectile(void) {
    projectileActive = 0;
    move_sprite(PROJECTILE_SPRITE, 0, 0);
}

void fireProjectile(Actor *actor) {
    if (projectileActive) {
        return;
    }

    projectileDir = actor->dir;
    projectileX = actor->x + (actor->dir ? 10 : -2);
    projectileY = actor->y + 5;
    projectileActive = 1;
    move_sprite(PROJECTILE_SPRITE, projectileX, projectileY);
}

void updateProjectile(void) {
    if (!projectileActive) {
        return;
    }

    if (projectileDir) {
        projectileX += 2;
    } else {
        projectileX -= 2;
    }

    if (projectileX > 160 || projectileX < 8) {
        hideProjectile();
        return;
    }

    move_sprite(PROJECTILE_SPRITE, projectileX, projectileY);
}

void drawActor(Actor *actor, UINT8 spriteId, UINT8 tileIndex) {
    set_sprite_tile(spriteId, tileIndex);
    move_sprite(spriteId, actor->x, actor->y);
}

void updateEnemy(Actor *player, Actor *enemy) {
    if (enemy->x < player->x) {
        enemy->x += 1;
        enemy->dir = 1;
    } else if (enemy->x > player->x) {
        enemy->x -= 1;
        enemy->dir = 0;
    }

    if (enemy->x < 18) {
        enemy->x = 18;
    }
    if (enemy->x > 145) {
        enemy->x = 145;
    }

    if (player->y < enemy->y) {
        enemy->y -= 1;
    } else if (player->y > enemy->y) {
        enemy->y += 1;
    }

    drawActor(enemy, ENEMY_SPRITE, 1);
}

void updatePlayer(Actor *player, UINT8 joy) {
    if (joy & J_LEFT) {
        if (player->x > 12) {
            player->x -= 2;
        }
        player->dir = 0;
    }

    if (joy & J_RIGHT) {
        if (player->x < 148) {
            player->x += 2;
        }
        player->dir = 1;
    }

    if ((joy & J_B) && !player->jumping) {
        player->jumping = 1;
        player->jumpTimer = 12;
    }

    if (player->jumping) {
        if (player->jumpTimer > 0) {
            player->y -= 2;
            player->jumpTimer--;
        } else {
            player->y += 2;
            if (player->y >= GROUND_Y) {
                player->y = GROUND_Y;
                player->jumping = 0;
            }
        }
    }

    if ((joy & J_A) && player->attackCooldown == 0) {
        fireProjectile(player);
        player->attackCooldown = 12;
    }

    if (player->attackCooldown > 0) {
        player->attackCooldown--;
    }
}

void titleScreen(void) {
    set_bkg_data(0, 3, bgTiles);
    set_bkg_tiles(0, 0, 20, 18, arenaMap);
    SHOW_BKG;
    SHOW_SPRITES;
}

void main(void) {
    Actor player = { 20, GROUND_Y, 8, 8, 1, MAX_HEALTH, 0, 0, 0 };
    Actor enemy = { 120, GROUND_Y, 8, 8, 0, MAX_HEALTH, 0, 0, 0 };
    UINT8 joy;
    UINT8 currentState = TITLE_STATE;

    DISPLAY_ON;
    SHOW_SPRITES;
    SHOW_BKG;
    BGP_REG = 0xE4;
    OBP0_REG = 0xD2;

    initArena();
    set_sprite_data(0, 3, spriteTiles);

    drawActor(&player, PLAYER_SPRITE, 0);
    drawActor(&enemy, ENEMY_SPRITE, 1);
    hideProjectile();

    while (1) {
        wait_vbl_done();
        joy = joypad();
        frameCounter++;

        if (currentState == TITLE_STATE) {
            titleScreen();
            if (joy & J_START) {
                resetRound(&player, &enemy);
                currentState = PLAYING_STATE;
            }
            continue;
        }

        if (currentState == PLAYING_STATE) {
            updatePlayer(&player, joy);
            updateProjectile();

            if (projectileActive && projectileX > enemy.x && projectileX < enemy.x + 8 &&
                projectileY > enemy.y && projectileY < enemy.y + 8) {
                enemy.health--;
                hideProjectile();
            }

            if (frameCounter % 3 == 0) {
                updateEnemy(&player, &enemy);
            }

            if (player.x + 8 > enemy.x && player.x < enemy.x + 8 &&
                player.y + 8 > enemy.y && player.y < enemy.y + 8) {
                player.health--;
                player.x = 20;
                player.y = GROUND_Y;
                if (player.health == 0) {
                    currentState = LOSE_STATE;
                }
            }

            if (enemy.health == 0) {
                currentState = WIN_STATE;
            }

            drawActor(&player, PLAYER_SPRITE, 0);
            drawActor(&enemy, ENEMY_SPRITE, 1);

            if (currentState == WIN_STATE || currentState == LOSE_STATE) {
                if (joy & J_START) {
                    resetRound(&player, &enemy);
                    currentState = PLAYING_STATE;
                }
            }
        }

        if (currentState == WIN_STATE || currentState == LOSE_STATE) {
            if (joy & J_START) {
                resetRound(&player, &enemy);
                currentState = PLAYING_STATE;
            }
        }
    }
}
