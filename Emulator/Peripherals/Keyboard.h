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
    
    // Action type (true = press, false = release)
    bool press;

    // Key identifier (0 .. 65)
    u8 nr;
    
    // Keyboard matrix location
    u8 row, col;

    // Delay until the next action is performed, measures in frames
    i64 delay;

    // Constructors
    KeyAction(bool _press, u8 _nr, u64 _delay);
    KeyAction(bool _press, u8 _row, u8 _col, u64 _delay);
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
        & shiftLock;
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
    void press(long nr);
	void pressRowCol(u8 row, u8 col);
    void pressCommodore() { pressRowCol(7,5); }
    void pressCtrl() { pressRowCol(7,2); }
	void pressRunstop() { pressRowCol(7,7); }
    void pressLeftShift() { pressRowCol(1,7); }
    void pressRightShift() { pressRowCol(6,4); }
    void pressShiftLock() { shiftLock = true; }
    void pressRestore();

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
    
private:
    
    void _press(long nr);
    void _pressRowCol(u8 row, u8 col);
    void _pressRestore();
    
    void _release(long nr);
    void _releaseRowCol(u8 row, u8 col);
    void _releaseRestore();

    void _releaseAll();
    
    
    //
    // Accessing the keyboard matrix
    //
    
public:
    
	// Reads a row or a column from the keyboard matrix
	u8 getRowValues(u8 columnMask);
    u8 getColumnValues(u8 rowMask);
    
    
    //
    // Auto typing
    //
    
public:
    
    void scheduleKeyPress(long nr, i64 delay);
    void scheduleKeyPress(u8 row, u8 col, i64 delay);
    void scheduleKeyRelease(long nr, i64 delay);
    void scheduleKeyRelease(u8 row, u8 col, i64 delay);

    // Inserts a delay after the last pending action
    void addDelay(i64 delay);

    // Deletes all pending actions and clears the keyboard matrix
    void abortAutoTyping();
    
private:
    
    void _scheduleKeyAction(bool press, long nr, i64 delay);
    void _scheduleKeyAction(bool press, u8 row, u8 col, i64 delay);

    
    //
    // Performing periodic events
    //
    
public:
    
    void vsyncHandler();
};
	
#endif
