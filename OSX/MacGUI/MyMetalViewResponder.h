/*
 * (C) 2015 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#import "MyMetalView.h"


// Mac keycodes of special keys
const uint16_t MAC_F1 = 122;
const uint16_t MAC_F2 = 120;
const uint16_t MAC_F3 = 99;
const uint16_t MAC_F4 = 118;
const uint16_t MAC_F5 = 96;
const uint16_t MAC_F6 = 97;
const uint16_t MAC_F7 = 98;
const uint16_t MAC_F8 = 100;
const uint16_t MAC_APO = 39;
const uint16_t MAC_DEL = 51;
const uint16_t MAC_RET = 36;
const uint16_t MAC_CL = 123;
const uint16_t MAC_CR = 124;
const uint16_t MAC_CU = 126;
const uint16_t MAC_CD = 125;
const uint16_t MAC_TAB = 48;
const uint16_t MAC_SPC = 49;
const uint16_t MAC_ESC = 53;
const uint16_t MAC_HAT = 10;
const uint16_t MAC_TILDE_US = 50;

#if 0

@interface MyMetalView(Responder)

#pragma mark Joystick simulation

//! Computes a fingerprint for the keycode/modifierFlags combination that uniquely identifies a key for joystick emulation
- (MacKeyFingerprint)fingerprintForKey:(int)keycode withModifierFlags:(unsigned long)flags;

//! @brief Returns the keycode for a joystick emulation key
- (MacKeyFingerprint)joyKeyFingerprint:(int)nr direction:(JoystickDirection)dir;

//! @brief Sets the keycode for a joystick emulation key
- (void)setJoyKeyFingerprint:(MacKeyFingerprint)key keymap:(int)nr direction:(JoystickDirection)dir;

//! @brief Returns a joystick emulation key as printable character
- (char)joyChar:(int)nr direction:(JoystickDirection)dir;

//! @brief Sets the printable character for a joystick emulation key
- (void)setJoyChar:(char)c keymap:(int)nr direction:(JoystickDirection)dir;

// Compares keycode with the joystick simulation keys and pulls joystick if appropriate
- (BOOL)pullJoystick:(int)nr withKey:(MacKeyFingerprint)key device:(int)d;

// Compares keycode with the joystick simulation keys and releases joystick if appropriate
- (BOOL)releaseJoystick:(int)nr withKey:(MacKeyFingerprint)key device:(int)d;

//! Get mac specific character and keycode and translate it to a virtual c64 key
/*! The returned value can be passed to the pressKey() method of the emulator */
- (C64KeyFingerprint)translateKey:(char)key plainkey:(char)plainkey keycode:(short)keycode flags:(unsigned long)flags;

@end

#endif

