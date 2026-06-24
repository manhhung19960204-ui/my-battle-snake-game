#include "game_logic.h"
#include "app_data.h"
#include <stdlib.h>

game_state_t game;

// ============ PRIVATE PROTOTYPES ============
static void    spawn_food(int slot);
static void    spawn_all_food(void);
static bool    is_on_snake(int8_t x, int8_t y);
static bool    check_wall(snake_t* s);
static bool    check_self(snake_t* s);
static bool    check_hit(snake_t* a, snake_t* b);
static void    move_snake(snake_t* s);
static void    grow_snake(snake_t* s);
static void    ai_move(void);

// ============ INIT ============
void game_init(void) {
    // food_count = difficulty (1/2/3 mồi)
    game.food_count = (uint8_t)app_data.difficulty;
    if (game.food_count < 1 || game.food_count > FOOD_MAX) {
        game.food_count = 1; // an toàn nếu chưa set
    }

    // Reset player
    game.player.length   = 3;
    game.player.dir      = DIR_RIGHT;
    game.player.score    = 0;
    game.player.alive    = true;
    game.player.body[0]  = {5, 5};
    game.player.body[1]  = {4, 5};
    game.player.body[2]  = {3, 5};

    // Reset COM
    if (app_data.mode == GAME_MODE_COM) {
        game.com.length  = 3;
        game.com.dir     = DIR_LEFT;
        game.com.score   = 0;
        game.com.alive   = true;
        game.com.body[0] = {14, 5};
        game.com.body[1] = {15, 5};
        game.com.body[2] = {16, 5};
    }

    game.tick     = 0;
    game.max_tick = 300;
    game.running  = true;

    spawn_all_food();
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
    if (game.player.alive &&
        (check_wall(&game.player) || check_self(&game.player))) {
        game.player.alive = false;
    }

    // Kiểm tra va chạm COM
    if (app_data.mode == GAME_MODE_COM && game.com.alive) {
        if (check_wall(&game.com) || check_self(&game.com)) {
            game.com.alive = false;
        }
        if (game.player.alive && check_hit(&game.player, &game.com)) {
            game.player.alive = false;
        }
        if (game.com.alive && check_hit(&game.com, &game.player)) {
            game.com.alive = false;
        }
    }

    // Ăn mồi: kiểm tra từng slot
    for (int f = 0; f < game.food_count; f++) {
        // Player ăn mồi
        if (game.player.alive &&
            game.player.body[0].x == game.food[f].x &&
            game.player.body[0].y == game.food[f].y) {
            game.player.score++;
            grow_snake(&game.player);
            spawn_food(f); // sinh lại đúng slot này
        }
        // COM ăn mồi
        if (app_data.mode == GAME_MODE_COM && game.com.alive &&
            game.com.body[0].x == game.food[f].x &&
            game.com.body[0].y == game.food[f].y) {
            game.com.score++;
            grow_snake(&game.com);
            spawn_food(f);
        }
    }

    // Kết thúc game
    if (app_data.mode == GAME_MODE_SINGLE) {
        if (!game.player.alive) game.running = false;
    } else {
        game.tick++;
        if (game.tick >= game.max_tick) {
            game.running = false; // hết giờ → so điểm
        }
        if (!game.player.alive || !game.com.alive) {
            game.running = false;
        }
    }
}

// ============ HELPER PUBLIC ============
void game_player_turn(uint8_t dir) {
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
    if (app_data.mode == GAME_MODE_SINGLE) return 0; // player "thắng" (sống sót)
    if (!game.player.alive && game.com.alive)  return 1; // COM thắng
    if (game.player.alive  && !game.com.alive) return 0; // player thắng
    // Hết giờ hoặc cả 2 chết → so điểm
    if (game.player.score > game.com.score) return 0;
    if (game.com.score > game.player.score) return 1;
    return 2; // hòa
}

// ============ PRIVATE ============

// Kiểm tra ô (x,y) có trùng thân rắn nào không
static bool is_on_snake(int8_t x, int8_t y) {
    for (int i = 0; i < game.player.length; i++) {
        if (game.player.body[i].x == x && game.player.body[i].y == y) return true;
    }
    if (app_data.mode == GAME_MODE_COM) {
        for (int i = 0; i < game.com.length; i++) {
            if (game.com.body[i].x == x && game.com.body[i].y == y) return true;
        }
    }
    return false;
}

