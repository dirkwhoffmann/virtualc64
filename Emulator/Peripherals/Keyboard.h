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

/* This class manages the keyboard matrix of the virtual C64. Keyboard
 * management works as follows: When the GUI recognizes a key press or a key
 * release, it calls one of the functions in this class to tell the virtual
 * keyboard about the event. The called functions do nothing more than clearing
 * or setting a bit in the 8x8 keyboard matrix.Each key corresponds to a
 * specific bit in the matrix and is uniquely determined by a row and a column
 * value.
 *
 * Communication with the virtual computer is managed solely by the CIA. When a
 * method getRowValues is called which finally gets the contents of the keyboard
 * matrix into the C64.
 */
class Keyboard : public C64Component {
    
    // Maping from key numbers to keyboard matrix positions
    static const u8 rowcol[66][2];
    
	// The C64 keyboard matrix indexed by row
	u8 kbMatrixRow[8];

    // The C64 keyboard matrix indexed by column
    u8 kbMatrixCol[8];

    // Indicates if the shift lock is currently pressed
    bool shiftLock;
    
    // Counter for clearing the keyboard matrix with delay
    i64 clearCnt = 0;
    
    
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
        worker
        
        & kbMatrixRow
        & kbMatrixCol
        & shiftLock
        & clearCnt;
    }
    
    size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    

    //
    // Accessing
    //
    
public:
    
    // Checks whether a certain key is being pressed
    bool keyIsPressed(long nr);
    bool keyIsPressed(u8 row, u8 col);
    bool commodoreIsPressed() { return keyIsPressed(7,5); }
    bool ctrlIsPressed() { return keyIsPressed(7,2); }
    bool runstopIsPressed() { return keyIsPressed(7,7); }
    bool leftShiftIsPressed() { return keyIsPressed(1,7); }
    bool rightShiftIsPressed() { return keyIsPressed(6,4); }
    bool shiftLockIsPressed() { return shiftLock; }
    bool restoreIsPressed();
    
	// Presses a key
    void pressKey(long nr, i64 period = 0);
	void pressKey(u8 row, u8 col, i64 period = 0);
    void pressCommodoreKey(i64 period = 0) { pressKey(7,5, period); }
    void pressCtrlKey(i64 period = 0) { pressKey(7,2, period); }
	void pressRunstopKey(i64 period = 0) { pressKey(7,7, period); }
    void pressLeftShiftKey(i64 period = 0) { pressKey(1,7, period); }
    void pressRightShiftKey(i64 period = 0) { pressKey(6,4, period); }
    void pressShiftLockKey() { shiftLock = true; }
    void pressRestoreKey(i64 period = 0);

	// Releases a pressed key
    void releaseKey(long nr);
	void releaseKey(u8 row, u8 col);
	void releaseCommodoreKey() { releaseKey(7,5); }
    void releaseCtrlKey() { releaseKey(7,2); }
	void releaseRunstopKey() { releaseKey(7,7); }
    void releaseLeftShiftKey() { releaseKey(1,7); }
    void releaseRightShiftKey() { releaseKey(6,4); }
    void releaseShiftLock() { shiftLock = false; }
    void releaseRestoreKey();
    
    // Clears the keyboard matrix
    void releaseAll();
    
    // Presses a released key and vice versa
    void toggleKey(long nr);
    void toggleKey(u8 row, u8 col);
    void toggleLeftShiftKey() { toggleKey(1,7); }
    void toggleRightShiftKey() { toggleKey(6,4); }
    void toggleShiftLock() { shiftLock = !shiftLock; }
    void toggleCommodoreKey() { toggleKey(7,5); }
    void toggleCtrlKey() { toggleKey(7,2); }
    void toggleRunstopKey() { toggleKey(7,7); }
    

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
    
    
    //
    // Performing periodic events
    //
    
public:
    
    void vsyncHandler();
};
	
#endif
