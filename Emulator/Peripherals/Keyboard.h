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
#include "C64Key.h"

#include <queue>

struct KeyAction {
        
    // Action type
    enum class Action { press, release, releaseAll };
    Action type;

    // The key the action is performed on
    C64Key key;
    
    // Delay until the next action is performed, measures in frames
    i64 delay;

    // Constructors
    KeyAction(Action a, C64Key k, u64 d) : type(a), key(k), delay(d) { };
};

class Keyboard : public C64Component {
        
	// The keyboard matrix (indexed by row or by column)
    u8 kbMatrixRow[8] = { };
    u8 kbMatrixCol[8] = { };

    // The number of pressed keys in a certain row or column
    u8 kbMatrixRowCnt[8] = { };
    u8 kbMatrixColCnt[8] = { };
    
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
        << kbMatrixRowCnt
        << kbMatrixColCnt
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
    bool isPressed(C64Key key) const;
    bool commodoreIsPressed() const { return isPressed(C64Key::commodore); }
    bool ctrlIsPressed() const { return isPressed(C64Key::control); }
    bool runstopIsPressed() const { return isPressed(C64Key::runStop); }
    bool leftShiftIsPressed() const { return isPressed(C64Key::leftShift); }
    bool rightShiftIsPressed() const { return isPressed(C64Key::rightShift); }
    bool shiftLockIsPressed() const { return shiftLock; }
    bool restoreIsPressed() const;
    
	// Presses a key
    void press(C64Key key);
    void pressCommodore() { press(C64Key::commodore); }
    void pressCtrl() { press(C64Key::control); }
	void pressRunstop() { press(C64Key::runStop); }
    void pressLeftShift() { press(C64Key::leftShift); }
    void pressRightShift() { press(C64Key::rightShift); }
    void pressShiftLock() { shiftLock = true; }
    void pressRestore();

	// Releases a pressed key
    void release(C64Key key);
	void releaseCommodore() { release(C64Key::commodore); }
    void releaseCtrl() { release(C64Key::control); }
	void releaseRunstop() { release(C64Key::runStop); }
    void releaseLeftShift() { release(C64Key::leftShift); }
    void releaseRightShift() { release(C64Key::rightShift); }
    void releaseShiftLock() { shiftLock = false; }
    void releaseRestore();
    
    // Clears the keyboard matrix
    void releaseAll();
    
    // Presses a released key and vice versa
    void toggle(C64Key key);
    void toggleCommodore() { toggle(C64Key::commodore); }
    void toggleCtrl() { toggle(C64Key::control); }
    void toggleRunstop() { toggle(C64Key::runStop); }
    void toggleLeftShift() { toggle(C64Key::leftShift); }
    void toggleRightShift() { toggle(C64Key::rightShift); }
    void toggleShiftLock() { shiftLock = !shiftLock; }
    
private:
    
    void _press(C64Key key);
    void _pressRestore();
    
    void _release(C64Key key);
    void _releaseRestore();

    void _releaseAll();
    
    
    //
    // Accessing the keyboard matrix
    //
    
public:
    
	// Reads a column or row from the keyboard matrix
    u8 getColumnValues(u8 rowMask);
    u8 getRowValues(u8 columnMask);
	u8 getRowValues(u8 columnMask, u8 thresholdMask);
    
    
    //
    // Auto typing
    //
    
public:
    
    void autoType(const string &text);
    
    void scheduleKeyPress(C64Key key, i64 delay);
    void scheduleKeyPress(char c, i64 delay);

    void scheduleKeyRelease(C64Key key, i64 delay);
    void scheduleKeyRelease(char c, i64 delay);
    
    void scheduleKeyReleaseAll(i64 delay);

private:
    
    // Deletes all pending actions and clears the keyboard matrix
    void abortAutoTyping();
    
    // Workhorses for scheduleKeyPress and scheduleKeyRelease
    void _scheduleKeyAction(KeyAction::Action type, C64Key key, i64 delay);

    
    //
    // Performing periodic events
    //
    
public:
    
    void vsyncHandler();
};
	
