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
Keyboard::_reset() 
{
    RESET_SNAPSHOT_ITEMS
    
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
Keyboard::getRowValues(u8 columnMask)
{
	u8 result = 0xff;
		
	for (unsigned i = 0; i < 8; i++) {
		if (GET_BIT(columnMask, i)) {
			result &= kbMatrixRow[i];
		}
	}
    
    // Check for shift lock
    if (shiftLock && GET_BIT(columnMask, 6))
        CLR_BIT(result, 4);
	
	return result;
}

u8
Keyboard::getColumnValues(u8 rowMask)
{
    u8 result = 0xff;
    
    for (unsigned i = 0; i < 8; i++) {
        if (GET_BIT(rowMask, i)) {
            result &= kbMatrixCol[i];
        }
    }
    
    // Check for shift lock
    if (shiftLock && GET_BIT(rowMask, 4))
        CLR_BIT(result, 6);
    
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
    
    kbMatrixRow[key.row] &= ~(1 << key.col);
    kbMatrixCol[key.col] &= ~(1 << key.row);
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
    
    kbMatrixRow[key.row] |= (1 << key.col);
    kbMatrixCol[key.col] |= (1 << key.row);
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
    
    for (unsigned i = 0; i < 8; i++) {
        kbMatrixRow[i] = kbMatrixCol[i] = 0xFF;
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

    bool result1 = (kbMatrixRow[key.row] & (1 << key.col)) == 0;

    // We could have also checked the column matrix
    if (KBD_DEBUG) {
        assert(result1 == ((kbMatrixCol[key.col] & (1 << key.row)) == 0));
    }

    return result1;
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
                
        scheduleKeyPress(c, 1);
        scheduleKeyRelease(c, 1);
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
    debug(KBD_DEBUG, "Recording %d %zd %lld\n", type, key.nr, delay);

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
