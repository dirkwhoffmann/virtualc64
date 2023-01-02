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
#include "IOUtils.h"

/*
KeyAction::KeyAction(Action a, C64Key k, u64 d) : type(a), delay(d)
{
    keys.push_back(k);
};
*/

void 
Keyboard::_reset(bool hard) 
{
    RESET_SNAPSHOT_ITEMS(hard)
    
    // Reset the keyboard matrix
    releaseAll();
}

void
Keyboard::_dump(Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category == Category::State) {
        
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
    SYNCHRONIZED return _isPressed(key);
}

bool
Keyboard::shiftLockIsPressed() const
{
    SYNCHRONIZED return _shiftLockIsPressed();
}

bool
Keyboard::restoreIsPressed() const
{
    SYNCHRONIZED return _restoreIsPressed();
}

void
Keyboard::press(C64Key key)
{
    SYNCHRONIZED _abortAutoTyping(); _press(key);
}

void
Keyboard::pressShiftLock()
{
    SYNCHRONIZED _pressShiftLock();
}

void
Keyboard::pressRestore()
{
    SYNCHRONIZED _abortAutoTyping(); _pressRestore();
}

void
Keyboard::release(C64Key key)
{
    SYNCHRONIZED _release(key);
}

void
Keyboard::releaseShiftLock()
{
    SYNCHRONIZED _releaseShiftLock();
}

void
Keyboard::releaseRestore()
{
    SYNCHRONIZED _releaseRestore();
}

void
Keyboard::releaseAll()
{
    SYNCHRONIZED _releaseAll();
}

bool
Keyboard::_isPressed(C64Key key) const
{
    assert(key.nr < 66);

    switch (key.nr) {
        case 34: return shiftLockIsPressed();
        case 31: return restoreIsPressed();
    }

    return (kbMatrixRow[key.row] & (1 << key.col)) == 0;
}

bool
Keyboard::_shiftLockIsPressed() const
{
    return shiftLock;
}

bool
Keyboard::_restoreIsPressed() const
{
    return cpu.getNmiLine() & INTSRC_KBD;
}

void
Keyboard::_press(C64Key key)
{
    debug(KBD_DEBUG, "_press(%ld)\n", key.nr);

    switch (key.nr) {

        case 34: toggleShiftLock(); return;
        case 31: _pressRestore(); return;

        default:
            assert(key.nr < 66);
            break;
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
Keyboard::_pressShiftLock()
{
    shiftLock = true;
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
    debug(KBD_DEBUG, "_release(%ld)\n", key.nr);

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
Keyboard::_releaseShiftLock()
{
    shiftLock = false;
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

void
Keyboard::autoType(const string &text)
{
    SYNCHRONIZED _autoType(text);
}

void
Keyboard::_autoType(const string &text)
{    
    for (char const &c: text) {
                
        _scheduleKeyPress(c, 0.03);
        _scheduleKeyRelease(c, 0.03);
    }
}

void
Keyboard::scheduleKeyPress(std::vector<C64Key> keys, double delay)
{
    SYNCHRONIZED _scheduleKeyPress(keys, delay);
}

void
Keyboard::_scheduleKeyPress(std::vector<C64Key> keys, double delay)
{
    actions.push(KeyAction(KeyAction::Action::wait, SEC(delay)));
    actions.push(KeyAction(KeyAction::Action::press, keys));

    if (!c64.hasEvent<SLOT_KEY>()) c64.scheduleImm<SLOT_KEY>(KEY_AUTO_TYPE);
}

void
Keyboard::scheduleKeyRelease(std::vector<C64Key> keys, double delay)
{
    SYNCHRONIZED _scheduleKeyRelease(keys, delay);
}

void
Keyboard::_scheduleKeyRelease(std::vector<C64Key> keys, double delay)
{
    actions.push(KeyAction(KeyAction::Action::wait, SEC(delay)));
    actions.push(KeyAction(KeyAction::Action::release, keys));

    if (!c64.hasEvent<SLOT_KEY>()) c64.scheduleImm<SLOT_KEY>(KEY_AUTO_TYPE);
}

void
Keyboard::scheduleKeyReleaseAll(double delay)
{
    SYNCHRONIZED _scheduleKeyReleaseAll(delay);
}

void
Keyboard::_scheduleKeyReleaseAll(double delay)
{
    actions.push(KeyAction(KeyAction::Action::wait, SEC(delay)));
    actions.push(KeyAction(KeyAction::Action::releaseAll, {}));

    if (!c64.hasEvent<SLOT_KEY>()) c64.scheduleImm<SLOT_KEY>(KEY_AUTO_TYPE);
}

void
Keyboard::_abortAutoTyping()
{
    if (!actions.empty()) {

        std::queue<KeyAction> empty;
        std::swap(actions, empty);

        _releaseAll();
    }
}

void
Keyboard::processKeyEvent(EventID id)
{
    SYNCHRONIZED

    // Process all pending events
    while (!actions.empty()) {

        auto type = actions.front().type;
        auto keys = actions.front().keys;
        auto delay = actions.front().delay;

        actions.pop();

        switch (type) {

            case KeyAction::Action::wait:

                debug(KBD_DEBUG, "Waiting %lld cycles\n", delay);
                c64.rescheduleRel<SLOT_KEY>(delay);
                return;

            case KeyAction::Action::press:

                for (auto &key: keys) {

                    debug(KBD_DEBUG, "Pressing %ld\n", key.nr);
                    _press(key);
                }
                break;

            case KeyAction::Action::release:

                for (auto &key: keys) {

                    debug(KBD_DEBUG, "Releasing %ld\n", key.nr);
                    _release(key);
                }
                break;

            case KeyAction::Action::releaseAll:

                debug(KBD_DEBUG, "Releasing all\n");
                _releaseAll();
                break;
        }
    }

    _releaseAll();
    c64.cancel<SLOT_KEY>();
}
