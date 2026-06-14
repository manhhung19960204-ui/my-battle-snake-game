#include "app_data.h"

/* global modbus object */
#if defined (TASK_MBMASTER_EN)
xMBHandle xMBMMaster;
app_data_t app_data = {
    .mode       = GAME_MODE_SINGLE,
    .score      = 0,
    .high_score = 0,
};
#endif
