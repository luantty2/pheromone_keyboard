#pragma once

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x6060
#define DEVICE_VER      0x0001
#define MANUFACTURER    LUANTY
#define PRODUCT         PHEROMONE
#define DESCRIPTION     SEX PHEROMONE

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

/* key matrix pins */
#define MATRIX_ROW_PINS { B3, B2, B1, B0, E6 }
#define MATRIX_COL_PINS { D3, D5, D4, D6, D7, B4, B5, B6, C6, C7, F7, F6, F4, F1, F0 }

#define OLED_FONT_H "keymaps/default/glcdfont.c"

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

// #define TAPPING_TERM 200

#define ENCODERS_PAD_A { B7 }
#define ENCODERS_PAD_B { D2 }
#define ENCODER_RESOLUTION 4

#define MIDI_BASIC

#define BOOTMAGIC_LITE_ROW 0
#define BOOTMAGIC_LITE_COLUMN 0

#define NO_ACTION_TAPPING
#define NO_ACTION_ONESHOT
#define DISABLE_LEADER

/*enable midi slider*/
// #define POT_ENABLE

/*enable right shift+2 email address marco*/
#define EMAIL_ENABLE
#ifdef EMAIL_ENABLE
	#define EMAIL_ADDRESS "luanty@163.com"
#endif



