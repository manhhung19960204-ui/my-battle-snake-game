#include "game_logic.h"
#include "app_data.h"
#include <stdlib.h>

game_state_t game;

static void    spawn_food(int slot);
static void    spawn_all_food(void);
static bool    is_occupied(int8_t x, int8_t y);
static bool    check_wall(snake_t* s);
static bool    check_self(snake_t* s);
static bool    check_hit(snake_t* a, snake_t* b);
static void    move_snake(snake_t* s);
static void    grow_snake(snake_t* s);
static void    try_eat_food(snake_t* s);
static void    ai_move_snake(snake_t* s, point_t target);
static void    com_ai(void);
static void    obstacle_ai(void);

void game_init(void) {
    // 1. Reset trạng thái cơ bản ban đầu
    game.obstacle.alive = false;
    game.obstacle.length = 0;

    game.tick     = 0;
    game.max_tick = 300;
    game.running  = true;

    // 2. Thiết lập độ khó: NẾU LÀ CHẾ ĐỘ COM THÌ MỚI ÁP DỤNG 3 CẤP ĐỘ SETTING
    if (app_data.mode == GAME_MODE_COM) {
        uint8_t diff = (uint8_t)app_data.difficulty;
        game.food_count   = (diff >= 1) ? 2 : 1;
        game.has_obstacle = (diff >= 2);
        game.has_wall     = (diff >= 3);
    } else {
        // Cấu hình mặc định cho chế độ Single (ví dụ: 1 mồi, không vật cản, xuyên tường)
        game.food_count   = 1;
        game.has_obstacle = false;
        game.has_wall     = false; 
    }

    // 3. Khởi tạo Player
    game.player.length  = 3;
    game.player.dir     = DIR_RIGHT;
    game.player.score   = 0;
    game.player.alive   = true;
    game.player.body[0] = {3, 5};
    game.player.body[1] = {2, 5};
    game.player.body[2] = {1, 5};

    // 4. Khởi tạo COM (nếu đúng mode)
    if (app_data.mode == GAME_MODE_COM) {
        game.com.length  = 3;
        game.com.dir     = DIR_LEFT;
        game.com.score   = 0;
        game.com.alive   = true;
        game.com.body[0] = {16, 5};
        game.com.body[1] = {17, 5};
        game.com.body[2] = {18, 5};
    }

    // 5. Khởi tạo Obstacle (nếu game có bật obstacle)
    if (game.has_obstacle) {
        game.obstacle.length  = 4;
        game.obstacle.dir     = DIR_DOWN;
        game.obstacle.score   = 0;
        game.obstacle.alive   = true;
        game.obstacle.body[0] = {10, 1};
        game.obstacle.body[1] = {10, 2};
        game.obstacle.body[2] = {10, 3};
        game.obstacle.body[3] = {10, 4};
    }

    // 6. Sinh mồi
    spawn_all_food();
}

