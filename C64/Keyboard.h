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

#ifndef _KEYBOARD_INC
#define _KEYBOARD_INC

#include "VirtualComponent.h"

//! The virtual keyboard of a C64
/*! This class manages the keyboard matrix of the virtual C64. 
	Keyboard management works as follows: When the GUI recognizes a pressed or release key,
	it calls one of the functions in this class to tell the virtual keyboard about the event.
	The called functions does nothing more than clearing or setting a bit in the keyboard matrix.
	Communication with the virtual computer is managed solely by the CIA chip. When a special
	CIA register is peeked, method \a getRowValues is called which finally brings the contents
	of the keyboard matrix into the virtual C64.
*/
class Keyboard : public VirtualComponent {
	
	//! The C64 keyboard matrix
	/*! The C64 maintains a 8x8 matrix. Each key corresponds to a specific bit in the matrix and
	    is uniquely determined by a row and a column value. 
	*/
	uint8_t kbMatrix[8];
			
public:
    //! Special keys
    enum C64Key {
        C64KEY_F1 = 0x80,
        C64KEY_F2,
        C64KEY_F3,
        C64KEY_F4,
        C64KEY_F5,
        C64KEY_F6,
        C64KEY_F7,
        C64KEY_F8,
        C64KEY_DEL,
        C64KEY_INS,
        C64KEY_RET,
        C64KEY_CL,
        C64KEY_CR,
        C64KEY_CU,
        C64KEY_CD,
        C64KEY_COMMODORE = 0x0100 // flag that is combinable with all other keys
    };
    
	//! Constructor
	Keyboard();

	//! Destructor
	~Keyboard();

	//! Reset 
	void reset(C64 *c64);
    	
	//! Dump internal state to console
	void dumpState();	

	//! Inform keyboard about a pressed key (by keycode)
	void pressKey(uint8_t row, uint8_t col);
    
	//! Inform keyboard about a pressed key (by character)
	void pressKey(int c);
    
	//! Inform keyboard that the Shift key has been pressen
	void pressShiftKey() { pressKey(1,7); }
    
	//! Inform keyboard that the Commodore key has been pressen
	void pressCommodoreKey() { pressKey(7,5); }
    
	//! Inform keyboard that the Runstop key has been pressen
	void pressRunstopKey() { pressKey(7,7); }
    
    //! Inform keyboard that the Clear key has been pressen
    void pressClearKey() { pressShiftKey(); pressKey(6,3); }
    
    //! Inform keyboard that the Home key has been pressen
    void pressHomeKey() { pressKey(6,3); }
    
    //! Inform keyboard that the Insert key has been pressen
    void pressInsertKey() { pressShiftKey(); pressKey(0,0); }
	
	//! Inform keyboard about a released key (by keycode)
	void releaseKey(uint8_t row, uint8_t col);
    
	//! Inform keyboard about a pressed key (by character)
	void releaseKey(int c);
    
	//! Inform keyboard that the Shift key has been released
	void releaseShiftKey() { releaseKey(1,7); }
    
	//! Inform keyboard that the Commodore key has been released
	void releaseCommodoreKey() { releaseKey(7,5); }
    
	//! Inform keyboard that the Runstop key has been released
	void releaseRunstopKey() { releaseKey(7,7); }
    
    //! Inform keyboard that the Clear key has been released
    void releaseClearKey() { releaseKey(6,3); releaseShiftKey(); }
    
    //! Inform keyboard that the Home key has been released
    void releaseHomeKey() { releaseKey(6,3); }
    
    //! Inform keyboard that the Insert key has been released
    void releaseInsertKey() { releaseKey(0,0); releaseShiftKey(); }

    //! Clear keyboard matrix
	void releaseAll() { for (int i=0; i<8; i++) kbMatrix[i] = 0xff; }
    
	//! Read the keyboard matrix
	/*! /param columnMask determines the column bits to be read. */
	uint8_t getRowValues(uint8_t columnMask);
	
private:
	
	//! Mapping from characters to the C64 row/column format
	uint16_t rowcolmap[256];
};
	
#endif
