#include "pheromone.h"
#ifdef POT_ENABLE
  #include "analog.h"
#endif



#define _BASE 0
#define _FUNC 1

int16_t pot_oldVal = 0;
int16_t pot_val    = 0;
int16_t pot_ccVal  = 0;
#define POT_TOLERANCE 12

static char current_alpha_oled = '\0';

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_GRV, KC_BSPC,
                     //---
                     KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC,
                     //---
                     KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_BSLS, KC_ENT,
                     //---
                     KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_UP, TO(1),/*TO(_FUNC),*/
                     //---
                     KC_LCTL, KC_LALT, KC_LGUI, KC_SPC, KC_LEFT, KC_DOWN, KC_RGHT),

    [_FUNC] = LAYOUT(KC_TRNS, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-ESC_____1______2______3______4______5______6______7______8______9______10______ _ _______=________`_______bksp
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-Tab_____q_________w________e________r________t________y________u________i_______o_________p_______{__________}
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-Caps____a_________s________d________f________g________h_______j________k_________l________;________'________\______Enter
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, TO(0),/* TO(_LTRM),*/
                     //-Shift___z_________x________c________v________b________n_______m________,_________.________/______Shift______Up_____________fn
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MPRV, KC_MSTP, KC_MNXT),
    				 //-Ctrl____Alt_______Win____Space____Left______Down____Right
    [2] = LAYOUT(KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-ESC_____1______2______3______4______5______6______7______8______9______10______ _ _______=________`_______bksp
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-Tab_____q_________w________e________r________t________y________u________i_______o_________p_______{__________}
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-Caps____a_________s________d________f________g________h_______j________k_________l________;________'________\______Enter
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,/* TO(_LTRM),*/
                     //-Shift___z_________x________c________v________b________n_______m________,_________.________/______Shift______Up_____________fn
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
                     //-Ctrl____Alt_______Win____Space____Left______Down____Right
    [3] = LAYOUT(KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-ESC_____1______2______3______4______5______6______7______8______9______10______ _ _______=________`_______bksp
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-Tab_____q_________w________e________r________t________y________u________i_______o_________p_______{__________}
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-Caps____a_________s________d________f________g________h_______j________k_________l________;________'________\______Enter
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,/* TO(_LTRM),*/
                     //-Shift___z_________x________c________v________b________n_______m________,_________.________/______Shift______Up_____________fn
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
                     //-Ctrl____Alt_______Win____Space____Left______Down____Right

};

void matrix_init_user(void) {
#ifdef POT_ENABLE
    analogReference(ADC_REF_POWER);
#endif
}

void matrix_scan_user(void) {
#ifdef POT_ENABLE
    pot_val   = (analogReadPin(F5));
    pot_ccVal = pot_val / 8;
    if (abs(pot_val - pot_oldVal) > POT_TOLERANCE) {
        pot_oldVal = pot_val;
        // midi_send_cc(&midi_device, 0, 27, pot_ccVal);
    }
#endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch(keycode){
        case KC_A:
            if (record->event.pressed) {
                current_alpha_oled = 'A';
            } else {
            }
            break;
        case KC_B:
            if (record->event.pressed) {
                current_alpha_oled = 'B';
            } else {
            }
            break;
        case KC_C:
            if (record->event.pressed) {
                current_alpha_oled = 'C';
            } else {
            }
            break;
        case KC_D:
            if (record->event.pressed) {
                current_alpha_oled = 'D';
            } else {
            }
            break;
        case KC_E:
            if (record->event.pressed) {
                current_alpha_oled = 'E';
            } else {
            }
            break;
        case KC_F:
            if (record->event.pressed) {
                current_alpha_oled = 'F';
            } else {
            }
            break;
        case KC_G:
            if (record->event.pressed) {
                current_alpha_oled = 'G';
            } else {
            }
            break;
        case KC_H:
            if (record->event.pressed) {
                current_alpha_oled = 'H';
            } else {
            }
            break;
        case KC_I:
            if (record->event.pressed) {
                current_alpha_oled = 'I';
            } else {
            }
            break;
        case KC_J:
            if (record->event.pressed) {
                current_alpha_oled = 'J';
            } else {
            }
            break;
        case KC_K:
            if (record->event.pressed) {
                current_alpha_oled = 'K';
            } else {
            }
            break;
        case KC_L:
            if (record->event.pressed) {
                current_alpha_oled = 'L';
            } else {
            }
            break;
        case KC_M:
            if (record->event.pressed) {
                current_alpha_oled = 'M';
            } else {
            }
            break;
        case KC_N:
            if (record->event.pressed) {
                current_alpha_oled = 'N';
            } else {
            }
            break;
        case KC_O:
            if (record->event.pressed) {
                current_alpha_oled = 'O';
            } else {
            }
            break;
        case KC_P:
            if (record->event.pressed) {
                current_alpha_oled = 'P';
            } else {
            }
            break;
        case KC_Q:
            if (record->event.pressed) {
                current_alpha_oled = 'Q';
            } else {
            }
            break;
        case KC_R:
            if (record->event.pressed) {
                current_alpha_oled = 'R';
            } else {
            }
            break;
        case KC_S:
            if (record->event.pressed) {
                current_alpha_oled = 'S';
            } else {
            }
            break;
        case KC_T:
            if (record->event.pressed) {
                current_alpha_oled = 'T';
            } else {
            }
            break;
        case KC_U:
            if (record->event.pressed) {
                current_alpha_oled = 'U';
            } else {
            }
            break;
        case KC_V:
            if (record->event.pressed) {
                current_alpha_oled = 'V';
            } else {
            }
            break;
        case KC_W:
            if (record->event.pressed) {
                current_alpha_oled = 'W';
            } else {
            }
            break;
        case KC_X:
            if (record->event.pressed) {
                current_alpha_oled = 'X';
            } else {
            }
            break;
        case KC_Y:
            if (record->event.pressed) {
                current_alpha_oled = 'Y';
            } else {
            }
            break;
        case KC_Z:
            if (record->event.pressed) {
                current_alpha_oled = 'Z';
            } else {
            }
            break;
#ifdef EMAIL_ENABLE
        case KC_2:
            if (record->event.pressed) {
                if (get_mods() & MOD_BIT(KC_LSHIFT)) {
                    register_code16(LSFT(KC_2));
                } else if (get_mods() & MOD_BIT(KC_RSHIFT)) {
                    del_mods(MOD_BIT(KC_RSHIFT));
                    SEND_STRING(EMAIL_ADDRESS);
                } else {
                    register_code(KC_2);
                }
                if (get_mods() & MOD_BIT(KC_LSHIFT)) {
                    unregister_code16(LSFT(KC_2));
                } else if (get_mods() & MOD_BIT(KC_RSHIFT)) {
                } else {
                    unregister_code(KC_2);
                }
                return false;
            }
            break;
#endif
        default:
            break;
    }
    return true;
}

#ifdef OLED_DRIVER_ENABLE
void oled_task_user(void) {
    static const char PROGMEM image[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4};
    oled_write_P(image, false);
    uint8_t led_usb_state = host_keyboard_leds();
    switch (biton32(layer_state)) {
        case _BASE:
        case _FUNC:
            oled_write_P(led_usb_state & (1 << USB_LED_CAPS_LOCK) ? PSTR("}~LOCK") : PSTR("{|    "), false);
            oled_write_P(PSTR("   ["), false);
            if (current_alpha_oled == '\0') {
                oled_write_char('*', false);
            } else {
                oled_write_char(current_alpha_oled, false);
            }
            oled_write_P(PSTR("]   "), false);
            break;
    }
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_P(PSTR("1 BASE"), false);
            break;
        case _FUNC:
            oled_write_P(PSTR("2 FUNC"), false);
            break;
        default:
            oled_write_ln_P(PSTR("  USER"), false);
    }
}
#endif

void encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        switch (biton32(layer_state)) {
            case _BASE:
                if (clockwise) {
                    tap_code(KC_VOLU);
                } else {
                    tap_code(KC_VOLD);
                }
                break;
            case _FUNC:
                if (clockwise) {
                    tap_code(KC_BRIGHTNESS_UP);

                } else {
                    tap_code(KC_BRIGHTNESS_DOWN);
                }
                break;
            default:
                break;
        }
    }
}
