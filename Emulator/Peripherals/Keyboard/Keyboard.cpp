// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Keyboard.h"
#include "C64.h"
#include "C64Key.h"
#include "IO.h"

void 
Keyboard::_reset(bool hard) 
{
    RESET_SNAPSHOT_ITEMS(hard)
    
    // Reset the keyboard matrix
    releaseAll();
}

void
Keyboard::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category & dump::State) {
        
        for (int i = 0; i < 8; i++) {
            
            os << dec((kbMatrixRow[i] >> 0) & 1) << " ";
            os << dec((kbMatrixRow[i] >> 1) & 1) << " ";
            os << dec((kbMatrixRow[i] >> 2) & 1) << " ";
            os << dec((kbMatrixRow[i] >> 3) & 1) << " ";
            os << dec((kbMatrixRow[i] >> 4) & 1) << " ";
            os << dec((kbMatrixRow[i] >> 5) & 1) << " ";
            os << dec((kbMatrixRow[i] >> 6) & 1) << " ";
            os << dec((kbMatrixRow[i] >> 7) & 1) << "    ";

            os << dec((kbMatrixCol[i] >> 0) & 1) << " ";
            os << dec((kbMatrixCol[i] >> 1) & 1) << " ";
            os << dec((kbMatrixCol[i] >> 2) & 1) << " ";
            os << dec((kbMatrixCol[i] >> 3) & 1) << " ";
            os << dec((kbMatrixCol[i] >> 4) & 1) << " ";
            os << dec((kbMatrixCol[i] >> 5) & 1) << " ";
            os << dec((kbMatrixCol[i] >> 6) & 1) << " ";
            os << dec((kbMatrixCol[i] >> 7) & 1) << std::endl;

            os << std::endl;
            os << "Shift lock " << (shiftLock ? "pressed" : "not pressed");
            os << std::endl;
        }
    }
}

u8
Keyboard::getColumnValues(u8 rowMask)
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
Keyboard::getRowValues(u8 columnMask)
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
Keyboard::getRowValues(u8 columnMask, u8 thresholdMask)
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

void
Keyboard::press(C64Key key)
{
    synchronized {

        abortAutoTyping();
        _press(key);
    }
}

void
Keyboard::pressRestore()
{
    synchronized {
        
        abortAutoTyping();
        _pressRestore();        
    }
}

void
Keyboard::release(C64Key key)
{
    synchronized { _release(key); }
}

void
Keyboard::releaseRestore()
{
    synchronized { _releaseRestore(); }
}

void
Keyboard::releaseAll()
{
    synchronized { _releaseAll(); }
}

