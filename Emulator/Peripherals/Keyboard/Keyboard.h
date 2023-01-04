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
#include "C64Types.h"
#include "C64Key.h"
#include <queue>

namespace vc64 {

struct KeyAction {

    // Action type
    enum class Action { wait, press, release, releaseAll };
    Action type;

    // The key the action is performed on
    std::vector<C64Key> keys;
    
    // Number of idle cycles (wait action)
    Cycle delay;

    // Constructors
    KeyAction(Action a, std::vector<C64Key> k, Cycle d) : type(a), keys(k), delay(d) { };
    KeyAction(Action a, std::vector<C64Key> k) : type(a), keys(k), delay(0) { };
    KeyAction(Action a, C64Key k) : KeyAction(a, std::vector<C64Key> { k }) { };
    KeyAction(Action a, Cycle d) : type(a), delay(d) { };
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

    
    //
    // Initializing
    //
    
public:
    
    Keyboard(C64 &ref) : SubComponent(ref) { }
    
    
    //
    // Methods from CoreObject
    //
    
private:
    
    const char *getDescription() const override { return "Keyboard"; }
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from CoreComponent
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


    //
    // Auto typing
    //
    
public:
    
    void autoType(const string &text);

    void scheduleKeyPress(std::vector<C64Key> keys, double delay);
    void scheduleKeyPress(C64Key key, double delay) { scheduleKeyPress(std::vector<C64Key>{key}, delay); }
    void scheduleKeyPress(char c, double delay) { scheduleKeyPress(C64Key::translate(c), delay); }

    void scheduleKeyRelease(std::vector<C64Key> keys, double delay);
    void scheduleKeyRelease(C64Key key, double delay) { scheduleKeyRelease(std::vector<C64Key>{key}, delay); }
    void scheduleKeyRelease(char c, double delay) { scheduleKeyRelease(C64Key::translate(c), delay); }

    void scheduleKeyReleaseAll(double delay);

    // Deletes all pending actions and clears the keyboard matrix
    void abortAutoTyping();


    //
    // Processing events
    //

public:

    // Processes the next auto-type event
    void processKeyEvent(EventID id);
};

}
