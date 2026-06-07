#include "screens_bitmap.h"
#include "scr_menu.h"
#define TOTAL_MENU_ITEMS (sizeof(menu_list) / sizeof(menu_item_t))

static void view_scr_menu();

typedef struct {
    const unsigned char* bitmap; 
    const char* label;
} menu_item_t;

static const menu_item_t menu_list[] = {
    {bitmap_menu_1, "Single"},
    {bitmap_menu_2, "COM"},
    {bitmap_menu_3, "Setting"},
    {bitmap_menu_4, "Exit"}
};

static uint8_t current_menu_index = 0; // Biến ghi nhớ bạn đang đứng ở đâu

view_dynamic_t dyn_view_menu = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_menu
};

view_screen_t scr_menu = {
	&dyn_view_menu,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_menu() {
	view_render.clear();
	view_render.drawBitmap(	0, \
		0, \
		bitmap_menu, \
		119, \
		62, \
		WHITE);
    view_render.setTextColor(WHITE); // 
    
    const char* label = menu_list[current_menu_index].label;
    int x = (119 - strlen(label) * 6) / 2;
    view_render.setCursor(x, 35); // 
    view_render.print(label);
    
    view_render.update();

}

void scr_menu_handle(ak_msg_t *msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		BUZZER_PlaySound(BUZZER_SOUND_WELCOME);
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_WELCOME_TEXT_ANIM_TICK, AC_DISPLAY_WELCOME_TEXT_ANIM_TICK_INTERVAL, TIMER_PERIODIC);
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE, AC_DISPLAY_IDLE_INTERVAL, TIMER_ONE_SHOT);
		view_scr_menu(); 
	} break;

	case AC_DISPLAY_BUTON_UP_PRESSED: 
	case AC_DISPLAY_BUTON_DOWN_PRESSED: {
    // Tăng/giảm index
    if (msg->sig == AC_DISPLAY_BUTON_UP_PRESSED) {
        if (current_menu_index > 0) current_menu_index--;
        else current_menu_index = TOTAL_MENU_ITEMS - 1; // Vòng lại cuối
    } else {
        current_menu_index++;
        if (current_menu_index >= TOTAL_MENU_ITEMS) current_menu_index = 0; // Vòng lại đầu
    }
    
    view_scr_menu(); 
	} break;

	default:
		break;
	}
}