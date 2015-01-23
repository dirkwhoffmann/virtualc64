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

#define SHIFT_FLAG 0x1000

Keyboard::Keyboard(C64 *c64)
{
	name = "Keyboard";

	debug(2, "Creating keyboard at address %p...\n", this);
	
    this->c64 = c64;
    
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
    //    6    LIRA      *        ;       HOME     RSHIFT       =         ^         /
    //    7     1       <-       CTRL       2       SPACE       C=        Q        STOP
    rowcolmap[(unsigned)'\n'] = 0x0001;
    rowcolmap[(unsigned)' '] = 0x0704;
	rowcolmap[(unsigned)'*'] = 0x0601;
	rowcolmap[(unsigned)'+'] = 0x0500;
	rowcolmap[(unsigned)','] = 0x0507; rowcolmap[(unsigned)'<'] = 0x0507 | SHIFT_FLAG;
	rowcolmap[(unsigned)'-'] = 0x0503;
	rowcolmap[(unsigned)'.'] = 0x0504; rowcolmap[(unsigned)'>'] = 0x0504 | SHIFT_FLAG;
	rowcolmap[(unsigned)'/'] = 0x0607; rowcolmap[(unsigned)'?'] = 0x0607 | SHIFT_FLAG;
    rowcolmap[(unsigned)'^'] = 0x0701;
	rowcolmap[(unsigned)'0'] = 0x0403;
	rowcolmap[(unsigned)'1'] = 0x0700; rowcolmap[(unsigned)'!'] = 0x0700 | SHIFT_FLAG;
	rowcolmap[(unsigned)'2'] = 0x0703; rowcolmap[(unsigned)'"'] = 0x0703 | SHIFT_FLAG;
	rowcolmap[(unsigned)'3'] = 0x0100; rowcolmap[(unsigned)'#'] = 0x0100 | SHIFT_FLAG;
	rowcolmap[(unsigned)'4'] = 0x0103; rowcolmap[(unsigned)'$'] = 0x0103 | SHIFT_FLAG;
	rowcolmap[(unsigned)'5'] = 0x0200; rowcolmap[(unsigned)'%'] = 0x0200 | SHIFT_FLAG;
	rowcolmap[(unsigned)'6'] = 0x0203; rowcolmap[(unsigned)'&'] = 0x0203 | SHIFT_FLAG;
	rowcolmap[(unsigned)'7'] = 0x0300; rowcolmap[(unsigned)'\'']= 0x0300 | SHIFT_FLAG;
	rowcolmap[(unsigned)'8'] = 0x0303; rowcolmap[(unsigned)'('] = 0x0303 | SHIFT_FLAG;
	rowcolmap[(unsigned)'9'] = 0x0400; rowcolmap[(unsigned)')'] = 0x0400 | SHIFT_FLAG;
	rowcolmap[(unsigned)':'] = 0x0505; rowcolmap[(unsigned)'['] = 0x0505 | SHIFT_FLAG;
	rowcolmap[(unsigned)';'] = 0x0602; rowcolmap[(unsigned)']'] = 0x0602 | SHIFT_FLAG;
	rowcolmap[(unsigned)'='] = 0x0605;
	rowcolmap[(unsigned)'@'] = 0x0506;
    rowcolmap[(unsigned)'a'] = 0x0102; rowcolmap[(unsigned)'A'] = 0x0102 | SHIFT_FLAG;
	rowcolmap[(unsigned)'b'] = 0x0304; rowcolmap[(unsigned)'B'] = 0x0304 | SHIFT_FLAG;
	rowcolmap[(unsigned)'c'] = 0x0204; rowcolmap[(unsigned)'C'] = 0x0204 | SHIFT_FLAG;
	rowcolmap[(unsigned)'d'] = 0x0202; rowcolmap[(unsigned)'D'] = 0x0202 | SHIFT_FLAG;
	rowcolmap[(unsigned)'e'] = 0x0106; rowcolmap[(unsigned)'E'] = 0x0106 | SHIFT_FLAG;
	rowcolmap[(unsigned)'f'] = 0x0205; rowcolmap[(unsigned)'F'] = 0x0205 | SHIFT_FLAG;
	rowcolmap[(unsigned)'g'] = 0x0302; rowcolmap[(unsigned)'G'] = 0x0302 | SHIFT_FLAG;
	rowcolmap[(unsigned)'h'] = 0x0305; rowcolmap[(unsigned)'H'] = 0x0305 | SHIFT_FLAG;
	rowcolmap[(unsigned)'i'] = 0x0401; rowcolmap[(unsigned)'I'] = 0x0401 | SHIFT_FLAG;
	rowcolmap[(unsigned)'j'] = 0x0402; rowcolmap[(unsigned)'J'] = 0x0402 | SHIFT_FLAG;
	rowcolmap[(unsigned)'k'] = 0x0405; rowcolmap[(unsigned)'K'] = 0x0405 | SHIFT_FLAG;
	rowcolmap[(unsigned)'l'] = 0x0502; rowcolmap[(unsigned)'L'] = 0x0502 | SHIFT_FLAG;
	rowcolmap[(unsigned)'m'] = 0x0404; rowcolmap[(unsigned)'M'] = 0x0404 | SHIFT_FLAG;
	rowcolmap[(unsigned)'n'] = 0x0407; rowcolmap[(unsigned)'N'] = 0x0407 | SHIFT_FLAG;
	rowcolmap[(unsigned)'o'] = 0x0406; rowcolmap[(unsigned)'O'] = 0x0406 | SHIFT_FLAG;
	rowcolmap[(unsigned)'p'] = 0x0501; rowcolmap[(unsigned)'P'] = 0x0501 | SHIFT_FLAG;
	rowcolmap[(unsigned)'q'] = 0x0706; rowcolmap[(unsigned)'Q'] = 0x0706 | SHIFT_FLAG;
	rowcolmap[(unsigned)'r'] = 0x0201; rowcolmap[(unsigned)'R'] = 0x0201 | SHIFT_FLAG;
	rowcolmap[(unsigned)'s'] = 0x0105; rowcolmap[(unsigned)'S'] = 0x0105 | SHIFT_FLAG;
	rowcolmap[(unsigned)'t'] = 0x0206; rowcolmap[(unsigned)'T'] = 0x0206 | SHIFT_FLAG;
	rowcolmap[(unsigned)'u'] = 0x0306; rowcolmap[(unsigned)'U'] = 0x0306 | SHIFT_FLAG;
	rowcolmap[(unsigned)'v'] = 0x0307; rowcolmap[(unsigned)'V'] = 0x0307 | SHIFT_FLAG;
	rowcolmap[(unsigned)'w'] = 0x0101; rowcolmap[(unsigned)'W'] = 0x0101 | SHIFT_FLAG;
	rowcolmap[(unsigned)'x'] = 0x0207; rowcolmap[(unsigned)'X'] = 0x0207 | SHIFT_FLAG;
	rowcolmap[(unsigned)'y'] = 0x0301; rowcolmap[(unsigned)'Y'] = 0x0301 | SHIFT_FLAG;
	rowcolmap[(unsigned)'z'] = 0x0104; rowcolmap[(unsigned)'Z'] = 0x0104 | SHIFT_FLAG;
    rowcolmap[C64KEY_F1] = 0x0004; rowcolmap[C64KEY_F2] = 0x0004 | SHIFT_FLAG;
    rowcolmap[C64KEY_F3] = 0x0005; rowcolmap[C64KEY_F4] = 0x0005 | SHIFT_FLAG;
    rowcolmap[C64KEY_F5] = 0x0006; rowcolmap[C64KEY_F6] = 0x0006 | SHIFT_FLAG;
    rowcolmap[C64KEY_F7] = 0x0003; rowcolmap[C64KEY_F8] = 0x0003 | SHIFT_FLAG;
    rowcolmap[C64KEY_DEL] = 0x0000;
    rowcolmap[C64KEY_INS] = 0x0000 | SHIFT_FLAG;
    rowcolmap[C64KEY_RET] = 0x0001;
    rowcolmap[C64KEY_CL] = 0x0002 | SHIFT_FLAG;
    rowcolmap[C64KEY_CR] = 0x0002;
    rowcolmap[C64KEY_CU] = 0x0007 | SHIFT_FLAG;
    rowcolmap[C64KEY_CD] = 0x0007;
}

