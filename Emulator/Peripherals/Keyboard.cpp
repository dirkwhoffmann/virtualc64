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

KeyAction::KeyAction(Action _type, u8 _nr, u64 _delay)
: type(_type), nr(_nr), delay(_delay)
{
    assert(nr < 66);
    
    row = Keyboard::rowcol[nr][0];
    col = Keyboard::rowcol[nr][1];
}

KeyAction::KeyAction(Action _type, u8 _row, u8 _col, u64 _delay)
: type(_type), row(_row), col(_col), delay(_delay)
{
    constexpr u8 _nr[64] = {
        
        15, 47, 63, 64, 16, 32, 48, 62,
        3, 19, 35, 4, 51, 36, 20, 50,
        5, 21, 37, 6, 53, 38, 22, 52,
        7, 23, 39, 8, 55, 40, 24, 54,
        9, 25, 41, 10, 57, 42, 26, 56,
        11, 27, 43, 12, 59, 44, 28, 58,
        13, 29, 45, 14, 61, 46, 30, 60,
        1, 0, 17, 2, 65, 49, 18, 33
    };
    
    assert(row < 8);
    assert(col < 8);
    
    nr = _nr[8 * row + col];
}

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
Keyboard::press(long nr)
{
    synchronized {

        abortAutoTyping();
        _press(nr);
    }
}

void
Keyboard::pressRowCol(u8 row, u8 col)
{
    synchronized {
        
        abortAutoTyping();
        _pressRowCol(row, col);
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
Keyboard::release(long nr)
{
    synchronized { _release(nr); }
}

void
Keyboard::releaseRowCol(u8 row, u8 col)
{
    synchronized { _releaseRowCol(row, col); }
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
Keyboard::_press(long nr)
{
    debug(KBD_DEBUG, "_press(%ld)\n", nr);

    assert(nr < 66);

    switch (nr) {
        case 34: toggleShiftLock(); return;
        case 31: _pressRestore(); return;
        default: _pressRowCol(rowcol[nr][0], rowcol[nr][1]);
    }
}

void
Keyboard::_pressRowCol(u8 row, u8 col)
{
    debug(KBD_DEBUG, "_pressRowCol(%d,%d)\n", row, col);
    
    assert(row < 8);
    assert(col < 8);
    
    kbMatrixRow[row] &= ~(1 << col);
    kbMatrixCol[col] &= ~(1 << row);
}

void
Keyboard::_pressRestore()
{
    debug(KBD_DEBUG, "_pressRestor()\n");
    
    cpu.pullDownNmiLine(INTSRC_KBD);
}

void
Keyboard::_release(long nr)
{
    debug(KBD_DEBUG, "_release(%ld)\n", nr);

    assert(nr < 66);
    
    switch (nr) {
        case 34: releaseShiftLock(); return;
        case 31: _releaseRestore(); return;
        default: _releaseRowCol(rowcol[nr][0], rowcol[nr][1]);
    }
}

void
Keyboard::_releaseRowCol(u8 row, u8 col)
{
    debug(KBD_DEBUG, "_releaseRowCol(%d,%d)\n", row, col);

    assert(row < 8);
    assert(col < 8);
    
    // Only release right shift key if shift lock is not pressed
    if (row == 6 && col == 4 && shiftLock) return;
    
    kbMatrixRow[row] |= (1 << col);
    kbMatrixCol[col] |= (1 << row);
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
Keyboard::isPressed(long nr) const
{
    assert(nr < 66);
    
    switch (nr) {
        case 34: return shiftLockIsPressed();
        case 31: return restoreIsPressed();
        default: return isPressed(rowcol[nr][0], rowcol[nr][1]);
    }
}

bool
Keyboard::isPressed(u8 row, u8 col) const
{
    bool result1 = (kbMatrixRow[row] & (1 << col)) == 0;

    // We could have also checked the column matrix
    if (KBD_DEBUG) {
        assert(result1 == ((kbMatrixCol[col] & (1 << row)) == 0));
    }

    return result1;
}

bool
Keyboard::restoreIsPressed() const
{
    return cpu.nmiLine & INTSRC_KBD;
}

void
Keyboard::toggle(long nr)
{
    isPressed(nr) ? release(nr) : press(nr);
}

void
Keyboard::toggle(u8 row, u8 col)
{
    isPressed(row, col) ? releaseRowCol(row, col) : pressRowCol(row,col);
}

void
Keyboard::scheduleKeyPress(long nr, i64 delay)
{
    synchronized { _scheduleKeyAction(KeyAction::Action::press, nr, delay); }
}

void
Keyboard::scheduleKeyPress(u8 row, u8 col, i64 delay)
{
    synchronized { _scheduleKeyAction(KeyAction::Action::press, row, col, delay); }
}

void
Keyboard::scheduleKeyRelease(long nr, i64 delay)
{
    synchronized { _scheduleKeyAction(KeyAction::Action::release, nr, delay); }
}

void
Keyboard::scheduleKeyRelease(u8 row, u8 col, i64 delay)
{
    synchronized { _scheduleKeyAction(KeyAction::Action::release, row, col, delay); }
}

void
Keyboard::scheduleKeyReleaseAll(i64 delay)
{
    synchronized { _scheduleKeyAction(KeyAction::Action::releaseAll, 0, delay); }
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
Keyboard::_scheduleKeyAction(KeyAction::Action type, long nr, i64 delay)
{
    debug(KBD_DEBUG, "Recording %d %ld %lld\n", type, nr, delay);

    if (actions.empty()) this->delay = delay;
    actions.push(KeyAction(type, nr, delay));
}

void
Keyboard::_scheduleKeyAction(KeyAction::Action type, u8 row, u8 col, i64 delay)
{
    debug(KBD_DEBUG, "Recording %d %d %d %lld\n", type, row, col, delay);
    
    _scheduleKeyAction(type, C64Key(row,col).nr, delay);
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
                        
                        debug(KBD_DEBUG, "Pressing %d\n", action.nr);
                        _press(action.nr);
                        break;
                        
                    case KeyAction::Action::release:
                        
                        debug(KBD_DEBUG, "Releasing %d\n", action.nr);
                        _release(action.nr);
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
