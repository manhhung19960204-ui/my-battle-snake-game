        #include "scr_gameover.h"
        #include "screen_manager.h"
        #include "app_data.h"
        #include "app.h"
        #include "scr_game.h"   

        static void view_scr_gameover();

        view_dynamic_t dyn_view_gameover = {
            { .item_type = ITEM_TYPE_DYNAMIC },
            view_scr_gameover
        };

        view_screen_t scr_gameover = {
            &dyn_view_gameover,
            ITEM_NULL,
            ITEM_NULL,
            .focus_item = 0,
        };

        // ============ STATE NỘI BỘ ============
        static game_result_t s_result      = GAME_RESULT_LOSE;
        static uint16_t       s_player_score = 0;
        static uint16_t       s_com_score    = 0;

        void gameover_set_result(game_result_t result, uint16_t player_score, uint16_t com_score) {
            s_result       = result;
            s_player_score = player_score;
            s_com_score    = com_score;
        }

    // ============ RENDER Màn hình ============
        static void view_scr_gameover() {
            view_render.clear();

            // Khung viền giống màn chơi cho đồng bộ
            view_render.drawRect(0, 0, 119, 62, WHITE);

            // ---- Tiêu đề WIN / LOSE / DRAW ----
            const char *title;
            switch (s_result) {
                case GAME_RESULT_WIN:  title = "YOU WIN";  break;
                case GAME_RESULT_DRAW: title = "DRAW";     break;
                case GAME_RESULT_LOSE:
                default:                title = "GAME OVER"; break;
            }

            view_render.setTextColor(WHITE);
            int title_len = 0;
            while (title[title_len] != '\0') title_len++;
            int title_x = (118 - title_len * 6) / 2;
            if (title_x < 2) title_x = 2;

            view_render.setCursor(title_x, 14);
            view_render.print(title);

            // ---- Điểm số ----
            if (app_data.mode == GAME_MODE_COM) {
                view_render.setCursor(10, 30);
                view_render.print("YOU:");
                view_render.setCursor(45, 30);
                view_render.print(s_player_score);

                view_render.setCursor(70, 30);
                view_render.print("COM:");
                view_render.setCursor(105, 30);
                view_render.print(s_com_score);
            } else {
                // Chế độ đơn: chỉ hiện 1 điểm, canh giữa
                view_render.setCursor(40, 30);
                view_render.print("SCORE:");
                view_render.setCursor(85, 30);
                view_render.print(s_player_score);
            }

            view_render.setCursor(6, 50);
            view_render.print("UP:Retry  MODE:Menu");

            view_render.update();
        }

        // ============ HANDLE ============
        void scr_gameover_handle(ak_msg_t *msg) {
            switch (msg->sig) {
            case SCREEN_ENTRY: {
                APP_DBG_SIG("SCREEN_ENTRY\n");
                view_scr_gameover();
            } break;

            case SCREEN_EXIT: {
                // Không có timer nào cần dọn ở màn này
            } break;

            case AC_DISPLAY_BUTON_UP_PRESSED: {
                // Chơi lại
                SCREEN_TRAN(scr_game_handle, &scr_game);
            } break;

            case AC_DISPLAY_BUTON_DOWN_PRESSED: {
                // Chơi lại 
                SCREEN_TRAN(scr_game_handle, &scr_game);
            } break;

            case AC_DISPLAY_BUTON_MODE_PRESSED: {
                extern void scr_menu_handle(ak_msg_t* msg);
                extern view_screen_t scr_menu;
                SCREEN_TRAN(scr_menu_handle, &scr_menu);  
            } break;    

            default:
                break;
            }
        }