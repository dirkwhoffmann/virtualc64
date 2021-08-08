// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64ComponentTypes.h"
#include "C64Object.h"
#include "Serialization.h"
#include "Concurrency.h"

#include <vector>
#include <iostream>
#include <iomanip>

/* This class defines the base functionality of all hardware components. It
 * comprises functions for initializing, configuring, and serializing, as well
 * as functions for powering up and down, running and pausing. Furthermore,
 * a 'synchronized' macro is provided to prevent mutual execution of certain
 * code sections.
 */

#define synchronized \
for (util::AutoMutex _am(mutex); _am.active; _am.active = false)

namespace dump {
enum Category : usize {
    
    Config    = 0b00000001,
    State     = 0b00000010,
    Registers = 0b00000100,
    Events    = 0b00001000,
    Checksums = 0b00010000,
    Dma       = 0b00100000,
    BankMap   = 0b01000000,
    Disk      = 0b10000000
};

}
    
class C64Component : public C64Object {
        
protected:
    
    // Sub components
    std::vector<C64Component *> subComponents;
            
    /* Indicates if the emulator should be executed in debug mode. Debug mode
     * is enabled when the GUI debugger is opend and disabled when the GUI
     * debugger is closed. In debug mode, several time-consuming tasks are
     * performed that are usually left out. E.g., the CPU checks for
     * breakpoints and records the executed instruction in it's trace buffer.
     */
    bool debugMode = false;
    
    /* Mutex for implementing the 'synchronized' macro. The macro can be used
     * to prevent multiple threads to enter the same code block. It mimics the
     * behaviour of the well known Java construct 'synchronized(this) { }'.
     */
    util::ReentrantMutex mutex;
    

    //
    // Initializing
    //
    
public:
    
    virtual ~C64Component();
    
    /* Initializes the component and it's subcomponents. The initialization
     * procedure is initiated once, in the constructor of the C64 class. By
     * default, a component enters it's initial configuration. Custom actions
     * can be performed by implementing the _initialize() delegation function.
     */
    void initialize();
    virtual void _initialize() { resetConfig(); }
    
    /* Resets the component and its subcomponent. It is mandatory for each
     * component to implement this function.
     */
    void reset(bool hard);
    virtual void _reset(bool hard) = 0;
    
    
    //
    // Configuring
    //
    
    // Initializes all configuration items with their default values
    virtual void resetConfig() { };

    /* Configures the component and it's subcomponents. This function
     * distributes a configuration request to all subcomponents by calling
     * setConfigItem().
     */
    void configure(Option option, i64 value) throws;
    void configure(Option option, long id, i64 value) throws;
    
    /* Requests the change of a single configuration item. Each sub-component
     * checks if it is responsible for the requested configuration item. If
     * yes, it changes the internal state. If no, it ignores the request.
     */
    virtual void setConfigItem(Option option, i64 value) throws { }
    virtual void setConfigItem(Option option, long id, i64 value) throws { }
            

    //
    // Analyzing
    //
    
    /* Collects information about the component and it's subcomponents. Many
     * components contain an info variable of a class specific type (e.g.,
     * CPUInfo, MemoryInfo, ...). These variables contain the information shown
     * in the GUI's inspector window and are updated by calling this function.
     * Note: Because this function accesses the internal emulator state with
     * many non-atomic operations, it must not be called on a running emulator.
     * To carry out inspections while the emulator is running, set up an
     * inspection target via C64::setInspectionTarget().
     */
    void inspect();
    virtual void _inspect() { }
    
    /* Base method for building the class specific getInfo() methods. When the
     * emulator is running, the result of the most recent inspection is
     * returned. If the emulator isn't running, the function first updates the
     * cached values in order to return up-to-date results.
     */
    template<class T> T getInfo(T &cachedValues) {
        
        if (!isRunning()) inspect();
        
        T result;
        synchronized { result = cachedValues; }
        return result;
    }
    
    /* Prints debug information about this component. The additional 'flags'
     * parameter is a bit field which can be used to limit the displayed
     * information to certain categories.
     */
    void dump(dump::Category category, std::ostream& ss) const;
    void dump(dump::Category category) const;
    void dump(std::ostream& ss) const;
    void dump() const;
    virtual void _dump(dump::Category category, std::ostream& ss) const { };
    
 
    //
    // Serializing
    //
    
    // Returns the size of the internal state in bytes
    isize size();
    virtual isize _size() = 0;
    
    // Loads the internal state from a memory buffer
    isize load(const u8 *buffer);
    virtual isize _load(const u8 *buffer) = 0;
    
    // Saves the internal state to a memory buffer
    isize save(u8 *buffer);
    virtual isize _save(u8 *buffer) = 0;
    
    /* Delegation methods called inside load() or save(). Some components
     * override these methods to add custom behavior if not all elements can be
     * processed by the default implementation.
     */
    virtual isize willLoadFromBuffer(const u8 *buffer) { return 0; }
    virtual isize didLoadFromBuffer(const u8 *buffer) { return 0; }
    virtual isize willSaveToBuffer(const u8 *buffer) {return 0; }
    virtual isize didSaveToBuffer(u8 *buffer) { return 0; }
    
    
    //
    // Controlling the state (see Thread class for details)
    //
    
public:
    
    virtual bool isPoweredOff() const = 0;
    virtual bool isPoweredOn() const = 0;
    virtual bool isPaused() const = 0;
    virtual bool isRunning() const = 0;
    
protected:
    
    void powerOn();
    void powerOff();
    void run();
    void pause();
    void warpOn();
    void warpOff();
    void debugOn();
    void debugOff();
    
    virtual void _powerOn() { }
    virtual void _powerOff() { }
    virtual void _run() { }
    virtual void _pause() { }
    virtual void _warpOn() { }
    virtual void _warpOff() { }
    virtual void _debugOn() { }
    virtual void _debugOff() { }
};

//
// Standard implementations of _reset, _load, and _save
//

#define COMPUTE_SNAPSHOT_SIZE \
util::SerCounter counter; \
applyToPersistentItems(counter); \
applyToResetItems(counter); \
return counter.count;
    
#define RESET_SNAPSHOT_ITEMS(hard) \
util::SerResetter resetter; \
applyToResetItems(resetter, hard);
    
#define LOAD_SNAPSHOT_ITEMS \
util::SerReader reader(buffer); \
applyToPersistentItems(reader); \
applyToResetItems(reader); \
return reader.ptr - buffer;
    
#define SAVE_SNAPSHOT_ITEMS \
util::SerWriter writer(buffer); \
applyToPersistentItems(writer); \
applyToResetItems(writer); \
return writer.ptr - buffer;
