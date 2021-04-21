// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"

#include <queue>

struct KeyAction {
        
    // Action type
    enum class Action { press, release, releaseAll };
    Action type;

    // Key identifier (0 .. 65)
    u8 nr;
    
    // Keyboard matrix location
    u8 row, col;

    // Delay until the next action is performed, measures in frames
    i64 delay;

    // Constructors
    KeyAction(Action _type, u8 _nr, u64 _delay);
    KeyAction(Action _type, u8 _row, u8 _col, u64 _delay);
};

class Keyboard : public C64Component {
    
    friend struct KeyAction;
    
    // Maping from key numbers to keyboard matrix positions
    static constexpr u8 rowcol[66][2] =
    {
        // First physical row
        {7, 1}, {7, 0}, {7, 3}, {1, 0}, {1, 3}, {2, 0}, {2, 3}, {3, 0},
        {3, 3}, {4, 0}, {4, 3}, {5, 0}, {5, 3}, {6, 0}, {6, 3}, {0, 0},
        {0, 4},
        
        // Second physical row
        {7, 2}, {7, 6}, {1, 1}, {1, 6}, {2, 1}, {2, 6}, {3, 1}, {3, 6},
        {4, 1}, {4, 6}, {5, 1}, {5, 6}, {6, 1}, {6, 6}, {9, 9}, {0, 5},
        
        // Third physical row
        {7, 7}, {9, 9}, {1, 2}, {1, 5}, {2, 2}, {2, 5}, {3, 2}, {3, 5},
        {4, 2}, {4, 5}, {5, 2}, {5, 5}, {6, 2}, {6, 5}, {0, 1}, {0, 6},
        
        // Fourth physical row
        {7, 5}, {1, 7}, {1, 4}, {2, 7}, {2, 4}, {3, 7}, {3, 4}, {4, 7},
        {4, 4}, {5, 7}, {5, 4}, {6, 7}, {6, 4}, {0, 7}, {0, 2}, {0, 3},
        
        // Fifth physical row
        {7, 4}
    };
    
	// The C64 keyboard matrix indexed by row
    u8 kbMatrixRow[8] = { };

    // The C64 keyboard matrix indexed by column
    u8 kbMatrixCol[8] = { };

    // Indicates if the shift lock is currently pressed
    bool shiftLock = false;
        
    // Key action list (for auto typing)
    std::queue<KeyAction> actions;
    
    // Delay counter until the next key action is processed
    i64 delay = INT64_MAX;
    
    
    //
    // Initializing
    //
    
public:
    
    Keyboard(C64 &ref) : C64Component(ref) { }
    const char *getDescription() const override { return "Keyboard"; }

private:
    
	void _reset() override;
    
    
    //
    // Analyzing
    //
    
private:
    
    void _dump(dump::Category category, std::ostream& os) const override;
    
    
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
        
        << kbMatrixRow
        << kbMatrixCol
        << shiftLock;
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    

    //
    // Accessing
    //
    
public:
    
    // Checks whether a certain key is being pressed
    bool isPressed(long nr) const;
    bool isPressed(u8 row, u8 col) const;
    bool commodoreIsPressed() const { return isPressed(7,5); }
    bool ctrlIsPressed() const { return isPressed(7,2); }
    bool runstopIsPressed() const { return isPressed(7,7); }
    bool leftShiftIsPressed() const { return isPressed(1,7); }
    bool rightShiftIsPressed() const { return isPressed(6,4); }
    bool shiftLockIsPressed() const { return shiftLock; }
    bool restoreIsPressed() const;
    
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
    void scheduleKeyReleaseAll(i64 delay);

private:
    
    // Deletes all pending actions and clears the keyboard matrix
    void abortAutoTyping();
    
    // Workhorses for scheduleKeyPress and scheduleKeyRelease
    void _scheduleKeyAction(KeyAction::Action type, long nr, i64 delay);
    void _scheduleKeyAction(KeyAction::Action type, u8 row, u8 col, i64 delay);

    
    //
    // Performing periodic events
    //
    
public:
    
    void vsyncHandler();
};
	