void
Keyboard::_press(C64Key key)
{
    debug(KBD_DEBUG, "_press(%zd)\n", key.nr);

    assert(key.nr < 66);

    switch (key.nr) {
        case 34: toggleShiftLock(); return;
        case 31: _pressRestore(); return;
    }

    assert(key.row < 8);
    assert(key.col < 8);
    
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
Keyboard::_pressRestore()
{
    debug(KBD_DEBUG, "_pressRestor()\n");
    
    cpu.pullDownNmiLine(INTSRC_KBD);
}

void
Keyboard::_release(C64Key key)
{
    debug(KBD_DEBUG, "_release(%zd)\n", key.nr);

    assert(key.nr < 66);
    
    switch (key.nr) {
        case 34: releaseShiftLock(); return;
        case 31: _releaseRestore(); return;
    }

    assert(key.row < 8);
    assert(key.col < 8);
    
    // Only release right shift key if shift lock is not pressed
    if (key.row == 6 && key.col == 4 && shiftLock) return;
    
    if (GET_BIT(kbMatrixRow[key.row], key.col) == 0) {
        SET_BIT(kbMatrixRow[key.row], key.col);
        kbMatrixRowCnt[key.row]--;
    }
    if (GET_BIT(kbMatrixCol[key.col], key.row) == 0) {
        SET_BIT(kbMatrixCol[key.col], key.row);
        kbMatrixColCnt[key.col]--;
    }
}

void
Keyboard::_releaseRestore()
{
    debug(KBD_DEBUG, "_releaseRestore()\n");
    
    cpu.releaseNmiLine(INTSRC_KBD);
}

void
Keyboard::_releaseAll()
{
    debug(KBD_DEBUG, "_releaseAll()\n");
    
    for (isize i = 0; i < 8; i++) {
        
        kbMatrixRow[i] = 0xFF; kbMatrixRowCnt[i] = 0;
        kbMatrixCol[i] = 0xFF; kbMatrixColCnt[i] = 0;
        
    }
    _releaseRestore();
}

bool
Keyboard::isPressed(C64Key key) const
{
    assert(key.nr < 66);
    
    switch (key.nr) {
        case 34: return shiftLockIsPressed();
        case 31: return restoreIsPressed();
    }

    bool result = (kbMatrixRow[key.row] & (1 << key.col)) == 0;

    // We could have also checked the column matrix
    assert(result == ((kbMatrixCol[key.col] & (1 << key.row)) == 0));

    return result;
}
    
bool
Keyboard::restoreIsPressed() const
{
    return cpu.nmiLine & INTSRC_KBD;
}

void
Keyboard::toggle(C64Key key)
{
    isPressed(key) ? release(key) : press(key);
}

void
Keyboard::autoType(const string &text)
{    
    for (char const &c: text) {
                
        scheduleKeyPress(c, 2);
        scheduleKeyRelease(c, 2);
    }
}

void
Keyboard::scheduleKeyPress(C64Key key, i64 delay)
{
    synchronized { _scheduleKeyAction(KeyAction::Action::press, key, delay); }
}

void
Keyboard::scheduleKeyPress(char c, i64 delay)
{
    auto keys = C64Key::translate(c);

    for (auto &key: keys) {
        scheduleKeyPress(key, delay);
        delay = 0;
    }
}

void
Keyboard::scheduleKeyRelease(C64Key key, i64 delay)
{
    synchronized { _scheduleKeyAction(KeyAction::Action::release, key, delay); }
}

void
Keyboard::scheduleKeyRelease(char c, i64 delay)
{
    auto keys = C64Key::translate(c);

    for (auto &key: keys) {
        scheduleKeyRelease(key, delay);
        delay = 0;
    }
}

void
Keyboard::scheduleKeyReleaseAll(i64 delay)
{
    synchronized {
        _scheduleKeyAction(KeyAction::Action::releaseAll, C64Key(0), delay);
    }
}

void
Keyboard::abortAutoTyping()
{
    if (!actions.empty()) {

        std::queue<KeyAction> empty;
        std::swap(actions, empty);

        _releaseAll();
    }
}

void
Keyboard::_scheduleKeyAction(KeyAction::Action type, C64Key key, i64 delay)
{
    debug(KBD_DEBUG, "Recording %d %zd %lld\n", (int)type, key.nr, delay);

    if (actions.empty()) this->delay = delay;
    actions.push(KeyAction(type, key.nr, delay));
}

void
Keyboard::vsyncHandler()
{
    // Only take action when the timer fires
    if (delay == 0) {
        
        // Process all pending auto-typing events
        synchronized {
                        
            while (delay == 0 && !actions.empty()) {
                
                KeyAction &action = actions.front();
                actions.pop();
                
                // trace(KBD_DEBUG, "%d: key (%d,%d) next: %lld\n",
                //       action.type, action.row, action.col, action.delay);
                
                // Process event
                switch (action.type) {
                        
                    case KeyAction::Action::press:
                        
                        debug(KBD_DEBUG, "Pressing %zd\n", action.key.nr);
                        _press(action.key);
                        break;
                        
                    case KeyAction::Action::release:
                        
                        debug(KBD_DEBUG, "Releasing %zd\n", action.key.nr);
                        _release(action.key);
                        break;
                        
                    case KeyAction::Action::releaseAll:
                        
                        debug(KBD_DEBUG, "Releasing all\n");
                        _releaseAll();
                        break;
                }
                
                // Schedule next event
                delay = actions.empty() ? INT64_MAX : actions.front().delay;
            }
        }
    }
    delay--;
}
