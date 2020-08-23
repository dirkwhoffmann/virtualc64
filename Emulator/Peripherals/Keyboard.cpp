// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

Keyboard::Keyboard(C64 &ref) : C64Component(ref)
{
	setDescription("Keyboard");
    
    // Register snapshot items
    SnapshotItem items[] = {

        { &kbMatrixRow, sizeof(kbMatrixRow), CLEAR_ON_RESET | BYTE_ARRAY },
        { &kbMatrixCol, sizeof(kbMatrixCol), CLEAR_ON_RESET | BYTE_ARRAY },
        { &shiftLock,   sizeof(shiftLock),   CLEAR_ON_RESET },
        { NULL,         0,                   0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

void 
Keyboard::_reset() 
{
    RESET_SNAPSHOT_ITEMS

	// Release all keys (resets the keyboard matrix)
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

void
Keyboard::setShiftLock(bool value)
{
    if (value != shiftLock) {
        shiftLock = value;
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
Keyboard::pressKey(u8 row, u8 col)
{
    debug(KBD_DEBUG, "pressKey(%d,%d)\n", row, col);

    assert(row < 8);
    assert(col < 8);
        
    kbMatrixRow[row] &= ~(1 << col);
    kbMatrixCol[col] &= ~(1 << row);
}

void
Keyboard::pressRestoreKey()
{
    debug(KBD_DEBUG, "pressRestoreKey()\n");

    cpu.pullDownNmiLine(INTSRC_KBD);
}

void
Keyboard::releaseKey(u8 row, u8 col)
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
Keyboard::releaseRestoreKey()
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
}

bool
Keyboard::keyIsPressed(u8 row, u8 col)
{
    // We can either check the row or column matrix
    bool result1 = (kbMatrixRow[row] & (1 << col)) == 0;
    // bool result2 = (kbMatrixCol[col] & (1 << row)) == 0;
    // assert(result1 == result2);

    return result1;
}

void
Keyboard::toggleKey(u8 row, u8 col)
{
    if (keyIsPressed(row, col)) {
        releaseKey(row, col);
    } else {
        pressKey(row,col);
    }
}

bool
Keyboard::inUpperCaseMode()
{
    return (vic.spypeek(0x18) & 0x02) == 0;
}

