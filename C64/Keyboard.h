/*!
 * @header      Keyboard.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2018 Dirk W. Hoffmann
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
 *            clearing or setting a bit in the 8x8 keyboard matrix.Each key corresponds to a
 *            specific bit in the matrix and is uniquely determined by a row and a column value.
 *
 *            Communication with the virtual computer is managed solely by the CIA chip. When a
 *            special CIA register is peeked, method getRowValues is called which finally gets
 *            the contents of the keyboard matrix into the virtual C64.
 */
class Keyboard : public VirtualComponent {

protected:
    
	//! @brief    The C64 keyboard matrix indexed by row
	uint8_t kbMatrixRow[8];

    //! @brief    The C64 keyboard matrix indexed by column
    uint8_t kbMatrixCol[8];

    //! @brief    True iff shift lock is pressed
    bool shiftLock;
    
public:
    
	//! @brief    Constructor
	Keyboard();

	//! @brief    Destructor
	~Keyboard();

	//! @brief    Methods from VirtualComponent
	void reset();
	void dumpState();	

    /*! @brief    Checks if a certain key is currently pressed.
     *  @details  The key is identified by its native row and column index.
     */
    bool keyIsPressed(uint8_t row, uint8_t col);

    //! @brief    Checks if the shift lock key is held down.
    bool shiftLockIsPressed() { return shiftLock; }

    //! @brief    Checks if the shift key is currently pressed.
    bool shiftKeyIsPressed() { return keyIsPressed(1,7); }
    
    //! @brief    Checks if the commodore key is currently pressed.
    bool commodoreKeyIsPressed() { return keyIsPressed(7,5); }
    
    //! @brief    Checks if the CTRL key is currently pressed.
    bool ctrlKeyIsPressed() { return keyIsPressed(7,2); }
    
    //! @brief    Checks if the runstop key is currently pressed.
    bool runstopKeyIsPressed() { return keyIsPressed(7,7); }
    
    
	/*! @brief    Presses a key.
     *  @details  The key is identified by its native row and column index.
     */
	void pressKey(uint8_t row, uint8_t col);
    
    /*! @brief    Presses the shift lock key
     *  @details  The shift lock key permanently holds down the right shift key
     */
    void pressShiftLockKey() { shiftLock = true; pressKey(6,4); }
    
    //! @brief    Releases the shift lock key
    void releaseShiftLockKey() { shiftLock = false; releaseKey(6,4); }

	//! @brief    Presses the shift hey.
	void pressShiftKey() { pressKey(1,7); }
    
	//! @brief    Presses the commodore key.
	void pressCommodoreKey() { pressKey(7,5); }
    
    //! @brief    Presses the CTRL key.
    void pressCtrlKey() { pressKey(7,2); }
    
	//! @brief    Presses the runstop key.
	void pressRunstopKey() { pressKey(7,7); }

    //! @brief    Presses shift and runstop simultaniously.
    // void pressShiftRunstopKey() { pressShiftKey(); pressKey(7,7); }

    //! @brief    Presses the restore key.
    void pressRestoreKey();
    
    
	/*! @brief    Releases a pressed key.
     *  @details  The key is identified by its native row and column index.
     */
	void releaseKey(uint8_t row, uint8_t col);
    
    //! @brief    Releases the shift key.
	void releaseShiftKey() { releaseKey(1,7); }
    
    //! @brief    Releases the commodore key.
	void releaseCommodoreKey() { releaseKey(7,5); }
    
    //! @brief    Releases the CTRL key.
    void releaseCtrlKey() { releaseKey(7,2); }
    
    //! @brief    Releases the runstop key.
	void releaseRunstopKey() { releaseKey(7,7); }

    //! @brief    Releases shift and runstop simultaniously.
    // void releaseShiftRunstopKey() { releaseKey(7,7); releaseShiftKey(); }

    //! @brief    Releases the restore key.
    void releaseRestoreKey();
    
    //! @brief    Clears the keyboard matrix.
    void releaseAll() { for (unsigned i = 0; i < 8; i++) kbMatrixRow[i] = kbMatrixCol[i] = 0xFF; }
    
    /*! @brief    Toggles a certain key.
     *  @details  The key is identified by its native row and column index.
     */
    void toggleKey(uint8_t row, uint8_t col);
    
    //! @brief    Toggles the shift key.
    void toggleShiftKey() { toggleKey(1,7); }
    
    //! @brief    Toggles the commodore key.
    void toggleCommodoreKey() { toggleKey(7,5); }
    
    //! @brief    Toggles the control key.
    void toggleCtrlKey() { toggleKey(7,2); }
    
    //! @brief    Toggles the runstop key.
    void toggleRunstopKey() { toggleKey(7,7); }
    
    
	/*! @brief    Reads a row from keyboard matrix
	 *  @param    columnMask  Indicates the rows to read
     */
	uint8_t getRowValues(uint8_t columnMask);

    /*! @brief    Reads a column from keyboard matrix.
     *  @param    rowMask  Indicates the rows to read
     */
    uint8_t getColumnValues(uint8_t rowMask);
    
    //! @brief    Returns true if the C64 is currently in upper case mode.
    /*! @details  To shift between two modes, press SHIFT + COMMODORE.
     */
    bool inUpperCaseMode();
};
	
#endif
