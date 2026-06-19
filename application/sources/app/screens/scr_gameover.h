#ifndef SCR_GAMEOVER_H
#define SCR_GAMEOVER_H
 
#include "screen_manager.h"
 
// Kết quả cuối ván, được set trước khi SCREEN_GOTO sang màn hình này
typedef enum {
    GAME_RESULT_WIN = 0,   // Player thắng 
    GAME_RESULT_LOSE,      // Player thua (chết / điểm thấp hơn COM)
    GAME_RESULT_DRAW,      // Hòa điểm (chỉ áp dụng mode COM)
} game_result_t;
 
extern view_screen_t scr_gameover;
 
void gameover_set_result(game_result_t result, uint16_t player_score, uint16_t com_score);
 
void scr_gameover_handle(ak_msg_t *msg);
 
#endif 