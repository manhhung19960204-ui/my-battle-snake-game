#include "scr_setting.h"
#include "screen_manager.h"
#include "app_data.h"
#include "app.h"
#include "app_dbg.h"


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

static void view_scr_setting() {
    view_render.clear();
    view_render.drawRect(0, 0, 119, 62, WHITE);

    // Tiêu đề
    view_render.setTextColor(WHITE);
    view_render.setCursor(34, 6);
    view_render.print("SETTINGS");

    // Dòng kẻ ngang dưới tiêu đề
    view_render.drawRect(1, 16, 117, 1, WHITE);

    // Label
    view_render.setCursor(4, 22);
    view_render.print("DIFFICULTY");

    // 3 mức: EASY / NORMAL / HARD
    // Mỗi mức cách nhau ~35px, highlight mức đang chọn bằng fillRect
    const char* labels[3] = {"EASY", "NORMAL", "HARD"};
    int x_pos[3] = {4, 42, 88};

    for (int i = 0; i < 3; i++) {
        game_diff_t diff = (game_diff_t)(i + 1); // 1/2/3
        if (app_data.difficulty == diff) {
            // Mức đang chọn: vẽ nền trắng, chữ đen
            int w = (i == 1) ? 36 : 26; // NORMAL rộng hơn
            view_render.fillRect(x_pos[i] - 1, 34, w, 11, WHITE);
            view_render.setTextColor(BLACK);
        } else {
            view_render.setTextColor(WHITE);
        }
        view_render.setCursor(x_pos[i], 36);
        view_render.print(labels[i]);
    }

    view_render.setTextColor(WHITE);

    // Hướng dẫn nút
    view_render.setCursor(2, 52);
    view_render.print("UP/DN:chon  MODE:luu");

    view_render.update();
}

void scr_setting_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY: {
        APP_DBG_SIG("SCREEN_ENTRY\n");
        // Mặc định EASY nếu chưa set
        if (app_data.difficulty < GAME_DIFF_EASY ||
            app_data.difficulty > GAME_DIFF_HARD) {
            app_data.difficulty = GAME_DIFF_EASY;
        }
        view_scr_setting();
    } break;

    case SCREEN_EXIT: {
    } break;

    case AC_DISPLAY_BUTON_UP_PRESSED: {
        // Giảm độ khó (vòng tròn)
        if (app_data.difficulty == GAME_DIFF_EASY) {
            app_data.difficulty = GAME_DIFF_HARD;
        } else {
            app_data.difficulty = (game_diff_t)(app_data.difficulty - 1);
        }
        view_scr_setting();
    } break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED: {
        // Tăng độ khó (vòng tròn)
        if (app_data.difficulty == GAME_DIFF_HARD) {
            app_data.difficulty = GAME_DIFF_EASY;
        } else {
            app_data.difficulty = (game_diff_t)(app_data.difficulty + 1);
        }
        view_scr_setting();
    } break;

    case AC_DISPLAY_BUTON_MODE_PRESSED: {
        // Lưu và quay về menu
        SCREEN_BACK();
    } break;

    default:
        break;
    }
}