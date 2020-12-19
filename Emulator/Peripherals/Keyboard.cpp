// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

void 
Keyboard::_reset() 
{
    RESET_SNAPSHOT_ITEMS

	// Reset the keyboard matrix
    releaseAll();
}

void 
Keyboard::_dump()
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
Keyboard::press(long nr, i64 duration)
{
    assert(nr < 66);
    
    switch (nr) {
        case 34: toggleShiftLock(); return;
        case 31: pressRestore(duration); return;
        default: pressRowCol(rowcol[nr][0], rowcol[nr][1], duration);
    }
}

void
Keyboard::pressRowCol(u8 row, u8 col, i64 duration)
{
    debug(KBD_DEBUG, "pressKey(%d,%d, [%lld])\n", row, col, duration);

    assert(row < 8);
    assert(col < 8);
        
    kbMatrixRow[row] &= ~(1 << col);
    kbMatrixCol[col] &= ~(1 << row);
    clearCnt = duration;
}

void
Keyboard::pressRestore(i64 duration)
{
    debug(KBD_DEBUG, "pressRestoreKey()\n");

    cpu.pullDownNmiLine(INTSRC_KBD);
    clearCnt = duration;
}

void
Keyboard::release(long nr)
{
    assert(nr < 66);

    switch (nr) {
        case 34: releaseShiftLock(); return;
        case 31: releaseRestore(); return;
        default: releaseRowCol(rowcol[nr][0], rowcol[nr][1]);
    }
}

void
Keyboard::releaseRowCol(u8 row, u8 col)
{
    debug(KBD_DEBUG, "releaseKey(%d,%d)\n", row, col);

    assert(row < 8);
    assert(col < 8);
    
    // Only release right shift key if shift lock is not pressed
    if (row == 6 && col == 4 && shiftLock)
        return;
    
    kbMatrixRow[row] |= (1 << col);
    kbMatrixCol[col] |= (1 << row);
}

void
Keyboard::releaseRestore()
{
    debug(KBD_DEBUG, "releaseRestoreKey()\n");
    
    cpu.releaseNmiLine(INTSRC_KBD);
}

void
Keyboard::releaseAll()
{
    for (unsigned i = 0; i < 8; i++) {
        kbMatrixRow[i] = kbMatrixCol[i] = 0xFF;
    }
    releaseRestore();
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
    // We can either check the row or column matrix
    bool result1 = (kbMatrixRow[row] & (1 << col)) == 0;
    // bool result2 = (kbMatrixCol[col] & (1 << row)) == 0;
    // assert(result1 == result2);

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

bool
Keyboard::inUpperCaseMode()
{
    return (vic.spypeek(0x18) & 0x02) == 0;
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

void
Keyboard::vsyncHandler()
{
    if (clearCnt) {
        if (--clearCnt == 0) {
            releaseAll();
            messageQueue.put(MSG_KB_AUTO_RELEASE);
        }
    }
}
