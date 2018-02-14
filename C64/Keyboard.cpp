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
    
	for (int i = 0; i < 256; i++) {
		rowcolmap[i] = 0xFFFF;
	}
	
    //          0        1         2         3         4         5         6         7
    //
    //    0    DEL    RETURN   CUR LR      F7        F1        F3        F5       CUR UD
    //    1     3        W        A         4         Z         S         E       LSHIFT
    //    2     5        R        D         6         C         F         T         X
    //    3     7        Y        G         8         B         H         U         V
    //    4     9        I        J         0         M         K         O         N
    //    5     +        P        L         -         .         :         @         ,
    //    6   POUND      *        ;       HOME     RSHIFT       =         ^         /
    //    7     1       <-       CTRL       2       SPACE       C=        Q        STOP
    rowcolmap[(unsigned char)'\n'] = 0x0001;
    rowcolmap[(unsigned char)' '] = 0x0704;
	rowcolmap[(unsigned char)'*'] = 0x0601;
	rowcolmap[(unsigned char)'+'] = 0x0500;
    rowcolmap[(unsigned char)','] = 0x0507; rowcolmap[(unsigned char)'<'] = 0x0507 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'-'] = 0x0503;
	rowcolmap[(unsigned char)'.'] = 0x0504; rowcolmap[(unsigned char)'>'] = 0x0504 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'/'] = 0x0607; rowcolmap[(unsigned char)'?'] = 0x0607 | C64KEY_SHIFT;
    rowcolmap[(unsigned char)'^'] = 0x0701;
    rowcolmap[(unsigned char)'~'] = 0x0701;
    rowcolmap[C64KEY_LEFTARROW]   = 0x0701;
    rowcolmap[C64KEY_UPARROW]     = 0x0606;
	rowcolmap[(unsigned char)'0'] = 0x0403;
	rowcolmap[(unsigned char)'1'] = 0x0700; rowcolmap[(unsigned char)'!'] = 0x0700 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'2'] = 0x0703; rowcolmap[(unsigned char)'"'] = 0x0703 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'3'] = 0x0100; rowcolmap[(unsigned char)'#'] = 0x0100 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'4'] = 0x0103; rowcolmap[(unsigned char)'$'] = 0x0103 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'5'] = 0x0200; rowcolmap[(unsigned char)'%'] = 0x0200 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'6'] = 0x0203; rowcolmap[(unsigned char)'&'] = 0x0203 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'7'] = 0x0300; rowcolmap[(unsigned char)'\'']= 0x0300 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'8'] = 0x0303; rowcolmap[(unsigned char)'('] = 0x0303 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'9'] = 0x0400; rowcolmap[(unsigned char)')'] = 0x0400 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)':'] = 0x0505; rowcolmap[(unsigned char)'['] = 0x0505 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)';'] = 0x0602; rowcolmap[(unsigned char)']'] = 0x0602 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'='] = 0x0605;
	rowcolmap[(unsigned char)'@'] = 0x0506;
    rowcolmap[(unsigned char)'a'] = 0x0102; rowcolmap[(unsigned char)'A'] = 0x0102 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'b'] = 0x0304; rowcolmap[(unsigned char)'B'] = 0x0304 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'c'] = 0x0204; rowcolmap[(unsigned char)'C'] = 0x0204 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'d'] = 0x0202; rowcolmap[(unsigned char)'D'] = 0x0202 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'e'] = 0x0106; rowcolmap[(unsigned char)'E'] = 0x0106 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'f'] = 0x0205; rowcolmap[(unsigned char)'F'] = 0x0205 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'g'] = 0x0302; rowcolmap[(unsigned char)'G'] = 0x0302 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'h'] = 0x0305; rowcolmap[(unsigned char)'H'] = 0x0305 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'i'] = 0x0401; rowcolmap[(unsigned char)'I'] = 0x0401 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'j'] = 0x0402; rowcolmap[(unsigned char)'J'] = 0x0402 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'k'] = 0x0405; rowcolmap[(unsigned char)'K'] = 0x0405 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'l'] = 0x0502; rowcolmap[(unsigned char)'L'] = 0x0502 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'m'] = 0x0404; rowcolmap[(unsigned char)'M'] = 0x0404 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'n'] = 0x0407; rowcolmap[(unsigned char)'N'] = 0x0407 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'o'] = 0x0406; rowcolmap[(unsigned char)'O'] = 0x0406 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'p'] = 0x0501; rowcolmap[(unsigned char)'P'] = 0x0501 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'q'] = 0x0706; rowcolmap[(unsigned char)'Q'] = 0x0706 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'r'] = 0x0201; rowcolmap[(unsigned char)'R'] = 0x0201 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'s'] = 0x0105; rowcolmap[(unsigned char)'S'] = 0x0105 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'t'] = 0x0206; rowcolmap[(unsigned char)'T'] = 0x0206 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'u'] = 0x0306; rowcolmap[(unsigned char)'U'] = 0x0306 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'v'] = 0x0307; rowcolmap[(unsigned char)'V'] = 0x0307 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'w'] = 0x0101; rowcolmap[(unsigned char)'W'] = 0x0101 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'x'] = 0x0207; rowcolmap[(unsigned char)'X'] = 0x0207 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'y'] = 0x0301; rowcolmap[(unsigned char)'Y'] = 0x0301 | C64KEY_SHIFT;
	rowcolmap[(unsigned char)'z'] = 0x0104; rowcolmap[(unsigned char)'Z'] = 0x0104 | C64KEY_SHIFT;
    rowcolmap[C64KEY_F1]   = 0x0004; rowcolmap[C64KEY_F2]   = 0x0004 | C64KEY_SHIFT;
    rowcolmap[C64KEY_F3]   = 0x0005; rowcolmap[C64KEY_F4]   = 0x0005 | C64KEY_SHIFT;
    rowcolmap[C64KEY_F5]   = 0x0006; rowcolmap[C64KEY_F6]   = 0x0006 | C64KEY_SHIFT;
    rowcolmap[C64KEY_F7]   = 0x0003; rowcolmap[C64KEY_F8]   = 0x0003 | C64KEY_SHIFT;
    rowcolmap[C64KEY_POUND] = 0x0600;
    rowcolmap[C64KEY_HOME] = 0x0603; rowcolmap[C64KEY_CLR]  = 0x0603 | C64KEY_SHIFT;
    rowcolmap[C64KEY_DEL]  = 0x0000; rowcolmap[C64KEY_INST] = 0x0000 | C64KEY_SHIFT;
    rowcolmap[C64KEY_RET]  = 0x0001; rowcolmap[C64KEY_CL]   = 0x0002 | C64KEY_SHIFT;
    rowcolmap[C64KEY_CR]   = 0x0002; rowcolmap[C64KEY_CU]   = 0x0007 | C64KEY_SHIFT;
    rowcolmap[C64KEY_CD]   = 0x0007;
    rowcolmap[C64KEY_RUNSTOP] = 0x0707;
    
    // Register snapshot items
    SnapshotItem items[] = {

        { &kbMatrixRow, sizeof(kbMatrixRow), CLEAR_ON_RESET | BYTE_FORMAT },
        { &kbMatrixCol, sizeof(kbMatrixCol), CLEAR_ON_RESET | BYTE_FORMAT },
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
}

