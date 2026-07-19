// game_logic.h
#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__

#include <stdint.h>
#include <stdbool.h>

#define GRID_W        20   // số ô ngang
#define GRID_H        10   // số ô dọc  
#define CELL_SIZE     6    // pixel mỗi ô
#define SNAKE_MAX_LEN 50
#define FOOD_MAX      3

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
    snake_t com;        
    snake_t  obstacle;
    point_t  food[FOOD_MAX]; 
    uint8_t  food_count;     
    bool     has_obstacle;  // level 2+: có snake cản
    bool     has_wall;      // level 3: có tường (chết khi ra biên) 
    uint16_t tick;
    uint16_t max_tick;  
    bool running;
} game_state_t;

extern game_state_t game;

void game_init(void);
void game_tick(void);          
void game_player_turn(uint8_t dir);
bool game_is_over(void);
uint8_t game_get_winner(void); 

#endif