void game_tick(void) {
    if (!game.running) return;

    if (app_data.mode == GAME_MODE_COM && game.com.alive) {
        com_ai();
    }
    if (game.has_obstacle && game.obstacle.alive) {
        obstacle_ai();
    }

    // Di chuyển
    if (game.player.alive) move_snake(&game.player);
    if (app_data.mode == GAME_MODE_COM && game.com.alive) {
        move_snake(&game.com);
    }
    if (game.has_obstacle && game.obstacle.alive) {
        move_snake(&game.obstacle);
    }

    // Player đụng tường hoặc tự cắn
    if (check_wall(&game.player) || check_self(&game.player)) {
        game.player.alive = false;
        game.running = false;
        return;
    }

    if (app_data.mode == GAME_MODE_COM) {
        // Player đụng thân COM → chết
        if (check_hit(&game.player, &game.com)) {
            game.player.alive = false;
            game.running = false;
            return;
        }

        // COM đụng tường/tự cắn → hồi sinh
        if (check_wall(&game.com) || check_self(&game.com)) {
            game.com.length   = 3;
            game.com.dir      = DIR_LEFT;
            game.com.body[0]  = {16, 5};
            game.com.body[1]  = {17, 5};
            game.com.body[2]  = {18, 5};
            game.com.alive    = true;
        }

        // COM đụng player (đụng thân/đuôi của player)
        if (check_hit(&game.com, &game.player)) {
            
            bool is_multi_snake_setting = (app_data.difficulty >= 2); // Ví dụ: Setting cấp 2 hoặc 3 bật chế độ nhiều rắn/đặc biệt

            if (is_multi_snake_setting) {
                game.com.alive = false; 
            } else {
                game.com.alive = false;
                game.running = false; // Dừng game để hàm game_is_over() trả về true và hiển thị You Win
            }
        }
    }

    // Player đụng obstacle → chết
    if (game.has_obstacle && game.obstacle.alive) {
        if (check_hit(&game.player, &game.obstacle)) {
            game.player.alive = false;
            game.running = false;
            return;
        }

        // Obstacle đụng tường → hồi sinh
        if (check_wall(&game.obstacle) || check_self(&game.obstacle)) {
            game.obstacle.body[0] = {10, 1};
            game.obstacle.body[1] = {10, 2};
            game.obstacle.body[2] = {10, 3};
            game.obstacle.body[3] = {10, 4};
            game.obstacle.dir = DIR_DOWN;
        }
    }

    // Ăn mồi
    if (game.player.alive) try_eat_food(&game.player);
    if (app_data.mode == GAME_MODE_COM && game.com.alive) try_eat_food(&game.com);

    // Đếm thời gian
    if (app_data.mode == GAME_MODE_COM) {
        game.tick++;
        if (game.tick >= game.max_tick) {
            game.running = false;
        }
    }

    // Single mode: player chết → game over
    if (app_data.mode == GAME_MODE_SINGLE && !game.player.alive) {
        game.running = false;
    }

    // if (!game.running) return;

    // if (app_data.mode == GAME_MODE_COM && game.com.alive) com_ai();
    // if (game.has_obstacle && game.obstacle.alive) obstacle_ai();

    // if (game.player.alive) move_snake(&game.player);
    // if (app_data.mode == GAME_MODE_COM && game.com.alive) move_snake(&game.com);
    // if (game.has_obstacle && game.obstacle.alive) move_snake(&game.obstacle);

    // // Va chạm player
    // if (game.player.alive) {
    //     bool dead = check_self(&game.player);
    //     if (!dead && game.has_wall)  dead = check_wall(&game.player);
    //     if (!dead && game.has_obstacle && game.obstacle.alive)
    //         dead = check_hit(&game.player, &game.obstacle);
    //     if (!dead && app_data.mode == GAME_MODE_COM && game.com.alive)
    //         dead = check_hit(&game.player, &game.com);
    //     if (dead) game.player.alive = false;
    // }

    // // Va chạm COM
    // if (app_data.mode == GAME_MODE_COM && game.com.alive) {
    //     bool dead = check_self(&game.com);
    //     if (!dead && game.has_wall)  dead = check_wall(&game.com);
    //     if (!dead && game.has_obstacle && game.obstacle.alive)
    //         dead = check_hit(&game.com, &game.obstacle);
    //     if (!dead && game.player.alive)
    //         dead = check_hit(&game.com, &game.player);
    //     if (dead) game.com.alive = false;
    // }

    // // Va chạm obstacle -> hồi sinh
    // if (game.has_obstacle && game.obstacle.alive) {
    //     bool dead = check_self(&game.obstacle);
    //     if (!dead && game.has_wall) dead = check_wall(&game.obstacle);
    //     if (dead) {
    //         game.obstacle.body[0] = {10, 1};
    //         game.obstacle.body[1] = {10, 2};
    //         game.obstacle.body[2] = {10, 3};
    //         game.obstacle.body[3] = {10, 4};
    //         game.obstacle.dir = DIR_DOWN;
    //     }
    // }

    // if (game.player.alive) try_eat_food(&game.player);
    // if (app_data.mode == GAME_MODE_COM && game.com.alive) try_eat_food(&game.com);

    // if (app_data.mode == GAME_MODE_SINGLE) {
    //     if (!game.player.alive) game.running = false;
    // } else {
    //     game.tick++;
    //     if (game.tick >= game.max_tick) game.running = false;
    //     if (!game.player.alive || !game.com.alive) game.running = false;
    // }
}

void game_player_turn(uint8_t dir) {
    if (dir == DIR_UP    && game.player.dir == DIR_DOWN)  return;
    if (dir == DIR_DOWN  && game.player.dir == DIR_UP)    return;
    if (dir == DIR_LEFT  && game.player.dir == DIR_RIGHT) return;
    if (dir == DIR_RIGHT && game.player.dir == DIR_LEFT)  return;
    game.player.dir = dir;
}

bool game_is_over(void) { return !game.running; }

uint8_t game_get_winner(void) {
    if (app_data.mode == GAME_MODE_SINGLE) return 0;
    if (!game.player.alive && game.com.alive)  return 1;
    if (game.player.alive  && !game.com.alive) return 0;
    if (game.player.score > game.com.score) return 0;
    if (game.com.score > game.player.score) return 1;
    return 2;
}

