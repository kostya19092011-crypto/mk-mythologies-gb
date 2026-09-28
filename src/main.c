#include <gb/gb.h>
#include <stdint.h>

#define PLAYER_SPRITE 0
#define ENEMY_SPRITE 1
#define PROJECTILE_SPRITE 2
#define ARENA_W 20
#define ARENA_H 18
#define MAX_HEALTH 10

typedef struct {
    UINT8 x;
    UINT8 y;
    UINT8 dir;
    UINT8 health;
    UINT8 jumping;
    UINT8 jump_timer;
} Fighter;

UINT8 arena_map[ARENA_W * ARENA_H];
UINT8 projectile_x;
UINT8 projectile_y;
UINT8 projectile_active;
UINT8 projectile_dir;
UINT8 win_flag;

const UINT8 sprite_tiles[3 * 16] = {
    /* Hero sprite */
    0x00, 0x00, 0x00, 0x00, 0x3C, 0x3C, 0x7E, 0x7E,
    0x7E, 0x18, 0x3C, 0x3C, 0x3C, 0x18, 0x18, 0x00,

    /* Enemy sprite */
    0x00, 0x00, 0x00, 0x00, 0x3C, 0x3C, 0x1E, 0x7E,
    0x3C, 0x18, 0x3C, 0x1E, 0x1E, 0x18, 0x18, 0x00,

    /* Ice shard */
    0x00, 0x00, 0x04, 0x0E, 0x1F, 0x0E, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x0E, 0x1F, 0x0E, 0x04
};

const UINT8 bg_tiles[3 * 16] = {
    /* tile 0: empty / sky */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* tile 1: ground */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    /* tile 2: temple stone */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55
};

void init_arena(void) {
    UINT8 x;
    UINT8 y;

    for (y = 0; y < ARENA_H; ++y) {
        for (x = 0; x < ARENA_W; ++x) {
            if (y < 12) {
                arena_map[y * ARENA_W + x] = 2;
            } else if (y < 17) {
                arena_map[y * ARENA_W + x] = 1;
            } else {
                arena_map[y * ARENA_W + x] = 0;
            }
        }
    }

    set_bkg_data(0, 3, bg_tiles);
    set_bkg_tiles(0, 0, ARENA_W, ARENA_H, arena_map);
}

void hide_projectile(void) {
    projectile_active = 0;
    move_sprite(PROJECTILE_SPRITE, 0, 0);
}

void fire_projectile(Fighter *fighter) {
    if (projectile_active) {
        return;
    }

    projectile_dir = fighter->dir;
    projectile_x = fighter->x + (fighter->dir ? 10 : -2);
    projectile_y = fighter->y + 5;
    projectile_active = 1;
    move_sprite(PROJECTILE_SPRITE, projectile_x, projectile_y);
}

void update_projectile(void) {
    if (!projectile_active) {
        return;
    }

    if (projectile_dir) {
        projectile_x += 2;
    } else {
        projectile_x -= 2;
    }

    if (projectile_x > 160 || projectile_x < 8) {
        hide_projectile();
        return;
    }

    move_sprite(PROJECTILE_SPRITE, projectile_x, projectile_y);
}

void draw_fighter(Fighter *fighter, UINT8 sprite_id, UINT8 tile_index) {
    set_sprite_tile(sprite_id, tile_index);
    move_sprite(sprite_id, fighter->x, fighter->y);
}

void update_enemy(Fighter *player, Fighter *enemy) {
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

    draw_fighter(enemy, ENEMY_SPRITE, 1);
}

void main(void) {
    Fighter player = { 20, 96, 1, MAX_HEALTH, 0, 0 };
    Fighter enemy = { 120, 96, 0, MAX_HEALTH, 0, 0 };
    UINT8 joy;
    UINT8 frame = 0;
    UINT8 player_cooldown = 0;

    DISPLAY_ON;
    SHOW_SPRITES;
    SHOW_BKG;

    BGP_REG = 0xE4;
    OBP0_REG = 0xD2;

    init_arena();
    set_sprite_data(0, 3, sprite_tiles);

    draw_fighter(&player, PLAYER_SPRITE, 0);
    draw_fighter(&enemy, ENEMY_SPRITE, 1);
    hide_projectile();

    while (1) {
        wait_vbl_done();
        joy = joypad();
        frame++;

        if (joy & J_LEFT) {
            player.x = (player.x > 12) ? player.x - 2 : player.x;
            player.dir = 0;
        }
        if (joy & J_RIGHT) {
            player.x = (player.x < 148) ? player.x + 2 : player.x;
            player.dir = 1;
        }

        if ((joy & J_B) && !player.jumping) {
            player.jumping = 1;
            player.jump_timer = 12;
        }

        if (player.jumping) {
            if (player.jump_timer > 0) {
                player.y -= 2;
                player.jump_timer--;
            } else {
                player.y += 2;
                if (player.y >= 96) {
                    player.y = 96;
                    player.jumping = 0;
                }
            }
        }

        if ((joy & J_A) && player_cooldown == 0) {
            fire_projectile(&player);
            player_cooldown = 12;
        }

        if (player_cooldown > 0) {
            player_cooldown--;
        }

        update_projectile();

        if (projectile_active && projectile_x > enemy.x && projectile_x < enemy.x + 8 && projectile_y > enemy.y && projectile_y < enemy.y + 8) {
            enemy.health--;
            hide_projectile();
        }

        if (enemy.health == 0) {
            win_flag = 1;
            break;
        }

        if (frame % 3 == 0) {
            update_enemy(&player, &enemy);
        }

        if (player.x + 8 > enemy.x && player.x < enemy.x + 8 && player.y + 8 > enemy.y && player.y < enemy.y + 8) {
            player.health--;
            player.x = 20;
            player.y = 96;
            if (player.health == 0) {
                break;
            }
        }

        draw_fighter(&player, PLAYER_SPRITE, 0);
    }

    while (1) {
        wait_vbl_done();
    }
}

































































































































































































































































































































































































































































































































































