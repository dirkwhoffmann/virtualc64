/*!
 * @header      Keyboard.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2016 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _KEYBOARD_INC
#define _KEYBOARD_INC

#include "VirtualComponent.h"

/*! @class    The virtual keyboard of a C64
 *  @details  This class manages the keyboard matrix of the virtual C64.
 *            Keyboard management works as follows: When the GUI recognizes a pressed or 
 *            release key, it calls one of the functions in this class to tell the virtual 
 *            keyboard about the event.	The called functions does nothing more than 
 *            clearing or setting a bit in the keyboard matrix.	Communication with the virtual 
 *            computer is managed solely by the CIA chip. When a special CIA register is peeked, 
 *            method getRowValues is called which finally brings the contents of the keyboard 
 *            matrix into the virtual C64.
 */
class Keyboard : public VirtualComponent {
	
	/*! @brief    The C64 keyboard matrix
	 *  @details  The C64 maintains a 8x8 matrix. Each key corresponds to a specific bit in the 
     *            matrix and is uniquely determined by a row and a column value.
     */
	uint8_t kbMatrix[8];
			
public:
    //! @brief    Special keys
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
        C64KEY_ARROW,
        C64KEY_RUNSTOP,
        C64KEY_RESTORE,
        C64KEY_COMMODORE = 0x0100 // flag that is combinable with all other keys
    };
    
	//! @brief    Constructor
	Keyboard();

	//! @brief    Destructor
	~Keyboard();

	//! @brief    Restores the initial state.
	void reset();
    	
	//! @brief    Prints debug information.
	void dumpState();	

	/*! @brief    Presses a key.
     *  @details  The key is identified by its native row and column index.
     */
	void pressKey(uint8_t row, uint8_t col);
    
	//! @brief    Presses a key.
	void pressKey(int c);
    
	//! @brief    Presses the shift hey.
	void pressShiftKey() { pressKey(1,7); }
    
	//! @brief    Presses the commodore key.
	void pressCommodoreKey() { pressKey(7,5); }
    
	//! @brief    Presses the runstop key.
	void pressRunstopKey() { pressKey(7,7); }

    //! @brief    Presses shift and runstop simultaniously.
    void pressShiftRunstopKey() { pressShiftKey(); pressKey(7,7); }

    //! @brief    Presses the restore key.
    void pressRestoreKey();

    //! @brief    Presses the clear key.
    void pressClearKey() { pressShiftKey(); pressKey(6,3); }
    
    //! @brief    Presses the home key.
    void pressHomeKey() { pressKey(6,3); }
    
    //! @brief    Presses the insert key.
    void pressInsertKey() { pressShiftKey(); pressKey(0,0); }
	
	/*! @brief    Releases a pressed key.
     *  @details  The key is identified by its native row and column index.
     */
	void releaseKey(uint8_t row, uint8_t col);
    
	//! @brief    Releases a pressed key.
	void releaseKey(int c);
    
    //! @brief    Releases the shift key.
	void releaseShiftKey() { releaseKey(1,7); }
    
    //! @brief    Releases the commodore key.
	void releaseCommodoreKey() { releaseKey(7,5); }
    
    //! @brief    Releases the runstop key.
	void releaseRunstopKey() { releaseKey(7,7); }

    //! @brief    Releases shift and runstop simultaniously.
    void releaseShiftRunstopKey() { releaseKey(7,7); releaseShiftKey(); }

    //! @brief    Releases the restore key.
    void releaseRestoreKey();

    //! @brief    Releases the clear key.
    void releaseClearKey() { releaseKey(6,3); releaseShiftKey(); }
    
    //! @brief    Releases the home key.
    void releaseHomeKey() { releaseKey(6,3); }
    
    //! @brief    Releases the insert key.
    void releaseInsertKey() { releaseKey(0,0); releaseShiftKey(); }

    //! @brief    Clears the keyboard matrix.
	void releaseAll() { for (int i=0; i<8; i++) kbMatrix[i] = 0xff; }
    
	/*! @brief    Reads the keyboard matrix.
	 *  @param    columnMask  the column bits to be read.
     */
	uint8_t getRowValues(uint8_t columnMask);
	
private:
	
	//! @brief    Mapping from ASCII characters to the C64 row/column format
	uint16_t rowcolmap[256];
};
	
#endif
