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

Keyboard::Keyboard()
{
	name = "Keyboard";

	debug(2, "Creating keyboard at address %p...\n", this);
	
	for (int i = 0; i < 128; i++) {
		ASCII[i] = 0x0000;
	}
	
	ASCII[(unsigned)' '] = 0x0704;
	ASCII[(unsigned)'*'] = 0x0601;
	ASCII[(unsigned)'+'] = 0x0500;
	ASCII[(unsigned)','] = 0x0507; ASCII[(unsigned)'<'] = 0x0507 | SHIFT_FLAG;
	ASCII[(unsigned)'-'] = 0x0503;
	ASCII[(unsigned)'.'] = 0x0504; ASCII[(unsigned)'>'] = 0x0504 | SHIFT_FLAG;
	ASCII[(unsigned)'/'] = 0x0607; ASCII[(unsigned)'?'] = 0x0607 | SHIFT_FLAG;
	ASCII[(unsigned)'0'] = 0x0403;
	ASCII[(unsigned)'1'] = 0x0700; ASCII[(unsigned)'!'] = 0x0700 | SHIFT_FLAG;
	ASCII[(unsigned)'2'] = 0x0703; ASCII[(unsigned)'"'] = 0x0703 | SHIFT_FLAG;
	ASCII[(unsigned)'3'] = 0x0100; ASCII[(unsigned)'#'] = 0x0100 | SHIFT_FLAG;
	ASCII[(unsigned)'4'] = 0x0103; ASCII[(unsigned)'$'] = 0x0103 | SHIFT_FLAG;
	ASCII[(unsigned)'5'] = 0x0200; ASCII[(unsigned)'%'] = 0x0200 | SHIFT_FLAG;
	ASCII[(unsigned)'6'] = 0x0203; ASCII[(unsigned)'&'] = 0x0203 | SHIFT_FLAG;
	ASCII[(unsigned)'7'] = 0x0300; ASCII[(unsigned)'\'']= 0x0300 | SHIFT_FLAG;
	ASCII[(unsigned)'8'] = 0x0303; ASCII[(unsigned)'('] = 0x0303 | SHIFT_FLAG;
	ASCII[(unsigned)'9'] = 0x0400; ASCII[(unsigned)')'] = 0x0400 | SHIFT_FLAG;
	ASCII[(unsigned)':'] = 0x0505; ASCII[(unsigned)'['] = 0x0505 | SHIFT_FLAG;
	ASCII[(unsigned)';'] = 0x0602; ASCII[(unsigned)']'] = 0x0602 | SHIFT_FLAG;
	ASCII[(unsigned)'='] = 0x0605;
	ASCII[(unsigned)'@'] = 0x0506;
	ASCII[(unsigned)'a'] = 0x0102;
	ASCII[(unsigned)'b'] = 0x0304;
	ASCII[(unsigned)'c'] = 0x0204;
	ASCII[(unsigned)'d'] = 0x0202;
	ASCII[(unsigned)'e'] = 0x0106;
	ASCII[(unsigned)'f'] = 0x0205;
	ASCII[(unsigned)'g'] = 0x0302;
	ASCII[(unsigned)'h'] = 0x0305;
	ASCII[(unsigned)'i'] = 0x0401;
	ASCII[(unsigned)'j'] = 0x0402;
	ASCII[(unsigned)'k'] = 0x0405;
	ASCII[(unsigned)'l'] = 0x0502;
	ASCII[(unsigned)'m'] = 0x0404;
	ASCII[(unsigned)'n'] = 0x0407;
	ASCII[(unsigned)'o'] = 0x0406;
	ASCII[(unsigned)'p'] = 0x0501;
	ASCII[(unsigned)'q'] = 0x0706;
	ASCII[(unsigned)'r'] = 0x0201;
	ASCII[(unsigned)'s'] = 0x0105;
	ASCII[(unsigned)'t'] = 0x0206;
	ASCII[(unsigned)'u'] = 0x0306;
	ASCII[(unsigned)'v'] = 0x0307;
	ASCII[(unsigned)'w'] = 0x0101;
	ASCII[(unsigned)'x'] = 0x0207;
	ASCII[(unsigned)'y'] = 0x0301;
	ASCII[(unsigned)'z'] = 0x0104;
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

void
Keyboard::loadFromBuffer(uint8_t **buffer)
{
	debug(2, "    Loading keyboard state...\n");
	
	for (unsigned i = 0; i < sizeof(kbMatrix); i++) {
		kbMatrix[i] = read8(buffer);
	}
}

void
Keyboard::saveToBuffer(uint8_t **buffer)
{
	debug(2, "    Saving keyboard state...\n");

	for (unsigned i = 0; i < sizeof(kbMatrix); i++) {
		write8(buffer, kbMatrix[i]);
	}
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

void Keyboard::pressKey(char c)
{
	unsigned i = (unsigned)c;
	if (i <= 127 && ASCII[i] != 0x0000) {
		uint16_t rowcol = ASCII[i];
		if ((rowcol & SHIFT_FLAG) == SHIFT_FLAG) {
			pressShiftKey();
			rowcol &= 0x0FFF; // clear shift flag
		}
		// debug("Pressing %d %d\n", rowcol >> 8, rowcol & 0xFF);
		pressKey(rowcol >> 8, rowcol & 0xFF);
	}
}

void Keyboard::releaseKey(uint8_t row, uint8_t col)
{
	if (row < 8 && col < 8) {
		kbMatrix[row] |= (1 << col);
	}
}

void Keyboard::releaseKey(char c)
{
	unsigned i = (unsigned)c;
	if (i <= 127 && ASCII[i] != 0x0000) {
		uint16_t rowcol = ASCII[i];
		if ((rowcol & SHIFT_FLAG) == SHIFT_FLAG) {
			releaseShiftKey();
			rowcol &= 0x0FFF; // clear shift flag
		}
		releaseKey(rowcol >> 8, rowcol & 0xFF);
	}
}

void Keyboard::typeRun()
{
	// RUN<RETURN>
	uint8_t i, rowcol[] = { 2, 1,   3, 6,    4, 7,   0, 1 }; 

	for (i = 0; i < sizeof(rowcol); i+= 2) {
		pressKey(rowcol[i], rowcol[i+1]);
		usleep(100000);
		releaseKey(rowcol[i], rowcol[i+1]);
	}
}

void Keyboard::typeFormat()
{
	debug(1, "typeFormat\n");
	
	// OPEN 1,8,15,"N:TEST, ID": CLOSE 1<RETURN>
	unsigned i;
	uint8_t rowcol1[] = { 4,6, 5,1, 1,6, 4,7, 7,4, 7,0, 5,7, 3,3, 5,7, 7,0, 2,0, 5,7 };
	uint8_t rowcol2[] = { 4,7, 5,5, 2,6, 1,6, 1,5, 2,6, 5,7, 7,4, 4,1, 2,2 }; 
	uint8_t rowcol3[] = { 5,5, 7,4, 2,4, 5,2, 4,6, 1,5, 1,6, 7,4, 7,0, 0,1 };
	
	for (i = 0; i < sizeof(rowcol1); i+= 2) {
		pressKey(rowcol1[i], rowcol1[i+1]);
		usleep(20000);
		releaseKey(rowcol1[i], rowcol1[i+1]);
	}
	usleep(20000);
	pressShiftKey(); pressKey(7,3);
	usleep(20000);
	releaseKey(7,3); releaseShiftKey();
	usleep(20000);	

	for (i = 0; i < sizeof(rowcol2); i+= 2) {
		pressKey(rowcol2[i], rowcol2[i+1]);
		usleep(20000);
		releaseKey(rowcol2[i], rowcol2[i+1]);
	}
	usleep(20000);
	pressShiftKey(); pressKey(7,3);
	usleep(20000);
	releaseKey(7,3); releaseShiftKey();
	usleep(20000);	
	for (i = 0; i < sizeof(rowcol3); i+= 2) {
		pressKey(rowcol3[i], rowcol3[i+1]);
		usleep(20000);
		releaseKey(rowcol3[i], rowcol3[i+1]);
	}
}

#undef SHIFT_FLAG