static void move_snake(snake_t* s) {
    for (int i = s->length - 1; i > 0; i--) s->body[i] = s->body[i-1];
    switch (s->dir) {
        case DIR_UP:    s->body[0].y--; break;
        case DIR_DOWN:  s->body[0].y++; break;
        case DIR_LEFT:  s->body[0].x--; break;
        case DIR_RIGHT: s->body[0].x++; break;
    }
    if (!game.has_wall) {
        if (s->body[0].x < 0)       s->body[0].x = GRID_W - 1;
        if (s->body[0].x >= GRID_W) s->body[0].x = 0;
        if (s->body[0].y < 0)       s->body[0].y = GRID_H - 1;
        if (s->body[0].y >= GRID_H) s->body[0].y = 0;
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
    for (int i = 1; i < s->length; i++)
        if (s->body[0].x == s->body[i].x && s->body[0].y == s->body[i].y) return true;
    return false;
}

static bool check_hit(snake_t* a, snake_t* b) {
    for (int i = 0; i < b->length; i++)
        if (a->body[0].x == b->body[i].x && a->body[0].y == b->body[i].y) return true;
    return false;
}

static bool is_occupied(int8_t x, int8_t y) {
    for (int i = 0; i < game.player.length; i++)
        if (game.player.body[i].x == x && game.player.body[i].y == y) return true;
    if (app_data.mode == GAME_MODE_COM)
        for (int i = 0; i < game.com.length; i++)
            if (game.com.body[i].x == x && game.com.body[i].y == y) return true;
    if (game.has_obstacle)
        for (int i = 0; i < game.obstacle.length; i++)
            if (game.obstacle.body[i].x == x && game.obstacle.body[i].y == y) return true;
    return false;
}

static void spawn_food(int slot) {
    int8_t nx, ny;
    bool valid;
    int tries = 0;
    do {
        valid = true;
        nx = rand() % GRID_W;
        ny = rand() % GRID_H;
        if (is_occupied(nx, ny)) { valid = false; continue; }
        for (int f = 0; f < game.food_count; f++) {
            if (f == slot) continue;
            if (game.food[f].x == nx && game.food[f].y == ny) { valid = false; break; }
        }
        tries++;
    } while (!valid && tries < 100);
    game.food[slot] = {nx, ny};
}

static void spawn_all_food(void) {
    for (int f = 0; f < game.food_count; f++) spawn_food(f);
}

static void try_eat_food(snake_t* s) {
    for (int f = 0; f < game.food_count; f++) {
        if (s->body[0].x == game.food[f].x && s->body[0].y == game.food[f].y) {
            s->score++;
            grow_snake(s);
            spawn_food(f);
        }
    }
}

static void ai_move_snake(snake_t* s, point_t target) {
    point_t head = s->body[0];
    uint8_t current_dir = s->dir;
    int dx = target.x - head.x;
    int dy = target.y - head.y;
    if (!game.has_wall) {
        if (abs(dx) > GRID_W / 2) dx = (dx > 0) ? dx - GRID_W : dx + GRID_W;
        if (abs(dy) > GRID_H / 2) dy = (dy > 0) ? dy - GRID_H : dy + GRID_H;
    }
    uint8_t preferred = (abs(dx) >= abs(dy))
        ? ((dx > 0) ? DIR_RIGHT : DIR_LEFT)
        : ((dy > 0) ? DIR_DOWN  : DIR_UP);

    auto is_safe = [&](uint8_t dir) -> bool {
        point_t n = head;
        switch (dir) {
            case DIR_UP:    n.y--; break;
            case DIR_DOWN:  n.y++; break;
            case DIR_LEFT:  n.x--; break;
            case DIR_RIGHT: n.x++; break;
        }
        if (!game.has_wall) {
            if (n.x < 0)       n.x = GRID_W - 1;
            if (n.x >= GRID_W) n.x = 0;
            if (n.y < 0)       n.y = GRID_H - 1;
            if (n.y >= GRID_H) n.y = 0;
        } else {
            if (n.x < 0 || n.x >= GRID_W || n.y < 0 || n.y >= GRID_H) return false;
        }
        for (int i = 0; i < s->length; i++)
            if (n.x == s->body[i].x && n.y == s->body[i].y) return false;
        return true;
    };

    uint8_t all_dirs[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    uint8_t dirs[4];
    dirs[0] = preferred;
    int idx = 1;
    for (int i = 0; i < 4; i++)
        if (all_dirs[i] != preferred) dirs[idx++] = all_dirs[i];

    for (int i = 0; i < 4; i++) {
        if (dirs[i] == DIR_UP    && current_dir == DIR_DOWN)  continue;
        if (dirs[i] == DIR_DOWN  && current_dir == DIR_UP)    continue;
        if (dirs[i] == DIR_LEFT  && current_dir == DIR_RIGHT) continue;
        if (dirs[i] == DIR_RIGHT && current_dir == DIR_LEFT)  continue;
        if (is_safe(dirs[i])) { s->dir = dirs[i]; return; }
    }
}

static void com_ai(void) {
    point_t head = game.com.body[0];
    int best = 0;
    int best_dist = abs(game.food[0].x - head.x) + abs(game.food[0].y - head.y);
    for (int f = 1; f < game.food_count; f++) {
        int d = abs(game.food[f].x - head.x) + abs(game.food[f].y - head.y);
        if (d < best_dist) { best_dist = d; best = f; }
    }
    ai_move_snake(&game.com, game.food[best]);
}

static void obstacle_ai(void) {
    static uint8_t patrol_tick = 0;
    patrol_tick++;
    if (patrol_tick >= (GRID_H - 2)) {
        patrol_tick = 0;
        game.obstacle.dir = (game.obstacle.dir == DIR_DOWN) ? DIR_UP : DIR_DOWN;
    }
    if (game.has_wall) {
        if (game.obstacle.body[0].y <= 0)        game.obstacle.dir = DIR_DOWN;
        if (game.obstacle.body[0].y >= GRID_H-1) game.obstacle.dir = DIR_UP;
    }
}
