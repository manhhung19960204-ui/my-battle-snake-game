// game_logic.h
#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__

#include <stdint.h>
#include <stdbool.h>

#define GRID_W        20   // số ô ngang
#define GRID_H        10   // số ô dọc  
#define CELL_SIZE     6    // pixel mỗi ô
#define SNAKE_MAX_LEN 50

#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

typedef struct {
    int8_t x, y;
} point_t;

typedef struct {
    point_t body[SNAKE_MAX_LEN];
    uint8_t length;
    uint8_t dir;
    uint8_t score;
    bool alive;
} snake_t;

typedef struct {
    snake_t player;
    snake_t com;        // chỉ dùng ở mode COM
    point_t food;
    uint16_t tick;      // đếm thời gian
    uint16_t max_tick;  // khi hết giờ → kết thúc
    bool running;
} game_state_t;

extern game_state_t game;

// API
void game_init(void);
void game_tick(void);          // gọi mỗi timer tick
void game_player_turn(uint8_t dir);
bool game_is_over(void);
uint8_t game_get_winner(void); // 0=player, 1=COM, 2=draw

#endif