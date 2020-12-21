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

struct KeyAction {
    
    // Keyboard matrix location
    u8 row, col;

    // Action type (true = press, false = release)
    bool press;

    // Delay until the next action is performed, measures in frames
    u64 delay;

    // Initializes a key action
    KeyAction(u8 _row, u8 _col, bool _press, u64 _delay = 0) :
    row(_row), col(_col), press(_press), delay(_delay) { }

    // Performs a key action (presses or releases the recorded key)
    void perform(Keyboard &kb);
};

class Keyboard : public C64Component {
    
    friend struct KeyAction;
    
    // Maping from key numbers to keyboard matrix positions
    static const u8 rowcol[66][2];
    
	// The C64 keyboard matrix indexed by row
	u8 kbMatrixRow[8];

    // The C64 keyboard matrix indexed by column
    u8 kbMatrixCol[8];

    // Indicates if the shift lock is currently pressed
    bool shiftLock;
    
    // Counter for clearing the keyboard matrix with delay
    i64 clearCnt = 0; // DEPRECATED
    
    // Key action list (for auto typing)
    std::queue<KeyAction> actions;
    
    // Delay counter until the next key action is processed
    i64 delay = 0;
    
    
    //
    // Initializing
    //
    
public:
    
    Keyboard(C64 &ref) : C64Component(ref) { }
    const char *getDescription() override { return "Keyboard"; }

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
    bool isPressed(long nr);
    bool isPressed(u8 row, u8 col);
    bool commodoreIsPressed() { return isPressed(7,5); }
    bool ctrlIsPressed() { return isPressed(7,2); }
    bool runstopIsPressed() { return isPressed(7,7); }
    bool leftShiftIsPressed() { return isPressed(1,7); }
    bool rightShiftIsPressed() { return isPressed(6,4); }
    bool shiftLockIsPressed() { return shiftLock; }
    bool restoreIsPressed();
    
	// Presses a key
    void press(long nr, i64 duration = 0);
	void pressRowCol(u8 row, u8 col, i64 duration = 0);
    void pressCommodore(i64 duration = 0) { pressRowCol(7,5, duration); }
    void pressCtrl(i64 duration = 0) { pressRowCol(7,2, duration); }
	void pressRunstop(i64 duration = 0) { pressRowCol(7,7, duration); }
    void pressLeftShift(i64 duration = 0) { pressRowCol(1,7, duration); }
    void pressRightShift(i64 duration = 0) { pressRowCol(6,4, duration); }
    void pressShiftLock() { shiftLock = true; }
    void pressRestore(i64 duration = 0);

	// Releases a pressed key
    void release(long nr);
	void releaseRowCol(u8 row, u8 col);
	void releaseCommodore() { releaseRowCol(7,5); }
    void releaseCtrl() { releaseRowCol(7,2); }
	void releaseRunstop() { releaseRowCol(7,7); }
    void releaseLeftShift() { releaseRowCol(1,7); }
    void releaseRightShift() { releaseRowCol(6,4); }
    void releaseShiftLock() { shiftLock = false; }
    void releaseRestore();
    
    // Clears the keyboard matrix
    void releaseAll();
    
    // Presses a released key and vice versa
    void toggle(long nr);
    void toggle(u8 row, u8 col);
    void toggleLeftShift() { toggle(1,7); }
    void toggleRightShift() { toggle(6,4); }
    void toggleShiftLock() { shiftLock = !shiftLock; }
    void toggleCommodore() { toggle(7,5); }
    void toggleCtrl() { toggle(7,2); }
    void toggleRunstop() { toggle(7,7); }
    

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
    // DEPRECATED
    bool inUpperCaseMode();
    
    
    //
    // Auto typing
    //
    
    // void addKeyAction(KeyAction action);
    void addKeyPress(u8 row, u8 col, i64 delay = 0);
    void addKeyRelease(u8 row, u8 col, i64 delay = 0);

    // Sets the delay counter
    void setInitialDelay(i64 initialDelay);
    
    
    //
    // Performing periodic events
    //
    
public:
    
    void vsyncHandler();
};
	
#endif
