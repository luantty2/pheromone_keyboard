#pragma once

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x6060
#define DEVICE_VER      0x0001
#define MANUFACTURER    LUANTY
#define PRODUCT         CRUSHER
#define DESCRIPTION     LAYOUT

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

/* key matrix pins */
#define MATRIX_ROW_PINS { B3, B2, B1, B0, E6 }
#define MATRIX_COL_PINS { D3, D5, D4, D6, D7, B4, B5, B6, C6, C7, F7, F6, F4, F1, F0 }
#define UNUSED_PINS

#define OLED_FONT_H "keyboards/crusher/keymaps/default/glcdfont.c"

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE

/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

// #ifdef AUDIO_ENABLE
//   #define B5_AUDIO
//   #define STARTUP_SONG SONG(ONE_UP_SOUND)
//   #define NO_MUSIC_MODE
// #endif

#define ENCODERS_PAD_A { B7 }
#define ENCODERS_PAD_B { D2 }
#define ENCODER_RESOLUTION 4

#ifndef MIDI_ADVANCED
   #define MIDI_ADVANCED
#endif

// #define RGB_DI_PIN F5
// #define RGBLIGHT_ANIMATIONS
// #define RGBLIGHT_SLEEP
// #define RGBLED_NUM 47
// #define RGBLIGHT_HUE_STEP 8
// #define RGBLIGHT_SAT_STEP 8
// #define RGBLIGHT_VAL_STEP 8
// #define RGBLIGHT_EFFECT_KNIGHT_LED_NUM 12



