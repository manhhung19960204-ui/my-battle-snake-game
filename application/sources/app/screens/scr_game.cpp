#include "screens_bitmap.h"
#include "scr_game.h"

static void view_scr_game();

typedef struct {
    const unsigned char* bitmap; 
    const char* label;
} game_item_t;

static const game_item_t game_list[] = {
    {bitmap_screen_game, "Snake game play"},
};
static uint8_t current_game_index = 0; 

view_dynamic_t dyn_view_game = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_game
};

view_screen_t scr_game = {
	&dyn_view_game,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

// void view_scr_game() {
// 	view_render.clear();
// 	view_render.drawBitmap(	0, \
// 		0, \
// 		bitmap_screen_game, \
// 		119, \
// 		62, \
// 		WHITE);
//     view_render.setTextColor(WHITE); // 
    
//     const char* label = game_list[current_game_index].label;
//     int x = (119 - strlen(label) * 6) / 2;
//     view_render.setCursor(x, 35); // 
//     view_render.print(label);
    
//     view_render.update();

// }

void view_scr_game() {
    view_render.clear();

    // Hiện tên mode đang chọn
    view_render.setTextColor(WHITE);
    
    if (app_data.mode == GAME_MODE_SINGLE) {
        view_render.setCursor(40, 20);
        view_render.print("SINGLE");
    } else {
        view_render.setCursor(40, 20);
        view_render.print("COM");
    }

    view_render.setCursor(20, 35);
    view_render.print("Press MODE to start");

    view_render.update();
}


void scr_game_handle(ak_msg_t *msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		BUZZER_PlaySound(BUZZER_SOUND_WELCOME);
		view_scr_game(); 
	} break;
    
    view_scr_game(); 
	break;

	default:
		break;
    }
}