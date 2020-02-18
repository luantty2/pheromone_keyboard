#include "crusher.h"
#include "analog.h"
#include "midi.h"
// #include <print.h>

extern MidiDevice midi_device;

#define _BASE 0
#define _FUNC 1

int oldVal=0;
int MIDIVOL=0;
#define TOLERANCE 30

enum custom_keycodes {
    MIDI_VOL = SAFE_RANGE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE]=LAYOUT(
    KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_GRV, KC_BSPC, 
    KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, 
    KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_BSLS, KC_ENT, 
    KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_UP, TG(_FUNC), 
    KC_LCTL, KC_LALT, KC_LGUI, KC_SPC, KC_LEFT, KC_DOWN, KC_RGHT),

  [_FUNC]=LAYOUT(
    KC_TRNS, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
    KC_TRNS, MI_C, MIDI_VOL, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MPRV, KC_MSTP, KC_MNXT)
};

void matrix_init_user(void) {
  analogReference(ADC_REF_INTERNAL);
}

void matrix_scan_user(void) {
	int val = analogReadPin(F5);
  MIDIVOL=val/8;
  if(abs(val - oldVal)>TOLERANCE)
  {
    oldVal = val;
    midi_send_cc(&midi_device, 0, 7, MIDIVOL);
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t * record) {
  switch (keycode) {
        case MIDI_VOL:
            if (record->event.pressed) {
                midi_send_cc(&midi_device, 0, 7, 64);
                // midi_send_noteon(&midi_device, 1, 64, 100);
            } else {
                midi_send_cc(&midi_device, 0, 7, 0);
                // midi_send_noteoff(&midi_device, 1,64, 0);
            }
            break;
        default:
            break;
    }
    return true;
}
#ifdef OLED_DRIVER_ENABLE
void oled_task_user(void) {
	static const char PROGMEM caps_release[] = {
		0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,
		0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,
		0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4
	};
	oled_write_P(caps_release, false);
	uint8_t led_usb_state = host_keyboard_leds();
	oled_write_P(led_usb_state & (1<<USB_LED_CAPS_LOCK) ? PSTR("CAPLCK ") : PSTR("       "), false);

	oled_write_P(PSTR("          "),false);
		switch (get_highest_layer(layer_state)) {
    case _BASE:
      oled_write_P(PSTR("BASE"), false);
      break;
    case _FUNC:
      oled_write_P(PSTR("FUNC"), false);
      break;
    default:
      // Or use the write_ln shortcut over adding '\n' to the end of your string
      oled_write_ln_P(PSTR("BASE"), false);
  }
}
#endif

void encoder_update_user(uint8_t index, bool clockwise) {
	 if (index == 0) {
        switch(biton32(layer_state)){
            case 1:
                if (clockwise) {
                tap_code(KC_SLCK);
                } else {
                tap_code(KC_PAUS);

                }
                break;
            default:
                if (clockwise){
                    tap_code(KC_VOLD);
                } else{
                    tap_code(KC_VOLU);
                }
                break;
            }
        }
}