Keyboard::~Keyboard()
{
}

void 
Keyboard::reset() 
{
	debug(2, "  Resetting keyboard...\n");

	// Reset keyboard matrix (0 = pressed, 1 = not pressed)
	for (int i = 0; i < 8; i++) {
		kbMatrix[i] = 0xff;
	}		
}

uint32_t
Keyboard::stateSize()
{
    return sizeof(kbMatrix);
}

void
Keyboard::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
	
	for (unsigned i = 0; i < sizeof(kbMatrix); i++) {
		kbMatrix[i] = read8(buffer);
	}

    debug(2, "  Keyboard state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
Keyboard::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;

	for (unsigned i = 0; i < sizeof(kbMatrix); i++) {
		write8(buffer, kbMatrix[i]);
	}

    debug(2, "  Keyboard state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void 
Keyboard::dumpState()
{
	msg("Keyboard:\n");
	msg("---------\n\n");
	msg("Keyboard matrix: ");
	for (int i = 0; i < 8; i++) {
		msg("%d %d %d %d %d %d %d %d\n                 ", 
			  (kbMatrix[i] & 0x01) != 0, (kbMatrix[i] & 0x02) != 0, (kbMatrix[i] & 0x04) != 0, (kbMatrix[i] & 0x08) != 0,
			  (kbMatrix[i] & 0x10) != 0, (kbMatrix[i] & 0x20) != 0, (kbMatrix[i] & 0x40) != 0, (kbMatrix[i] & 0x80) != 0);				
	}
	msg("\n");
}

uint8_t Keyboard::getRowValues(uint8_t columnMask)
{
	uint8_t result = 0xff;
		
	for (int i = 0; i < 8; i++) {
		if ((columnMask & (1 << i)) == 0) {
			result &= kbMatrix[i];
		}
	}
	
	return result;
}

void Keyboard::pressKey(uint8_t row, uint8_t col)
{
	if (row < 8 && col < 8)
		kbMatrix[row] &= 255 - (1 << col);
}

void Keyboard::pressKey(int c)
{
    // Check for commodore key flag
    if (c & C64KEY_COMMODORE) {
        pressCommodoreKey();
    }

    c &= 0xFF;
    
    // Only proceed if key is known and mapped
    if (rowcolmap[c] == 0xFFFF)
        return;
    
    uint8_t row = (rowcolmap[c] & 0x0F00) >> 8;
    uint8_t col = (rowcolmap[c] & 0x000F);
        
	if (rowcolmap[c] & SHIFT_FLAG) {
		pressShiftKey();
	}

    // debug("Pressing (%ld,%ld)\n", (long)(row), (long)(col));
	pressKey(row, col);
}

void Keyboard::releaseKey(uint8_t row, uint8_t col)
{
	if (row < 8 && col < 8) {
		kbMatrix[row] |= (1 << col);
	}
}

void Keyboard::releaseKey(int c)
{
    // Check for commodore key flag
    if (c & C64KEY_COMMODORE) {
        releaseCommodoreKey();
    }
    
    c &= 0xFF;

    // Only proceed if key is known and mapped
    if (rowcolmap[c] == 0xFFFF)
        return;

    uint8_t row = (rowcolmap[c] & 0x0F00) >> 8;
    uint8_t col = (rowcolmap[c] & 0x000F);

    if (rowcolmap[c] & SHIFT_FLAG) {
		releaseShiftKey();
	}

    // debug("Releasing (%ld,%ld)\n", (long)(row), (long)(col));
	releaseKey(row, col);
}

#undef SHIFT_FLAG

