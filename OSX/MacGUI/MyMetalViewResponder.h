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

@interface MyMetalView(Responder)


#pragma mark Joystick simulation

//! Computes a fingerprint for the keycode/modifierFlags combination that uniquely identifies a key for joystick emulation
- (int)fingerprintForKey:(int)keycode withModifierFlags:(int)flags;

//! @brief Returns the keycode for a joystick emulation key
- (int)joyKeycode:(int)nr direction:(JoystickDirection)dir;

//! @brief Sets the keycode for a joystick emulation key
- (void)setJoyKeycode:(int)keycode keymap:(int)nr direction:(JoystickDirection)dir;

//! @brief Returns a joystick emulation key as printable character
- (char)joyChar:(int)nr direction:(JoystickDirection)dir;

//! @brief Sets the printable character for a joystick emulation key
- (void)setJoyChar:(char)c keymap:(int)nr direction:(JoystickDirection)dir;

// Compares keycode with the joystick simulation keys and pulls joystick if appropriate
- (BOOL)pullJoystick:(int)nr withKeycode:(int)keycode device:(int)d;

// Compares keycode with the joystick simulation keys and releases joystick if appropriate
- (BOOL)releaseJoystick:(int)nr withKeycode:(int)keycode device:(int)d;

//! Get mac specific character and keycode and translate it to a virtual c64 key
/*! The returned value can be passed to the pressKey() method of the emulator */
- (int)translateKey:(char)key plainkey:(char)plainkey keycode:(short)keycode flags:(int)flags;


@end