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
	debug("Creating keyboard at address %p...\n", this);
}

void 
Keyboard::reset() 
{
	debug("  Resetting keyboard...\n");

	// Reset keyboard matrix (0 = pressed, 1 = not pressed)
	for (int i = 0; i < 8; i++) {
		kbMatrix[i] = 0xff;
	}		
}

bool
Keyboard::load(FILE *file)
{
	for (int i = 0; i < sizeof(kbMatrix); i++) {
		kbMatrix[i] = read8(file);
	}
	return true;
}

bool
Keyboard::save(FILE *file)
{
	for (int i = 0; i < sizeof(kbMatrix); i++) {
		write8(file, kbMatrix[i]);
	}
	return true;
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
	// debug("Key %d %d pressed\n", row, col);
}

void Keyboard::pressKey(char c)
{
	debug("Not implemented yet");
	assert(0);
}

void Keyboard::releaseKey(uint8_t row, uint8_t col)
{
	if (row < 8 && col < 8) {
		kbMatrix[row] |= (1 << col);
	}
}

void Keyboard::releaseKey(char c)
{
	debug("Not implemented yet");
	assert(0);
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
	debug("typeFormat\n");
	
	// OPEN 1,8,15,"N:TEST, ID": CLOSE 1<RETURN>
	int i;
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
