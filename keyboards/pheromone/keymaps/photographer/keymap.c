#include "pheromone.h"
#ifdef POT_ENABLE
  #include "analog.h"
#endif
#include "midi.h"

extern MidiDevice midi_device;

#define _BASE 0
#define _FUNC 1
#define _LTRM 2

int16_t pot_oldVal = 0;
int16_t pot_val    = 0;
int16_t pot_ccVal  = 0;
#define POT_TOLERANCE 12

enum { TO_BASE_LAYER_BASE = 0, TO_BASE_LAYER_FUNC, TO_BASE_LAYER_LTRM, MIDI_Q, MIDI_W, MIDI_E, MIDI_R, MIDI_T, MIDI_Y, MIDI_U, MIDI_I, MIDI_O, MIDI_P, MIDI_A, MIDI_S, MIDI_D, MIDI_F, MIDI_G, MIDI_H, MIDI_J, MIDI_K, MIDI_L, MIDI_Z, MIDI_X, MIDI_C, MIDI_V, MIDI_B, MIDI_N, MIDI_M};
uint8_t last_stayed_layer = 0;

static char current_alpha_oled = '\0';
static char current_ltrm_alpha_oled = '\0';

static uint8_t current_MIDI_ccNumber         = 1;
static char    current_MIDI_ccNumber_char[3] = {'0', '1', '\0'};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_GRV, KC_BSPC,
                     //---
                     KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC,
                     //---
                     KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_BSLS, KC_ENT,
                     //---
                     KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_UP, TD(TO_BASE_LAYER_BASE),/*TO(_FUNC),*/
                     //---
                     #ifdef MOD_SEQUENCE_CAW
                     KC_LCTL, KC_LALT, KC_LGUI
                     #elif defined(MOD_SEQUENCE_CWA)
                     KC_LCTL, KC_LGUI, KC_LALT
                     #elif defined(MOD_SEQUENCE_WAC)
                     KC_LGUI, KC_LALT, KC_LCTL
                     #elif defined(MOD_SEQUENCE_WCA)
                     KC_LGUI, KC_LCTL, KC_LALT
                     #elif defined(MOD_SEQUENCE_AWC) 
                     KC_LALT, KC_LGUI, KC_LCTL
                     #elif defined(MOD_SEQUENCE_ACW)
                     KC_LALT, KC_LCTL, KC_LGUI
                     #else
                     KC_LCTL, KC_LALT, KC_LGUI
                     #endif
                     , KC_SPC, KC_LEFT, KC_DOWN, KC_RGHT),

    [_FUNC] = LAYOUT(KC_TRNS, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-ESC_____1______2______3______4______5______6______7______8______9______10______ _ _______=________`_______bksp
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-Tab_____q_________w________e________r________t________y________u________i_______o_________p_______{__________}
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-Caps____a_________s________d________f________g________h_______j________k_________l________;________'________\______Enter
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, TD(TO_BASE_LAYER_FUNC),/* TO(_LTRM),*/
                     //-Shift___z_________x________c________v________b________n_______m________,_________.________/______Shift______Up_____________fn
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MPRV, KC_MSTP, KC_MNXT),
    				 //-Ctrl____Alt_______Win____Space____Left______Down____Right

    [_LTRM] = LAYOUT(KC_TRNS, MI_C, MI_Db, MI_D, MI_Eb, MI_E, MI_F, MI_Gb, MI_G, MI_Ab, MI_A, MI_Bb, MI_B, MI_C_1, KC_TRNS,
                     //-ESC_____1_____2_____3______4______5____6______7______8_____9_____10____ _ ____=______`______bksp
                     KC_TRNS, TD(MIDI_Q), TD(MIDI_W), TD(MIDI_E), TD(MIDI_R), TD(MIDI_T), TD(MIDI_Y), TD(MIDI_U), TD(MIDI_I), TD(MIDI_O), TD(MIDI_P), KC_TRNS, KC_TRNS,
                     //-Tab_____q_________w__________e_________r_________t_________y_________u_________i_________o__________p_________{_______}
                     KC_TRNS, TD(MIDI_A), TD(MIDI_S), TD(MIDI_D), TD(MIDI_F), TD(MIDI_G), TD(MIDI_H), TD(MIDI_J), TD(MIDI_K), TD(MIDI_L), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     //-Caps______a__________s__________d__________f__________g__________h_________j__________k__________l__________;_______'_________\______Enter
                     KC_TRNS, TD(MIDI_Z), TD(MIDI_X), TD(MIDI_C), TD(MIDI_V), TD(MIDI_B), TD(MIDI_N), TD(MIDI_M), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MI_Db_1, TD(TO_BASE_LAYER_LTRM),/* TO(_BASE),*/
                     //-Shift_____z_________x___________c__________v__________b_________n___________m_________,________.________/_______Shift_____Up______________fn
                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MI_D_2, MI_Eb_2, MI_E_2),
    				 //-Ctrl____Alt_______Win____Space___Left_____Down_____Right

    // [_MIDI] = LAYOUT(KC_TRNS, MI_CH1, MI_CH2, MI_CH3, MI_CH4, MI_CH5, MI_CH6, MI_CH7, MI_CH8, MI_CH9, MI_CH10, MI_OCTD, MI_OCTU, KC_TRNS, KC_TRNS,
    //                  //-ESC_____1_______2_______3_______4_______5________6______7______8________9_______10________ _ ______=________`______bksp
    //                  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    //                  //-Tab_____q_________w__________e_________r_________t_________y_________u_________i_________o__________p_________{___________}
    //                  KC_TRNS, KC_TRNS, MI_Db, MI_Eb, KC_TRNS, MI_Gb, MI_Ab, MI_Bb, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    //                  //-Caps_____a_______s_______d______f_______g______h______j_______k________l________;________'________\______Enter
    //                  KC_TRNS, MI_C, MI_D, MI_E, MI_F, MI_G, MI_A, MI_B, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MI_VELU, TD(TO_BASE_LAYER_MIDI),
    //                  //-Shift___z_____x____c_____v_____b_____n_____m_______,_________._______/______Shift_____Up______________fn
    //                  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MPRV, MI_VELD, KC_MNXT),
    // 					//-Ctrl____Alt_______Win____Space___Left_____Down_____Right
};
void dance_MIDI_Q(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 1;
        current_ltrm_alpha_oled       = 'Q';
        current_MIDI_ccNumber_char[0] = '0';
        current_MIDI_ccNumber_char[1] = '1';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 27;
        current_ltrm_alpha_oled       = 'Q';
        current_MIDI_ccNumber_char[0] = '2';
        current_MIDI_ccNumber_char[1] = '7';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 53;
        current_ltrm_alpha_oled       = 'Q';
        current_MIDI_ccNumber_char[0] = '5';
        current_MIDI_ccNumber_char[1] = '3';
    }
}
void dance_MIDI_W(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 2;
        current_ltrm_alpha_oled       = 'W';
        current_MIDI_ccNumber_char[0] = '0';
        current_MIDI_ccNumber_char[1] = '2';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 28;
        current_ltrm_alpha_oled       = 'W';
        current_MIDI_ccNumber_char[0] = '2';
        current_MIDI_ccNumber_char[1] = '8';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 54;
        current_ltrm_alpha_oled       = 'W';
        current_MIDI_ccNumber_char[0] = '5';
        current_MIDI_ccNumber_char[1] = '4';
    }
}
void dance_MIDI_E(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 3;
        current_ltrm_alpha_oled       = 'E';
        current_MIDI_ccNumber_char[0] = '0';
        current_MIDI_ccNumber_char[1] = '3';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 29;
        current_ltrm_alpha_oled       = 'E';
        current_MIDI_ccNumber_char[0] = '2';
        current_MIDI_ccNumber_char[1] = '9';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 55;
        current_ltrm_alpha_oled       = 'E';
        current_MIDI_ccNumber_char[0] = '5';
        current_MIDI_ccNumber_char[1] = '5';
    }
}
void dance_MIDI_R(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 4;
        current_ltrm_alpha_oled       = 'R';
        current_MIDI_ccNumber_char[0] = '0';
        current_MIDI_ccNumber_char[1] = '4';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 30;
        current_ltrm_alpha_oled       = 'R';
        current_MIDI_ccNumber_char[0] = '3';
        current_MIDI_ccNumber_char[1] = '0';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 56;
        current_ltrm_alpha_oled       = 'R';
        current_MIDI_ccNumber_char[0] = '5';
        current_MIDI_ccNumber_char[1] = '6';
    }
}
void dance_MIDI_T(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 5;
        current_ltrm_alpha_oled       = 'T';
        current_MIDI_ccNumber_char[0] = '0';
        current_MIDI_ccNumber_char[1] = '5';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 31;
        current_ltrm_alpha_oled       = 'T';
        current_MIDI_ccNumber_char[0] = '3';
        current_MIDI_ccNumber_char[1] = '1';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 57;
        current_ltrm_alpha_oled       = 'T';
        current_MIDI_ccNumber_char[0] = '5';
        current_MIDI_ccNumber_char[1] = '7';
    }
}
void dance_MIDI_Y(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 6;
        current_ltrm_alpha_oled       = 'Y';
        current_MIDI_ccNumber_char[0] = '0';
        current_MIDI_ccNumber_char[1] = '6';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 32;
        current_ltrm_alpha_oled       = 'Y';
        current_MIDI_ccNumber_char[0] = '3';
        current_MIDI_ccNumber_char[1] = '2';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 58;
        current_ltrm_alpha_oled       = 'Y';
        current_MIDI_ccNumber_char[0] = '5';
        current_MIDI_ccNumber_char[1] = '8';
    }
}
void dance_MIDI_U(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 7;
        current_ltrm_alpha_oled       = 'U';
        current_MIDI_ccNumber_char[0] = '0';
        current_MIDI_ccNumber_char[1] = '7';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 33;
        current_ltrm_alpha_oled       = 'U';
        current_MIDI_ccNumber_char[0] = '3';
        current_MIDI_ccNumber_char[1] = '3';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 59;
        current_ltrm_alpha_oled       = 'U';
        current_MIDI_ccNumber_char[0] = '5';
        current_MIDI_ccNumber_char[1] = '9';
    }
}
void dance_MIDI_I(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 8;
        current_ltrm_alpha_oled       = 'I';
        current_MIDI_ccNumber_char[0] = '0';
        current_MIDI_ccNumber_char[1] = '8';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 34;
        current_ltrm_alpha_oled       = 'I';
        current_MIDI_ccNumber_char[0] = '3';
        current_MIDI_ccNumber_char[1] = '4';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 60;
        current_ltrm_alpha_oled       = 'I';
        current_MIDI_ccNumber_char[0] = '6';
        current_MIDI_ccNumber_char[1] = '0';
    }
}
void dance_MIDI_O(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 9;
        current_ltrm_alpha_oled       = 'O';
        current_MIDI_ccNumber_char[0] = '0';
        current_MIDI_ccNumber_char[1] = '9';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 35;
        current_ltrm_alpha_oled       = 'O';
        current_MIDI_ccNumber_char[0] = '3';
        current_MIDI_ccNumber_char[1] = '5';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 61;
        current_ltrm_alpha_oled       = 'O';
        current_MIDI_ccNumber_char[0] = '6';
        current_MIDI_ccNumber_char[1] = '1';
    }
}
void dance_MIDI_P(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 10;
        current_ltrm_alpha_oled       = 'P';
        current_MIDI_ccNumber_char[0] = '1';
        current_MIDI_ccNumber_char[1] = '0';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 36;
        current_ltrm_alpha_oled       = 'P';
        current_MIDI_ccNumber_char[0] = '3';
        current_MIDI_ccNumber_char[1] = '6';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 62;
        current_ltrm_alpha_oled       = 'P';
        current_MIDI_ccNumber_char[0] = '6';
        current_MIDI_ccNumber_char[1] = '2';
    }
}
void dance_MIDI_A(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 11;
        current_ltrm_alpha_oled       = 'A';
        current_MIDI_ccNumber_char[0] = '1';
        current_MIDI_ccNumber_char[1] = '1';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 37;
        current_ltrm_alpha_oled       = 'A';
        current_MIDI_ccNumber_char[0] = '3';
        current_MIDI_ccNumber_char[1] = '7';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 63;
        current_ltrm_alpha_oled       = 'A';
        current_MIDI_ccNumber_char[0] = '6';
        current_MIDI_ccNumber_char[1] = '3';
    }
}
void dance_MIDI_S(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 12;
        current_ltrm_alpha_oled       = 'S';
        current_MIDI_ccNumber_char[0] = '1';
        current_MIDI_ccNumber_char[1] = '2';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 38;
        current_ltrm_alpha_oled       = 'S';
        current_MIDI_ccNumber_char[0] = '3';
        current_MIDI_ccNumber_char[1] = '8';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 64;
        current_ltrm_alpha_oled       = 'S';
        current_MIDI_ccNumber_char[0] = '6';
        current_MIDI_ccNumber_char[1] = '4';
    }
}
void dance_MIDI_D(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 13;
        current_ltrm_alpha_oled       = 'D';
        current_MIDI_ccNumber_char[0] = '1';
        current_MIDI_ccNumber_char[1] = '3';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 39;
        current_ltrm_alpha_oled       = 'D';
        current_MIDI_ccNumber_char[0] = '3';
        current_MIDI_ccNumber_char[1] = '9';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 65;
        current_ltrm_alpha_oled       = 'D';
        current_MIDI_ccNumber_char[0] = '6';
        current_MIDI_ccNumber_char[1] = '5';
    }
}
void dance_MIDI_F(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 14;
        current_ltrm_alpha_oled       = 'F';
        current_MIDI_ccNumber_char[0] = '1';
        current_MIDI_ccNumber_char[1] = '4';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 40;
        current_ltrm_alpha_oled       = 'F';
        current_MIDI_ccNumber_char[0] = '4';
        current_MIDI_ccNumber_char[1] = '0';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 66;
        current_ltrm_alpha_oled       = 'F';
        current_MIDI_ccNumber_char[0] = '6';
        current_MIDI_ccNumber_char[1] = '6';
    }
}
void dance_MIDI_G(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 15;
        current_ltrm_alpha_oled       = 'G';
        current_MIDI_ccNumber_char[0] = '1';
        current_MIDI_ccNumber_char[1] = '5';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 41;
        current_ltrm_alpha_oled       = 'G';
        current_MIDI_ccNumber_char[0] = '4';
        current_MIDI_ccNumber_char[1] = '1';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 67;
        current_ltrm_alpha_oled       = 'G';
        current_MIDI_ccNumber_char[0] = '6';
        current_MIDI_ccNumber_char[1] = '7';
    }
}
void dance_MIDI_H(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 16;
        current_ltrm_alpha_oled       = 'H';
        current_MIDI_ccNumber_char[0] = '1';
        current_MIDI_ccNumber_char[1] = '6';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 42;
        current_ltrm_alpha_oled       = 'H';
        current_MIDI_ccNumber_char[0] = '4';
        current_MIDI_ccNumber_char[1] = '2';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 68;
        current_ltrm_alpha_oled       = 'H';
        current_MIDI_ccNumber_char[0] = '6';
        current_MIDI_ccNumber_char[1] = '8';
    }
}
void dance_MIDI_J(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 17;
        current_ltrm_alpha_oled       = 'J';
        current_MIDI_ccNumber_char[0] = '1';
        current_MIDI_ccNumber_char[1] = '7';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 43;
        current_ltrm_alpha_oled       = 'J';
        current_MIDI_ccNumber_char[0] = '4';
        current_MIDI_ccNumber_char[1] = '3';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 69;
        current_ltrm_alpha_oled       = 'J';
        current_MIDI_ccNumber_char[0] = '6';
        current_MIDI_ccNumber_char[1] = '9';
    }
}
void dance_MIDI_K(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 18;
        current_ltrm_alpha_oled       = 'K';
        current_MIDI_ccNumber_char[0] = '1';
        current_MIDI_ccNumber_char[1] = '8';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 44;
        current_ltrm_alpha_oled       = 'K';
        current_MIDI_ccNumber_char[0] = '4';
        current_MIDI_ccNumber_char[1] = '4';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 70;
        current_ltrm_alpha_oled       = 'K';
        current_MIDI_ccNumber_char[0] = '7';
        current_MIDI_ccNumber_char[1] = '0';
    }
}
void dance_MIDI_L(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 19;
        current_ltrm_alpha_oled       = 'L';
        current_MIDI_ccNumber_char[0] = '1';
        current_MIDI_ccNumber_char[1] = '9';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 45;
        current_ltrm_alpha_oled       = 'L';
        current_MIDI_ccNumber_char[0] = '4';
        current_MIDI_ccNumber_char[1] = '5';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 71;
        current_ltrm_alpha_oled       = 'L';
        current_MIDI_ccNumber_char[0] = '7';
        current_MIDI_ccNumber_char[1] = '1';
    }
}
void dance_MIDI_Z(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 20;
        current_ltrm_alpha_oled       = 'Z';
        current_MIDI_ccNumber_char[0] = '2';
        current_MIDI_ccNumber_char[1] = '0';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 46;
        current_ltrm_alpha_oled       = 'Z';
        current_MIDI_ccNumber_char[0] = '4';
        current_MIDI_ccNumber_char[1] = '6';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 72;
        current_ltrm_alpha_oled       = 'Z';
        current_MIDI_ccNumber_char[0] = '7';
        current_MIDI_ccNumber_char[1] = '2';
    }
}
void dance_MIDI_X(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 21;
        current_ltrm_alpha_oled       = 'X';
        current_MIDI_ccNumber_char[0] = '2';
        current_MIDI_ccNumber_char[1] = '1';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 47;
        current_ltrm_alpha_oled       = 'X';
        current_MIDI_ccNumber_char[0] = '4';
        current_MIDI_ccNumber_char[1] = '7';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 73;
        current_ltrm_alpha_oled       = 'X';
        current_MIDI_ccNumber_char[0] = '7';
        current_MIDI_ccNumber_char[1] = '3';
    }
}
void dance_MIDI_C(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 22;
        current_ltrm_alpha_oled       = 'C';
        current_MIDI_ccNumber_char[0] = '2';
        current_MIDI_ccNumber_char[1] = '2';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 48;
        current_ltrm_alpha_oled       = 'C';
        current_MIDI_ccNumber_char[0] = '4';
        current_MIDI_ccNumber_char[1] = '8';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 74;
        current_ltrm_alpha_oled       = 'C';
        current_MIDI_ccNumber_char[0] = '7';
        current_MIDI_ccNumber_char[1] = '4';
    }
}
void dance_MIDI_V(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 23;
        current_ltrm_alpha_oled       = 'V';
        current_MIDI_ccNumber_char[0] = '2';
        current_MIDI_ccNumber_char[1] = '3';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 49;
        current_ltrm_alpha_oled       = 'V';
        current_MIDI_ccNumber_char[0] = '4';
        current_MIDI_ccNumber_char[1] = '9';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 75;
        current_ltrm_alpha_oled       = 'V';
        current_MIDI_ccNumber_char[0] = '7';
        current_MIDI_ccNumber_char[1] = '5';
    }
}
void dance_MIDI_B(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 24;
        current_ltrm_alpha_oled       = 'B';
        current_MIDI_ccNumber_char[0] = '2';
        current_MIDI_ccNumber_char[1] = '4';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 50;
        current_ltrm_alpha_oled       = 'B';
        current_MIDI_ccNumber_char[0] = '5';
        current_MIDI_ccNumber_char[1] = '0';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 76;
        current_ltrm_alpha_oled       = 'B';
        current_MIDI_ccNumber_char[0] = '7';
        current_MIDI_ccNumber_char[1] = '6';
    }
}
void dance_MIDI_N(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 25;
        current_ltrm_alpha_oled       = 'N';
        current_MIDI_ccNumber_char[0] = '2';
        current_MIDI_ccNumber_char[1] = '5';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 51;
        current_ltrm_alpha_oled       = 'N';
        current_MIDI_ccNumber_char[0] = '5';
        current_MIDI_ccNumber_char[1] = '1';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 77;
        current_ltrm_alpha_oled       = 'N';
        current_MIDI_ccNumber_char[0] = '7';
        current_MIDI_ccNumber_char[1] = '7';
    }
}
void dance_MIDI_M(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        current_MIDI_ccNumber         = 26;
        current_ltrm_alpha_oled       = 'N';
        current_MIDI_ccNumber_char[0] = '2';
        current_MIDI_ccNumber_char[1] = '6';
    } else if (state->count == 2){  // TAP TWICE
        current_MIDI_ccNumber         = 52;
        current_ltrm_alpha_oled       = 'N';
        current_MIDI_ccNumber_char[0] = '5';
        current_MIDI_ccNumber_char[1] = '2';
    } else if (state->count == 3){  // TAP THREE TIMES
        current_MIDI_ccNumber         = 78;
        current_ltrm_alpha_oled       = 'N';
        current_MIDI_ccNumber_char[0] = '7';
        current_MIDI_ccNumber_char[1] = '8';
    }
}

