/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

#include "C64.h"

Keyboard::Keyboard()
{
	setDescription("Keyboard");
	debug(3, "Creating keyboard at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {

        { &kbMatrixRow, sizeof(kbMatrixRow), CLEAR_ON_RESET | BYTE_FORMAT },
        { &kbMatrixCol, sizeof(kbMatrixCol), CLEAR_ON_RESET | BYTE_FORMAT },
        { &shiftLock,   sizeof(shiftLock),   CLEAR_ON_RESET },
        { NULL,         0,                   0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

Keyboard::~Keyboard()
{
}

void 
Keyboard::reset() 
{
    VirtualComponent::reset();

	// Release all keys (resets the keyboard matrix)
    releaseAll();
}

void 
Keyboard::dumpState()
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

uint8_t
Keyboard::getRowValues(uint8_t columnMask)
{
	uint8_t result = 0xff;
		
	for (unsigned i = 0; i < 8; i++) {
		if (GET_BIT(columnMask, i)) {
			result &= kbMatrixRow[i];
		}
	}
	
	return result;
}

uint8_t
Keyboard::getColumnValues(uint8_t rowMask)
{
    uint8_t result = 0xff;
    
    for (unsigned i = 0; i < 8; i++) {
        if (GET_BIT(rowMask, i)) {
            result &= kbMatrixCol[i];
        }
    }
    
    return result;
}


void
Keyboard::pressKey(uint8_t row, uint8_t col)
{
    assert(row < 8);
    assert(col < 8);
    
    kbMatrixRow[row] &= ~(1 << col);
    kbMatrixCol[col] &= ~(1 << row);

    // debug("Set(%d %d)\n",row,col);
    // dumpState();
}

void
Keyboard::pressRestoreKey()
{
    c64->cpu.pullDownNmiLine(CPU::INTSRC_KEYBOARD);
}

void
Keyboard::releaseKey(uint8_t row, uint8_t col)
{
    assert(row < 8);
    assert(col < 8);
    
    // Only release right shift key if shift lock is not pressed
    if (row == 6 && col == 4 && shiftLock)
        return;
    
    kbMatrixRow[row] |= (1 << col);
    kbMatrixCol[col] |= (1 << row);

    // debug("Unset(%d %d)\n",row,col);
    // dumpState();
}

void
Keyboard::releaseRestoreKey()
{
    c64->cpu.releaseNmiLine(CPU::INTSRC_KEYBOARD);
}

bool
Keyboard::keyIsPressed(uint8_t row, uint8_t col)
{
    // We can either check the row or column matrix
    bool result1 = (kbMatrixRow[row] & (1 << col)) == 0;
    // bool result2 = (kbMatrixCol[col] & (1 << row)) == 0;
    // assert(result1 == result2);

    return result1;
}

void
Keyboard::toggleKey(uint8_t row, uint8_t col)
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
    return (c64->vic.spypeek(0x18) & 0x02) == 0;
}

