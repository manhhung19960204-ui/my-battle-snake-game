#include "game_logic.h"
#include "app_data.h"
#include <stdlib.h>

game_state_t game;

static void spawn_food();
static bool check_wall(snake_t* s);
static bool check_self(snake_t* s);
static bool check_hit(snake_t* a, snake_t* b); // a đâm vào b
static void move_snake(snake_t* s);
static void grow_snake(snake_t* s);
static void ai_move(void);

// ============ INIT ============
void game_init(void) {
    // Reset player
    game.player.length = 3;
    game.player.dir    = DIR_RIGHT;
    game.player.score  = 0;
    game.player.alive  = true;
    game.player.body[0] = {5, 5};
    game.player.body[1] = {4, 5};
    game.player.body[2] = {3, 5};

    // Reset COM 
    if (app_data.mode == GAME_MODE_COM) {
        game.com.length = 3;
        game.com.dir    = DIR_LEFT;
        game.com.score  = 0;
        game.com.alive  = true;
        game.com.body[0] = {14, 5};
        game.com.body[1] = {15, 5};
        game.com.body[2] = {16, 5};
    }

    game.tick     = 0;
    game.max_tick = 300; 
    game.running  = true;

    spawn_food();
}

// ============ TICK ============
void game_tick(void) {
    if (!game.running) return;

    // AI tính nước đi
    if (app_data.mode == GAME_MODE_COM && game.com.alive) {
        ai_move();
    }

    // Di chuyển
    if (game.player.alive) move_snake(&game.player);
    if (app_data.mode == GAME_MODE_COM && game.com.alive) {
        move_snake(&game.com);
    }

    // Kiểm tra va chạm player
    if (check_wall(&game.player) || check_self(&game.player)) {
        game.player.alive = false;
    }

    // Kiểm tra va chạm COM
    if (app_data.mode == GAME_MODE_COM) {
        if (check_wall(&game.com) || check_self(&game.com)) {
            game.com.alive = false;
        }
        // 2 rắn đâm nhau
        if (check_hit(&game.player, &game.com)) game.player.alive = false;
        if (check_hit(&game.com, &game.player)) game.com.alive = false;
    }

    // Ăn mồi
    if (game.player.alive &&
        game.player.body[0].x == game.food.x &&
        game.player.body[0].y == game.food.y) {
        game.player.score++;
        grow_snake(&game.player);
        spawn_food();
    }

    if (app_data.mode == GAME_MODE_COM && game.com.alive &&
        game.com.body[0].x == game.food.x &&
        game.com.body[0].y == game.food.y) {
        game.com.score++;
        grow_snake(&game.com);
        spawn_food();
    }

    // Đếm thời gian
    if (app_data.mode == GAME_MODE_COM) {
        game.tick++;
        if (game.tick >= game.max_tick) {
            game.running = false;
        }
    }

    // Nếu chơi đơn mà player chết
    if (app_data.mode == GAME_MODE_SINGLE && !game.player.alive) {
        game.running = false;
    }

    // Nếu chơi với COM, chỉ cần 1 trong 2 con chết -> Kết thúc game ngay
    if (app_data.mode == GAME_MODE_COM) {
        if (!game.player.alive || !game.com.alive) {
            game.running = false;
        }
    }
}

// ============ HELPER ============
void game_player_turn(uint8_t dir) {
    // Không cho quay 180 độ
    if (dir == DIR_UP    && game.player.dir == DIR_DOWN)  return;
    if (dir == DIR_DOWN  && game.player.dir == DIR_UP)    return;
    if (dir == DIR_LEFT  && game.player.dir == DIR_RIGHT) return;
    if (dir == DIR_RIGHT && game.player.dir == DIR_LEFT)  return;
    game.player.dir = dir;
}

bool game_is_over(void) {
    return !game.running;
}

uint8_t game_get_winner(void) {
    if (app_data.mode == GAME_MODE_SINGLE) return 0; // player
    if (!game.player.alive && game.com.alive) return 1; // COM
    if (game.player.alive && !game.com.alive) return 0; // player
    // Cả 2 còn sống → so điểm
    if (game.player.score > game.com.score) return 0;
    if (game.com.score > game.player.score) return 1;
    return 2; // draw
}

// ============ PRIVATE ============
static void spawn_food(void) {
    game.food.x = rand() % GRID_W;
    game.food.y = rand() % GRID_H;
}

static void move_snake(snake_t* s) {
    // Dịch thân
    for (int i = s->length - 1; i > 0; i--) {
        s->body[i] = s->body[i-1];
    }
    // Di chuyển đầu
    switch (s->dir) {
        case DIR_UP:    s->body[0].y--; break;
        case DIR_DOWN:  s->body[0].y++; break;
        case DIR_LEFT:  s->body[0].x--; break;
        case DIR_RIGHT: s->body[0].x++; break;
    }
}

static void grow_snake(snake_t* s) {
    if (s->length < SNAKE_MAX_LEN) {
        s->body[s->length] = s->body[s->length - 1];
        s->length++;
    }
}

static bool check_wall(snake_t* s) {
    return (s->body[0].x < 0 || s->body[0].x >= GRID_W ||
            s->body[0].y < 0 || s->body[0].y >= GRID_H);
}

static bool check_self(snake_t* s) {
    for (int i = 1; i < s->length; i++) {
        if (s->body[0].x == s->body[i].x &&
            s->body[0].y == s->body[i].y) return true;
    }
    return false;
}

static bool check_hit(snake_t* a, snake_t* b) {
    // Đầu a đâm vào thân b
    for (int i = 0; i < b->length; i++) {
        if (a->body[0].x == b->body[i].x &&
            a->body[0].y == b->body[i].y) return true;
    }
    return false;
}

static void ai_move(void) {
    point_t head = game.com.body[0];
    // Đi về phía food
    if (game.food.x > head.x)      game.com.dir = DIR_RIGHT;
    else if (game.food.x < head.x) game.com.dir = DIR_LEFT;
    else if (game.food.y > head.y) game.com.dir = DIR_DOWN;
    else                            game.com.dir = DIR_UP;
}