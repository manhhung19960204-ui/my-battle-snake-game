#include "scr_setting.h"
#include "screen_manager.h"
#include "app_data.h"
#include "app.h"
 
static void view_scr_setting();
 
    view_dynamic_t dyn_view_setting = {
        { .item_type = ITEM_TYPE_DYNAMIC },
        view_scr_setting
    };
    
    view_screen_t scr_setting = {
        &dyn_view_setting,
        ITEM_NULL,
        ITEM_NULL,
        .focus_item = 0,
    };
    
    // ============ RENDER ============
    static void view_scr_setting() {
        view_render.clear();
        view_render.drawRect(0, 0, 119, 62, WHITE);
    
        view_render.setTextColor(WHITE);
    
        // Tiêu đề
        view_render.setCursor(30, 4);
        view_render.print("- SETTINGS -");
    
        // Dòng kẻ
        view_render.drawRect(1, 14, 117, 1, WHITE);
    
        // Tên 3 level và mô tả ngắn tính năng thêm vào
        // Level 1: +1 food (2 mồi)
        // Level 2: +snake cản
        // Level 3: +tường
        const char* names[3]   = {"1: EASY",  "2: NORMAL", "3: HARD"};
        const char* descs[3]   = {"+2 food",  "+snake can", "+wall"};
        int y_pos[3]           = {19, 33, 47};
    
        for (int i = 0; i < 3; i++) {
            game_diff_t diff = (game_diff_t)(i + 1);
            bool selected = (app_data.difficulty == diff);
    
            if (selected) {
                // Highlight dòng đang chọn
                view_render.fillRect(2, y_pos[i] - 1, 115, 11, WHITE);
                view_render.setTextColor(BLACK);
            } else {
                view_render.setTextColor(WHITE);
            }
    
            view_render.setCursor(6, y_pos[i]);
            view_render.print(names[i]);
            view_render.setCursor(70, y_pos[i]);
            view_render.print(descs[i]);
    
            // Dấu ">>" chỉ mức đang chọn
            if (selected) {
                view_render.setCursor(116, y_pos[i]);
                // Dùng ký tự "<" vì font nhỏ có thể không có ">>"
                view_render.print("<");
            }
        }
    
        view_render.setTextColor(WHITE);
        view_render.setCursor(4, 56);
        view_render.print("UP/DN:chon  MODE:luu");
    
        view_render.update();
    }
    
    // ============ HANDLE ============
    void scr_setting_handle(ak_msg_t *msg) {
        switch (msg->sig) {
        case SCREEN_ENTRY: {
            if (app_data.difficulty < GAME_DIFF_EASY ||
                app_data.difficulty > GAME_DIFF_HARD) {
                app_data.difficulty = GAME_DIFF_EASY;
            }
            view_scr_setting();
        } break;
    
        case SCREEN_EXIT: break;
    
        case AC_DISPLAY_BUTON_UP_PRESSED: {
            // Lên = giảm level (vòng tròn)
            if (app_data.difficulty == GAME_DIFF_EASY)
                app_data.difficulty = GAME_DIFF_HARD;
            else
                app_data.difficulty = (game_diff_t)(app_data.difficulty - 1);
            view_scr_setting();
        } break;
    
        case AC_DISPLAY_BUTON_DOWN_PRESSED: {
            // Xuống = tăng level (vòng tròn)
            if (app_data.difficulty == GAME_DIFF_HARD)
                app_data.difficulty = GAME_DIFF_EASY;
            else
                app_data.difficulty = (game_diff_t)(app_data.difficulty + 1);
            view_scr_setting();
        } break;
    
        case AC_DISPLAY_BUTON_MODE_PRESSED: {
            SCREEN_BACK();
        } break;
    
        default: break;
        }
    }