void dance_layer_BASE(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        layer_on(_FUNC);
    } else {  // TAP TWICE OR MORE
        layer_on(last_stayed_layer);
    }
}
void dance_layer_reset_BASE(qk_tap_dance_state_t *state, void *user_data) { layer_off(_BASE); }

void dance_layer_FUNC(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        layer_on(_LTRM);
    } else {  // TAP TWICE OR MORE
        layer_on(_BASE);
        last_stayed_layer = _FUNC;
    }
}
void dance_layer_reset_FUNC(qk_tap_dance_state_t *state, void *user_data) { layer_off(_FUNC); }

void dance_layer_LTRM(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {  // TAP ONCE
        layer_on(_BASE);
    } else {  // TAP TWICE OR MORE
        layer_on(_BASE);
        last_stayed_layer = _LTRM;
    }
}
void dance_layer_reset_LTRM(qk_tap_dance_state_t *state, void *user_data) { layer_off(_LTRM); }

qk_tap_dance_action_t tap_dance_actions[] = {
    [TO_BASE_LAYER_BASE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_layer_BASE, dance_layer_reset_BASE), 
    [TO_BASE_LAYER_FUNC] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_layer_FUNC, dance_layer_reset_FUNC), 
    [TO_BASE_LAYER_LTRM] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_layer_LTRM, dance_layer_reset_LTRM),
    [MIDI_Q] = ACTION_TAP_DANCE_FN (dance_MIDI_Q),
    [MIDI_W] = ACTION_TAP_DANCE_FN (dance_MIDI_W),
    [MIDI_E] = ACTION_TAP_DANCE_FN (dance_MIDI_E),
    [MIDI_R] = ACTION_TAP_DANCE_FN (dance_MIDI_R),
    [MIDI_T] = ACTION_TAP_DANCE_FN (dance_MIDI_T),
    [MIDI_Y] = ACTION_TAP_DANCE_FN (dance_MIDI_Y),
    [MIDI_U] = ACTION_TAP_DANCE_FN (dance_MIDI_U),
    [MIDI_I] = ACTION_TAP_DANCE_FN (dance_MIDI_I),
    [MIDI_O] = ACTION_TAP_DANCE_FN (dance_MIDI_O),
    [MIDI_P] = ACTION_TAP_DANCE_FN (dance_MIDI_P),
    [MIDI_A] = ACTION_TAP_DANCE_FN (dance_MIDI_A),
    [MIDI_S] = ACTION_TAP_DANCE_FN (dance_MIDI_S),
    [MIDI_D] = ACTION_TAP_DANCE_FN (dance_MIDI_D),
    [MIDI_F] = ACTION_TAP_DANCE_FN (dance_MIDI_F),
    [MIDI_G] = ACTION_TAP_DANCE_FN (dance_MIDI_G),
    [MIDI_H] = ACTION_TAP_DANCE_FN (dance_MIDI_H),
    [MIDI_I] = ACTION_TAP_DANCE_FN (dance_MIDI_I),
    [MIDI_J] = ACTION_TAP_DANCE_FN (dance_MIDI_J),
    [MIDI_K] = ACTION_TAP_DANCE_FN (dance_MIDI_K),
    [MIDI_L] = ACTION_TAP_DANCE_FN (dance_MIDI_L),
    [MIDI_Z] = ACTION_TAP_DANCE_FN (dance_MIDI_Z),
    [MIDI_X] = ACTION_TAP_DANCE_FN (dance_MIDI_X),
    [MIDI_C] = ACTION_TAP_DANCE_FN (dance_MIDI_C),
    [MIDI_V] = ACTION_TAP_DANCE_FN (dance_MIDI_V),
    [MIDI_B] = ACTION_TAP_DANCE_FN (dance_MIDI_B),
    [MIDI_N] = ACTION_TAP_DANCE_FN (dance_MIDI_N),
    [MIDI_M] = ACTION_TAP_DANCE_FN (dance_MIDI_M)
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
        midi_send_cc(&midi_device, 0, 27, pot_ccVal);
    }
