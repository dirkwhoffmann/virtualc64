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
    
	// The C64 keyboard matrix indexed by row
	u8 kbMatrixRow[8];

    // The C64 keyboard matrix indexed by column
    u8 kbMatrixCol[8];

    // Indicates if the shift lock is currently pressed
    bool shiftLock;
    
    
    //
    // Initializing
    //
    
public:
    
    Keyboard(C64 &ref);
    
private:
    
	void _reset() override;
    
    
    //
    // Analyzing
    //
    
private:
    
	void _dump() override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
    }
    
    size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    size_t _save(u8 *buffer) override { assert(false); SAVE_SNAPSHOT_ITEMS }
    

    //
    // Accessing
    //
    
public:
    
    /* Checks if a certain key is currently pressed. The key is identified by
     * its native row and column index.
     */
    bool keyIsPressed(u8 row, u8 col);

    // Checks if the shift lock key is held down
    bool shiftLockIsHoldDown() { return shiftLock; }

    // Checks if the left shift key is currently pressed
    bool leftShiftIsPressed() { return keyIsPressed(1,7); }
    
    // Checks if the right shift key is currently pressed
    bool rightShiftIsPressed() { return keyIsPressed(6,4); }
    
    // Checks if the commodore key is currently pressed
    bool commodoreIsPressed() { return keyIsPressed(7,5); }
    
    // Checks if the CTRL key is currently pressed
    bool ctrlIsPressed() { return keyIsPressed(7,2); }
    
    // Checks if the runstop key is currently pressed
    bool runstopIsPressed() { return keyIsPressed(7,7); }
    
	// Presses a key
	void pressKey(u8 row, u8 col);
	void pressCommodoreKey() { pressKey(7,5); }
    void pressCtrlKey() { pressKey(7,2); }
	void pressRunstopKey() { pressKey(7,7); }
    void pressRestoreKey();
    
	// Releases a pressed key
	void releaseKey(u8 row, u8 col);
	void releaseCommodoreKey() { releaseKey(7,5); }
    void releaseCtrlKey() { releaseKey(7,2); }
	void releaseRunstopKey() { releaseKey(7,7); }
    void releaseRestoreKey();
    
    // Clears the keyboard matrix
    void releaseAll();
    
    // Presses a released key and vice versa
    void toggleKey(u8 row, u8 col);
    void toggleCommodoreKey() { toggleKey(7,5); }
    void toggleCtrlKey() { toggleKey(7,2); }
    void toggleRunstopKey() { toggleKey(7,7); }
    
    
    //
    // Handling the shift lock key
    //
    
    /* Setter for shiftLock. As a side effect, a KEYMATRIX message is sent if
     * if the variable changes.
     */
    void setShiftLock(bool value);
    
    /* Presses or releases the shift lock key. Pressing this key has the same
     * effect as holding the right shift key permanently.
     */
    void pressShiftLockKey() { setShiftLock(true); }
    void releaseShiftLockKey() { setShiftLock(false); }
    
    
    //
    // Accessing the keyboard matrix
    //
    
	// Reads a row or a column from the keyboard matrix
	u8 getRowValues(u8 columnMask);
    u8 getColumnValues(u8 rowMask);
    
    
    /* Returns true if the C64 is currently in upper case mode. When the C64
     * is in normal operation, pressing SHIFT + COMMODORE toggles between the
     * two modes.
     */
    bool inUpperCaseMode();
};
	
#endif
