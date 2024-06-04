// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "Keyboard.h"
#include "C64.h"
#include "C64Key.h"
#include "IOUtils.h"

u8
Keyboard::getColumnValues(u8 rowMask) const
{
    u8 result = 0xff;
    
    for (isize i = 0; i < 8; i++) {
        if (GET_BIT(rowMask, i)) {
            result &= kbMatrixCol[i];
        }
    }
    
    // Check for shift-lock
    if (shiftLock && GET_BIT(rowMask, 7)) {
        CLR_BIT(result, 1);
    }
    
    return result;
}

u8
Keyboard::getRowValues(u8 columnMask) const
{
    u8 result = 0xff;
        
    for (isize i = 0; i < 8; i++) {
        if (GET_BIT(columnMask, i)) {
            result &= kbMatrixRow[i];
        }
    }
    
    // Check for shift-lock
    if (shiftLock && GET_BIT(columnMask, 1)) {
        CLR_BIT(result, 7);
    }
    
    return result;
}

u8
Keyboard::getRowValues(u8 columnMask, u8 thresholdMask) const
{
    /* This function implements the special behaviour of the keyboard matrix
     * as described in the README file of VICE test ciaports.prg. It covers the
     * case that both CIA ports are driven as output.
     *
     * "Port A outputs (active) low, Port B outputs high. [...] Port B will be
     *  driven low (and then read back 0) only if the resistance of the physical
     *  connection created over the keyboard matrix is low enough to allow the
     *  required current. this is (again) usually not the case when pressing
     *  single keys, instead -depending on the keyboard- pressing two or more
     *  keys of the same column is required."
     *
     * This feature has an interesing side effect. It can be exploited to
     * detect if the shift-lock key is held down.
     *
     * "A special case is the shift-lock key, which will also work and which
     *  you can seperate from the normal left shift key in this configuration."
     */
    
    // Check if we can fallback to the (faster) standard routine
    if (thresholdMask == 0) return getRowValues(columnMask);
    
	u8 result = 0xff;
    u8 count[8] = { };
    
    // Count the number of pressed keys per column
	for (isize i = 0; i < 8; i++) {
		if (GET_BIT(columnMask, i)) {
            for (isize j = 0; j < 8; j++) {
                if (GET_BIT(kbMatrixRow[i], j) == 0) count[j]++;
            }
		}
	}

    // Only detect those keys with a high enough column count
    for (isize j = 0; j < 8; j++) {
        if (count[j] >= (GET_BIT(thresholdMask, j) ? 2 : 1)) CLR_BIT(result, j);
    }
    
    // Check for shift-lock
    if (shiftLock && GET_BIT(columnMask, 1)) {
        CLR_BIT(result, 7);
    }
    
	return result;
}

bool
Keyboard::isPressed(C64Key key) const
{
    assert(key.nr < 66);

    switch (key.nr) {
            
        case 34: return shiftLock;
        case 31: return cpu.getNmiLine() & INTSRC_KBD;
    }

    return (kbMatrixRow[key.row] & (1 << key.col)) == 0;
}

void
Keyboard::press(C64Key key)
{
    debug(KBD_DEBUG, "press(%ld)\n", key.nr);

    switch (key.nr) {

        case 34: shiftLock = true; return;
        case 31: cpu.pullDownNmiLine(INTSRC_KBD); return;
    }

    assert(key.nr < 66);
    assert(key.row < 8);
    assert(key.col < 8);

    SYNCHRONIZED

    if (GET_BIT(kbMatrixRow[key.row], key.col)) {
        CLR_BIT(kbMatrixRow[key.row], key.col);
        kbMatrixRowCnt[key.row]++;
    }
    if (GET_BIT(kbMatrixCol[key.col], key.row)) {
        CLR_BIT(kbMatrixCol[key.col], key.row);
        kbMatrixColCnt[key.col]++;
    }
}

void
Keyboard::release(C64Key key)
{
    debug(KBD_DEBUG, "release(%ld)\n", key.nr);

    switch (key.nr) {

        case 34: shiftLock = false; return;
        case 31: cpu.releaseNmiLine(INTSRC_KBD); return;
    }

    assert(key.nr < 66);
    assert(key.row < 8);
    assert(key.col < 8);

    SYNCHRONIZED

    // Only release right shift key if shift lock is not pressed
    if (key.row == 6 && key.col == 4 && shiftLock) return;

    if (GET_BIT(kbMatrixRow[key.row], key.col) == 0) {
        SET_BIT(kbMatrixRow[key.row], key.col);
        kbMatrixRowCnt[key.row]--;
    }
    if (GET_BIT(kbMatrixCol[key.col], key.row) == 0) {
        SET_BIT(kbMatrixCol[key.col], key.row);
        kbMatrixColCnt[key.col]--;
    }}

void
Keyboard::releaseAll()
{
    SYNCHRONIZED

    debug(KBD_DEBUG, "releaseAll()\n");

    // Clear the keyboard matrix
    for (isize i = 0; i < 8; i++) {

        kbMatrixRow[i] = 0xFF; kbMatrixRowCnt[i] = 0;
        kbMatrixCol[i] = 0xFF; kbMatrixColCnt[i] = 0;

    }

    // Release the restore key
    cpu.releaseNmiLine(INTSRC_KBD);
}

void
Keyboard::autoType(const string &text)
{
    auto trigger = c64.cpu.clock;

    for (char const &c: text) {

        auto keys = C64Key::translate(c);

        if (pending.free() > isize(2 * keys.size())) {

            // Schedule key presses
            for (C64Key &k : keys) {
                pending.insert(trigger, Cmd(CMD_KEY_PRESS, KeyCmd { .keycode = u8(k.nr) }));
            }

            trigger += C64::msec(30);

            // Schedule key releases
            for (C64Key &k : keys) {
                pending.insert(trigger, Cmd(CMD_KEY_RELEASE, KeyCmd { .keycode = u8(k.nr) }));
            }
            trigger += C64::msec(30);
        }
    }

    if (!c64.hasEvent<SLOT_KEY>()) c64.scheduleImm<SLOT_KEY>(KEY_AUTO_TYPE);
}

void
Keyboard::abortAutoTyping()
{
    SYNCHRONIZED

    if (!pending.isEmpty()) {

        pending.clear();
        releaseAll();
    }
}

void 
Keyboard::processCommand(const Cmd &cmd)
{
    if (cmd.key.delay > 0) {

        pending.insert(cpu.clock + C64::sec(cmd.key.delay),
                       Cmd(cmd.type, KeyCmd { .keycode = cmd.key.keycode }));
        c64.scheduleImm<SLOT_KEY>(KEY_AUTO_TYPE);
        return;
    }

    switch (cmd.type) {

        case CMD_KEY_PRESS:         press(C64Key(cmd.key.keycode)); break;
        case CMD_KEY_RELEASE:       release(C64Key(cmd.key.keycode)); break;
        case CMD_KEY_RELEASE_ALL:   releaseAll(); break;
        case CMD_KEY_TOGGLE:        toggle(C64Key(cmd.key.keycode)); break;

        default:
            fatalError;
    }
}

void
Keyboard::processKeyEvent(EventID id)
{
    SYNCHRONIZED

    // Process all pending events
    while (!pending.isEmpty()) {

        if (pending.keys[pending.r] > c64.cpu.clock) break;

        auto cmd = pending.read();
        processCommand(cmd);
    }

    // Schedule next event
    if (pending.isEmpty()) {

        releaseAll();
        c64.cancel<SLOT_KEY>();

    } else {

        c64.rescheduleAbs<SLOT_KEY>(pending.keys[pending.r]);
    }
}
