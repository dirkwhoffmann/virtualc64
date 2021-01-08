// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

KeyAction::KeyAction(bool _press, u8 _nr, u64 _delay)
: press(_press), nr(_nr), delay(_delay)
{
    assert(nr < 66);
    
    row = Keyboard::rowcol[nr][0];
    col = Keyboard::rowcol[nr][1];
}

KeyAction::KeyAction(bool _press, u8 _row, u8 _col, u64 _delay)
: press(_press), row(_row), col(_col), delay(_delay)
{
    const u8 _nr[64] = {
        
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

/*
void
KeyAction::perform(Keyboard &kb)
{
    debug(KBD_DEBUG, "%s %d (%d,%d)\n", press ? "Pressing" : "Releasing", nr, row, col);
    
    press ? kb._press(nr) : kb._release(nr);
    kb.delay = delay;
}
*/

void 
Keyboard::_reset() 
{
    RESET_SNAPSHOT_ITEMS

	// Reset the keyboard matrix
    releaseAll();
}

void 
Keyboard::_dump() const
{
	msg("Keyboard:\n");
	msg("---------\n\n");
	msg("Keyboard matrix: ");
	for (int i = 0; i < 8; i++) {
		msg("%d %d %d %d %d %d %d %d    %d %d %d %d %d %d %d %d\n                 ",
            (kbMatrixRow[i] & 0x01) != 0,
            (kbMatrixRow[i] & 0x02) != 0,
            (kbMatrixRow[i] & 0x04) != 0,
            (kbMatrixRow[i] & 0x08) != 0,
            (kbMatrixRow[i] & 0x10) != 0,
            (kbMatrixRow[i] & 0x20) != 0,
            (kbMatrixRow[i] & 0x40) != 0,
            (kbMatrixRow[i] & 0x80) != 0,

            (kbMatrixCol[i] & 0x01) != 0,
            (kbMatrixCol[i] & 0x02) != 0,
            (kbMatrixCol[i] & 0x04) != 0,
            (kbMatrixCol[i] & 0x08) != 0,
            (kbMatrixCol[i] & 0x10) != 0,
            (kbMatrixCol[i] & 0x20) != 0,
            (kbMatrixCol[i] & 0x40) != 0,
            (kbMatrixCol[i] & 0x80) != 0);
	}
	msg("\n");
    msg("Shift lock: %s pressed\n", shiftLock ? "" : "not");
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
    debug(KBD_DEBUG, "releaseRowCol(%d,%d)\n", row, col);

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
    for (unsigned i = 0; i < 8; i++) {
        kbMatrixRow[i] = kbMatrixCol[i] = 0xFF;
    }
    _releaseRestore();
}

bool
Keyboard::isPressed(long nr)
{
    assert(nr < 66);
    
    switch (nr) {
        case 34: return shiftLockIsPressed();
        case 31: return restoreIsPressed();
        default: return isPressed(rowcol[nr][0], rowcol[nr][1]);
    }
}

bool
Keyboard::isPressed(u8 row, u8 col)
{
    bool result1 = (kbMatrixRow[row] & (1 << col)) == 0;

    // We could have also checked the column matrix
    if (KBD_DEBUG) {
        assert(result1 == ((kbMatrixCol[col] & (1 << row)) == 0));
    }

    return result1;
}

bool
Keyboard::restoreIsPressed()
{
    return cpu.nmiLine & INTSRC_KBD;
}

void
Keyboard::toggle(long nr)
{
    if (isPressed(nr)) {
        release(nr);
    } else {
        press(nr);
    }
}

void
Keyboard::toggle(u8 row, u8 col)
{
    if (isPressed(row, col)) {
        releaseRowCol(row, col);
    } else {
        pressRowCol(row,col);
    }
}

void
Keyboard::scheduleKeyPress(long nr, i64 delay)
{
    synchronized { _scheduleKeyAction(true, nr, delay); }
}

void
Keyboard::scheduleKeyPress(u8 row, u8 col, i64 delay)
{
    synchronized { _scheduleKeyAction(true, row, col, delay); }
}

void
Keyboard::scheduleKeyRelease(long nr, i64 delay)
{
    synchronized { _scheduleKeyAction(false, nr, delay); }
}

void
Keyboard::scheduleKeyRelease(u8 row, u8 col, i64 delay)
{
    synchronized { _scheduleKeyAction(false, row, col, delay); }
}

void
Keyboard::addDelay(i64 delay)
{
    if (actions.empty()) {
        this->delay = delay;
    } else {
        actions.back().delay += delay;
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
Keyboard::_scheduleKeyAction(bool press, long nr, i64 delay)
{
    debug(KBD_DEBUG, "Recording %d %ld %lld\n", press, nr, delay);

    addDelay(delay);
    actions.push(KeyAction(press, nr, 0));
}

void
Keyboard::_scheduleKeyAction(bool press, u8 row, u8 col, i64 delay)
{
    debug(KBD_DEBUG, "Recording %d %d %d %lld\n", press, row, col, delay);
    
    addDelay(delay);
    actions.push(KeyAction(press, row, col, 0));
}

void
Keyboard::vsyncHandler()
{
    // Only proceed if the timer fires
    if (delay--) return;

    // Process all pending auto-typing events
    synchronized {
        
        // printf("delay = %d empty: %d count: %d\n", delay, actions.empty(), actions.size());
        
        while (!actions.empty()) {
                        
            KeyAction action = actions.front();
            actions.pop();

            trace(KBD_DEBUG, "%s key (%d,%d) next: %lld\n",
                  action.press ? "Pressing" : "Releasing",
                  action.row, action.col, action.delay);
            
            action.press ? _press(action.nr) : _release(action.nr);
            delay = action.delay;
            
            if (delay) break;
        }
        delay--;
    }
}

const u8
Keyboard::rowcol[66][2] = {
    
    // First physical row
    {7, 1}, {7, 0}, {7, 3}, {1, 0}, {1, 3}, {2, 0}, {2, 3}, {3, 0},
    {3, 3}, {4, 0}, {4, 3}, {5, 0}, {5, 3}, {6, 0}, {6, 3}, {0, 0},
    {0, 4},
    
    // Second physical row
    {7, 2}, {7, 6}, {1, 1}, {1, 6}, {2, 1}, {2, 6}, {3, 1}, {3, 6},
    {4, 1}, {4, 6}, {5, 1}, {5, 6}, {6, 1}, {6, 6}, {9, 9}, {0, 5},
    
    // Third physical row
    {7, 7}, {9, 9}, {1, 2}, {1, 5}, {2, 2}, {2, 5}, {3, 2}, {3, 5},
    {4, 2}, {4, 5}, {5, 2}, {5, 5}, {6, 2}, {6, 5}, {0, 1}, {0, 6},
    
    // Fourth physical row
    {7, 5}, {1, 7}, {1, 4}, {2, 7}, {2, 4}, {3, 7}, {3, 4}, {4, 7},
    {4, 4}, {5, 7}, {5, 4}, {6, 7}, {6, 4}, {0, 7}, {0, 2}, {0, 3},
    
    // Fifth physical row
    {7, 4}
};