#endif
}
/*test
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    	case KC_0:
            if (record->event.pressed) {
                if (get_mods() & MOD_BIT(KC_LCTL)) {
                    del_mods(MOD_BIT(KC_LCTL));
                    //SEND_STRING("hi");
                } else {
                    register_code(KC_0);
                }
                if (get_mods() & MOD_BIT(KC_LCTL)) {
                    unregister_code16(LCTL(KC_0));
                } else {
                    unregister_code(KC_0);
                }
                return false;
            }
        break;
    }
}
*/

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
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
        case _LTRM:
            oled_write_P(PSTR("CC "), false);
            oled_write(current_MIDI_ccNumber_char, false);
            oled_write_P(PSTR("    ["), false);
            if (current_ltrm_alpha_oled == '\0') {
                oled_write_char('*', false);
            } else {
                oled_write_char(current_ltrm_alpha_oled, false);
            }
            oled_write_P(PSTR("]   "), false);
            break;
            // case _MIDI:
            //     oled_write_P(PSTR("               "), false);
            //     // oled_write(itoa(midi_config.octave, vel, 10), false);
            //     break;
    }
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_P(PSTR("1 BASE"), false);
            break;
        case _FUNC:
            oled_write_P(PSTR("2 FUNC"), false);
            break;
        case _LTRM:
            oled_write_P(PSTR("3 LTRM"), false);
            break;
        // case _MIDI:
        //     oled_write_P(PSTR("4 MIDI"), false);
        //     break;
        default:
            oled_write_ln_P(PSTR("  NONE"), false);
    }
}
#endif

// int16_t encoder_val = 64;
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
            case _LTRM:
                if (clockwise) {
                    midi_send_cc(&midi_device, 0, current_MIDI_ccNumber, 65);
                } else {
                    midi_send_cc(&midi_device, 0, current_MIDI_ccNumber, 63);
                }
                break;
            // case _MIDI:
            //     if (clockwise) {
            //         encoder_val++;
            //         midi_send_cc(&midi_device, 0, 1, encoder_val);
            //     } else {
            //         encoder_val--;
            //         midi_send_cc(&midi_device, 0, 1, encoder_val);
            //     }
            //     break;
            default:
                break;
        }
    }
}