uint8_t
Keyboard::getRowValues(uint8_t columnMask)
{
	uint8_t result = 0xff;
		
	for (int i = 0; i < 8; i++) {
		if ((columnMask & (1 << i)) == 0) {
			result &= kbMatrixRow[i];
		}
	}
	
	return result;
}

uint8_t
Keyboard::getColumnValues(uint8_t rowMask)
{
    uint8_t result = 0xff;
    
    for (int i = 0; i < 8; i++) {
        if ((rowMask & (1 << i)) == 0) {
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
    
    kbMatrixRow[row] &= 255 - (1 << col);
    kbMatrixCol[col] &= 255 - (1 << row);

    // debug("Set(%d %d)\n",row,col);
    // dumpState();
}

void
Keyboard::pressKey(C64KeyFingerprint key)
{
    // debug("Pressing (%ld)\n", (long)key);

    // Check for shift key (soleley pressed)
    if (key == C64KEY_SHIFT) {
        pressShiftKey();
        dumpState();
        return;
    }
    
    // Check for restore key
    if (key == C64KEY_RESTORE) {
        // debug("RESTORE");
        pressRestoreKey();
        return;
    }
        
    // Check for commodore key flag
    if (key & C64KEY_COMMODORE) {
        pressCommodoreKey();
    }

    // Check for CTRL key flag
    if (key & C64KEY_CTRL) {
        pressCtrlKey();
    }
    
    key &= 0xFF;
    
    // Only proceed if key is known and mapped
    if (rowcolmap[key] == 0xFFFF)
        return;
    
    uint8_t row = (rowcolmap[key] & 0x0F00) >> 8;
    uint8_t col = (rowcolmap[key] & 0x000F);
        
	if (rowcolmap[key] & C64KEY_SHIFT) {
		pressShiftKey();
	}

    // debug("Pressing (%ld,%ld)\n", (long)(row), (long)(col));
	pressKey(row, col);
}

void
Keyboard::pressRestoreKey()
{
    c64->cpu.setNMILineReset();
}

void
Keyboard::releaseKey(uint8_t row, uint8_t col)
{
    assert(row < 8);
    assert(col < 8);
    
    kbMatrixRow[row] |= (1 << col);
    kbMatrixCol[col] |= (1 << row);

    // debug("Unset(%d %d)\n",row,col);
    // dumpState();
}

void
Keyboard::releaseKey(C64KeyFingerprint key)
{
    // debug("Releasing (%ld)\n", (long)key);
    
    // Check for shift key (soleley released)
    if (key == C64KEY_SHIFT) {
        releaseShiftKey();
        dumpState();
        return;
    }

    // Check for restore key
    if (key == C64KEY_RESTORE) {
        releaseRestoreKey();
        return;
    }
    
    // Check for commodore key flag
    if (key & C64KEY_COMMODORE) {
        releaseCommodoreKey();
    }
    
    // Check for CTRL key flag
    if (key & C64KEY_CTRL) {
        releaseCtrlKey();
    }
    
    key &= 0xFF;

    // Only proceed if key is known and mapped
    if (rowcolmap[key] == 0xFFFF)
        return;

    uint8_t row = (rowcolmap[key] & 0x0F00) >> 8;
    uint8_t col = (rowcolmap[key] & 0x000F);

    if (rowcolmap[key] & C64KEY_SHIFT) {
		releaseShiftKey();
	}

    // debug("Releasing (%ld,%ld)\n", (long)(row), (long)(col));
	releaseKey(row, col);
}

void
Keyboard::releaseRestoreKey()
{
    c64->cpu.clearNMILineReset();
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


