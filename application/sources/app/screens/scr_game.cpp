#include "scr_game.h"
#include "scr_gameover.h"
#include "game_logic.h"
#include "screen_manager.h"
#include "app_data.h"
#include "app.h"
#include "timer.h"

#define GAME_TICK_INTERVAL  200

static void view_scr_game();

view_dynamic_t dyn_view_game = {
    { .item_type = ITEM_TYPE_DYNAMIC },
    view_scr_game
};

view_screen_t scr_game = {
    &dyn_view_game,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

// ============ RENDER ============
void view_scr_game() {
    view_render.clear();
    view_render.drawRect(0, 0, 119, 62, WHITE);

    // Vẽ thân rắn player
    for (int i = 0; i < game.player.length; i++) {
        int px = 1 + game.player.body[i].x * CELL_SIZE;
        int py = 1 + game.player.body[i].y * CELL_SIZE;
        if (i == 0) {
            view_render.fillRect(px, py, CELL_SIZE-1, CELL_SIZE-1, WHITE);
        } else {
            view_render.drawRect(px, py, CELL_SIZE-1, CELL_SIZE-1, WHITE);
        }
    }

    // Vẽ thân rắn COM
    if (app_data.mode == GAME_MODE_COM) {
        for (int i = 0; i < game.com.length; i++) {
            int cx = 1 + game.com.body[i].x * CELL_SIZE;
            int cy = 1 + game.com.body[i].y * CELL_SIZE;
            view_render.fillRect(cx+1, cy+1, CELL_SIZE-3, CELL_SIZE-3, WHITE);
        }
    }

    // Vẽ tất cả mồi (hỗ trợ nhiều mồi theo độ khó)
    for (int f = 0; f < game.food_count; f++) {
        int fx = 1 + game.food[f].x * CELL_SIZE;
        int fy = 1 + game.food[f].y * CELL_SIZE;
        view_render.fillRect(fx+1, fy,   CELL_SIZE-3, CELL_SIZE-1, WHITE);
        view_render.fillRect(fx,   fy+1, CELL_SIZE-1, CELL_SIZE-3, WHITE);
    }

    // Hiện điểm
    view_render.setTextColor(WHITE);
    view_render.setCursor(2, 2);
    view_render.print(game.player.score);
    if (app_data.mode == GAME_MODE_COM) {
        view_render.setCursor(90, 2);
        view_render.print(game.com.score);
    }
    view_render.update();
}

// ============ HANDLE ============
void scr_game_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY: {
        APP_DBG_SIG("SCREEN_ENTRY\n");
        game_init();
        timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_GAME_TICK, GAME_TICK_INTERVAL, TIMER_PERIODIC);
        view_scr_game();
    } break;
    case SCREEN_EXIT: {
        timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_GAME_TICK);
    } break;
    case AC_DISPLAY_GAME_TICK: {
        game_tick();
        if (game_is_over()) {
            uint8_t winner = game_get_winner();
            game_result_t result;
            if (winner == 0)      result = GAME_RESULT_WIN;
            else if (winner == 1) result = GAME_RESULT_LOSE;
            else                  result = GAME_RESULT_DRAW;
            gameover_set_result(result, game.player.score, game.com.score);
            SCREEN_TRAN(scr_gameover_handle, &scr_gameover);
        } else {
            view_scr_game();
        }
    } break;
    case AC_DISPLAY_BUTON_UP_PRESSED: {
        switch (game.player.dir) {
            case DIR_UP:    game_player_turn(DIR_LEFT);  break;
            case DIR_LEFT:  game_player_turn(DIR_DOWN);  break;
            case DIR_DOWN:  game_player_turn(DIR_RIGHT); break;
            case DIR_RIGHT: game_player_turn(DIR_UP);    break;
        }
    } break;
    case AC_DISPLAY_BUTON_DOWN_PRESSED: {
        switch (game.player.dir) {
            case DIR_UP:    game_player_turn(DIR_RIGHT); break;
            case DIR_RIGHT: game_player_turn(DIR_DOWN);  break;
            case DIR_DOWN:  game_player_turn(DIR_LEFT);  break;
            case DIR_LEFT:  game_player_turn(DIR_UP);    break;
        }
    } break;
    case AC_DISPLAY_BUTON_MODE_PRESSED: {
        SCREEN_BACK();
    } break;
    default:
        break;
    }
}