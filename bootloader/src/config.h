#pragma once

// HID Bootloader takes 4K
#define USER_PROGRAM 0x08001000

#ifdef TARGET_GENERIC
#define VIAL_KEYBOARD_UID {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#else
#error Unknown target
#endif
