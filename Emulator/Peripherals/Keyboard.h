// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "C64Component.h"

/* The virtual keyboard of a C64
 * This class manages the keyboard matrix of the virtual C64. Keyboard
 * management works as follows: When the GUI recognizes a key press or a key
 * release, it calls one of the functions in this class to tell the virtual
 * keyboard about the event. The called functions do nothing more than clearing
 * or setting a bit in the 8x8 keyboard matrix.Each key corresponds to a
 * specific bit in the matrix and is uniquely determined by a row and a column
 * value.
 *
 * Communication with the virtual computer is managed solely by the CIA. When a
 * method getRowValues is called which finally gets the contents of the keyboard
 * matrix into the virtual C64.
 */
class Keyboard : public C64Component {

private:
    
	//! @brief    The C64 keyboard matrix indexed by row
	u8 kbMatrixRow[8];

    //! @brief    The C64 keyboard matrix indexed by column
    u8 kbMatrixCol[8];

    //! @brief    True iff shift lock is pressed
    bool shiftLock;
    
    
    //
    // Constructing and serializing
    //
    
public:
    
    Keyboard(C64 &ref);
    
    
    //
    // Methods from HardwareComponent
    //
    
public:
    
	void _reset() override;
    
private:
    
	void _dump() override;

    
public:
    
    /*! @brief    Checks if a certain key is currently pressed.
     *  @details  The key is identified by its native row and column index.
     */
    bool keyIsPressed(u8 row, u8 col);

    //! @brief    Checks if the shift lock key is held down.
    bool shiftLockIsHoldDown() { return shiftLock; }

    //! @brief    Checks if the left shift key is currently pressed.
    bool leftShiftIsPressed() { return keyIsPressed(1,7); }
    
    //! @brief    Checks if the right shift key is currently pressed.
    bool rightShiftIsPressed() { return keyIsPressed(6,4); }
    
    //! @brief    Checks if the commodore key is currently pressed.
    bool commodoreIsPressed() { return keyIsPressed(7,5); }
    
    //! @brief    Checks if the CTRL key is currently pressed.
    bool ctrlIsPressed() { return keyIsPressed(7,2); }
    
    //! @brief    Checks if the runstop key is currently pressed.
    bool runstopIsPressed() { return keyIsPressed(7,7); }
    
    
	/*! @brief    Presses a key.
     *  @details  The key is identified by its native row and column index.
     */
	void pressKey(u8 row, u8 col);
    
	//! @brief    Presses the left shift hey.
	// void pressLeftShiftKey() { pressKey(1,7); }
    
	//! @brief    Presses the commodore key.
	void pressCommodoreKey() { pressKey(7,5); }
    
    //! @brief    Presses the CTRL key.
    void pressCtrlKey() { pressKey(7,2); }
    
	//! @brief    Presses the runstop key.
	void pressRunstopKey() { pressKey(7,7); }

    //! @brief    Presses the restore key.
    void pressRestoreKey();
    
    
	/*! @brief    Releases a pressed key.
     *  @details  The key is identified by its native row and column index.
     */
	void releaseKey(u8 row, u8 col);
    
    //! @brief    Releases the left shift key.
	// void releaseShiftKey() { releaseKey(1,7); }
    
    //! @brief    Releases the commodore key.
	void releaseCommodoreKey() { releaseKey(7,5); }
    
    //! @brief    Releases the CTRL key.
    void releaseCtrlKey() { releaseKey(7,2); }
    
    //! @brief    Releases the runstop key.
	void releaseRunstopKey() { releaseKey(7,7); }

    //! @brief    Releases the restore key.
    void releaseRestoreKey();
    
    //! @brief    Clears the keyboard matrix.
    void releaseAll() { for (unsigned i = 0; i < 8; i++) kbMatrixRow[i] = kbMatrixCol[i] = 0xFF; }
    
    /*! @brief    Toggles a certain key.
     *  @details  The key is identified by its native row and column index.
     */
    void toggleKey(u8 row, u8 col);
    
    //! @brief    Toggles the shift key.
    void toggleShiftKey() { toggleKey(1,7); }
    
    //! @brief    Toggles the commodore key.
    void toggleCommodoreKey() { toggleKey(7,5); }
    
    //! @brief    Toggles the control key.
    void toggleCtrlKey() { toggleKey(7,2); }
    
    //! @brief    Toggles the runstop key.
    void toggleRunstopKey() { toggleKey(7,7); }
    
    
    //
    //! @functiongroup Handling the shift lock key
    //
    
    //! @brief    Setter for shiftLock
    /*! @details  Sends a KEYMATRIX message if the variable changes
     */
    void setShiftLock(bool value);
    
    /*! @brief    Presses the shift lock key
     *  @details  Pressing shift lock has the same effect as holding the
     *            right shift key permanently.
     */
    void pressShiftLockKey() { setShiftLock(true); }
    
    //! @brief    Releases the shift lock key
    void releaseShiftLockKey() { setShiftLock(false); }
    
    
    //
    //! @functiongroup Accessing the keyboard matrix
    //
    
	/*! @brief    Reads a row from keyboard matrix
	 *  @param    columnMask  Indicates the rows to read
     */
	u8 getRowValues(u8 columnMask);

    /*! @brief    Reads a column from keyboard matrix.
     *  @param    rowMask  Indicates the rows to read
     */
    u8 getColumnValues(u8 rowMask);
    
    
    //! @brief    Returns true if the C64 is currently in upper case mode.
    /*! @details  To shift between two modes, press SHIFT + COMMODORE.
     */
    bool inUpperCaseMode();
};
	
#endif
