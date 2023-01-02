// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "SubComponent.h"
#include "C64Key.h"
#include <queue>

namespace vc64 {

struct KeyAction {

    // Action type
    enum class Action { press, release, releaseAll };
    Action type;

    // The key the action is performed on
    std::vector<C64Key> keys;
    
    // Delay until the next action is performed, measures in frames
    i64 delay;

    // Constructors
    KeyAction(Action a, std::vector<C64Key> k, u64 d) : type(a), keys(k), delay(d) { };
    KeyAction(Action a, C64Key k, u64 d) : KeyAction(a, std::vector<C64Key> { k }, d) { };
};

class Keyboard : public SubComponent {

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
    
    Keyboard(C64 &ref) : SubComponent(ref) { }
    
    
    //
    // Methods from C64Object
    //
    
private:
    
    const char *getDescription() const override { return "Keyboard"; }
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from C64Component
    //
    
private:
    
    void _reset(bool hard) override;

    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        worker
        
        << kbMatrixRow
        << kbMatrixCol
        << kbMatrixRowCnt
        << kbMatrixColCnt
        << shiftLock;
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    u64 _checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    

    //
    // Accessing the keyboard matrix
    //
    
public:

    // Reads a column or row from the keyboard matrix
    u8 getColumnValues(u8 rowMask) const;
    u8 getRowValues(u8 columnMask) const;
    u8 getRowValues(u8 columnMask, u8 thresholdMask) const;

    // Checks whether a certain key is pressed
    bool isPressed(C64Key key) const;
    bool shiftLockIsPressed() const;
    bool restoreIsPressed() const;
    bool commodoreIsPressed() const { return isPressed(C64Key::commodore); }
    bool ctrlIsPressed() const { return isPressed(C64Key::control); }
    bool runstopIsPressed() const { return isPressed(C64Key::runStop); }
    bool leftShiftIsPressed() const { return isPressed(C64Key::leftShift); }
    bool rightShiftIsPressed() const { return isPressed(C64Key::rightShift); }

    // Presses a key
    void press(C64Key key);
    void pressShiftLock();
    void pressRestore();
    void pressCommodore() { press(C64Key::commodore); }
    void pressCtrl() { press(C64Key::control); }
    void pressRunstop() { press(C64Key::runStop); }
    void pressLeftShift() { press(C64Key::leftShift); }
    void pressRightShift() { press(C64Key::rightShift); }

    // Releases a pressed key
    void release(C64Key key);
    void releaseShiftLock();
    void releaseRestore();
    void releaseCommodore() { release(C64Key::commodore); }
    void releaseCtrl() { release(C64Key::control); }
    void releaseRunstop() { release(C64Key::runStop); }
    void releaseLeftShift() { release(C64Key::leftShift); }
    void releaseRightShift() { release(C64Key::rightShift); }

    // Presses a released key and vice versa
    void toggle(C64Key key) { isPressed(key) ? release(key) : press(key); }
    void toggleShiftLock() { shiftLockIsPressed() ? releaseShiftLock() : pressShiftLock(); }
    void toggleRestore() { restoreIsPressed() ? releaseRestore() : pressRestore(); }
    void toggleCommodore() { toggle(C64Key::commodore); }
    void toggleCtrl() { toggle(C64Key::control); }
    void toggleRunstop() { toggle(C64Key::runStop); }
    void toggleLeftShift() { toggle(C64Key::leftShift); }
    void toggleRightShift() { toggle(C64Key::rightShift); }

    // Clears the keyboard matrix
    void releaseAll();

private:

    bool _isPressed(C64Key key) const;
    bool _shiftLockIsPressed() const;
    bool _restoreIsPressed() const;

    void _press(C64Key key);
    void _pressShiftLock();
    void _pressRestore();
    
    void _release(C64Key key);
    void _releaseShiftLock();
    void _releaseRestore();

    void _releaseAll();
    

    //
    // Auto typing
    //
    
public:
    
    void autoType(const string &text);

    void scheduleKeyPress(std::vector<C64Key> keys, i64 delay);
    void scheduleKeyPress(C64Key key, i64 delay) { scheduleKeyPress(std::vector<C64Key>{key}, delay); }
    void scheduleKeyPress(char c, i64 delay) { scheduleKeyPress(C64Key::translate(c), delay); }

    void scheduleKeyRelease(std::vector<C64Key> keys, i64 delay);
    void scheduleKeyRelease(C64Key key, i64 delay) { scheduleKeyRelease(std::vector<C64Key>{key}, delay); }
    void scheduleKeyRelease(char c, i64 delay) { scheduleKeyRelease(C64Key::translate(c), delay); }

    void scheduleKeyReleaseAll(i64 delay);

private:
    
    // Deletes all pending actions and clears the keyboard matrix
    void abortAutoTyping();
    
    // Workhorses for scheduleKeyPress and scheduleKeyRelease
    void _scheduleKeyAction(KeyAction::Action type, std::vector<C64Key> keys, i64 delay);

    
    //
    // Performing periodic events
    //
    
public:
    
    void vsyncHandler();
};

}