// Sinh mồi cho 1 slot cụ thể, không trùng thân rắn và không trùng các slot khác
static void spawn_food(int slot) {
    int8_t nx, ny;
    bool valid;
    int tries = 0;
    do {
        valid = true;
        nx = rand() % GRID_W;
        ny = rand() % GRID_H;
        // Không trùng thân rắn
        if (is_on_snake(nx, ny)) { valid = false; continue; }
        // Không trùng các slot mồi khác
        for (int f = 0; f < game.food_count; f++) {
            if (f == slot) continue;
            if (game.food[f].x == nx && game.food[f].y == ny) {
                valid = false;
                break;
            }
        }
        tries++;
    } while (!valid && tries < 100); // tránh vòng lặp vô hạn khi bàn đầy
    game.food[slot].x = nx;
    game.food[slot].y = ny;
}

// Sinh tất cả mồi lúc khởi tạo
static void spawn_all_food(void) {
    for (int f = 0; f < game.food_count; f++) {
        spawn_food(f);
    }
}

static void move_snake(snake_t* s) {
    for (int i = s->length - 1; i > 0; i--) {
        s->body[i] = s->body[i-1];
    }
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
    for (int i = 0; i < b->length; i++) {
        if (a->body[0].x == b->body[i].x &&
            a->body[0].y == b->body[i].y) return true;
    }
    return false;
}

// AI: đi về mồi gần nhất, tránh tường và thân mình
static void ai_move(void) {
    point_t head = game.com.body[0];

    // Tìm mồi gần nhất (Manhattan)
    int best = 0;
    int best_dist = abs(game.food[0].x - head.x) + abs(game.food[0].y - head.y);
    for (int f = 1; f < game.food_count; f++) {
        int d = abs(game.food[f].x - head.x) + abs(game.food[f].y - head.y);
        if (d < best_dist) { best_dist = d; best = f; }
    }
    point_t target = game.food[best];

    // Hướng ưu tiên theo mồi gần nhất
    uint8_t preferred_dir;
    int dx = target.x - head.x;
    int dy = target.y - head.y;
    if (abs(dx) >= abs(dy)) {
        preferred_dir = (dx > 0) ? DIR_RIGHT : DIR_LEFT;
    } else {
        preferred_dir = (dy > 0) ? DIR_DOWN : DIR_UP;
    }

    // Kiểm tra hướng có an toàn không
    auto is_safe = [&](uint8_t dir) -> bool {
        point_t next = head;
        switch (dir) {
            case DIR_UP:    next.y--; break;
            case DIR_DOWN:  next.y++; break;
            case DIR_LEFT:  next.x--; break;
            case DIR_RIGHT: next.x++; break;
        }
        if (next.x < 0 || next.x >= GRID_W || next.y < 0 || next.y >= GRID_H) return false;
        for (int i = 0; i < game.com.length; i++) {
            if (next.x == game.com.body[i].x && next.y == game.com.body[i].y) return false;
        }
        return true;
    };

    // Thử preferred trước, rồi 3 hướng còn lại (không trùng, không quay 180°)
    uint8_t all_dirs[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    uint8_t dirs[4];
    dirs[0] = preferred_dir;
    int idx = 1;
    for (int i = 0; i < 4; i++) {
        if (all_dirs[i] != preferred_dir) dirs[idx++] = all_dirs[i];
    }

    uint8_t current_dir = game.com.dir;
    for (int i = 0; i < 4; i++) {
        if (dirs[i] == DIR_UP    && current_dir == DIR_DOWN)  continue;
        if (dirs[i] == DIR_DOWN  && current_dir == DIR_UP)    continue;
        if (dirs[i] == DIR_LEFT  && current_dir == DIR_RIGHT) continue;
        if (dirs[i] == DIR_RIGHT && current_dir == DIR_LEFT)  continue;
        if (is_safe(dirs[i])) { game.com.dir = dirs[i]; return; }
    }
}