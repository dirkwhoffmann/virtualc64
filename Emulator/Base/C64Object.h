// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Error.h"

namespace vc64 {

/* Object model:
 *
 * ------------------
 * |   C64Object    |
 * ------------------
 *         |
 * ------------------
 * |  C64Component  |
 * ------------------
 *         |
 *         |   ------------------   ----------------
 *         |-->|     Thread     |-->|     C64      |
 *         |   ------------------   ----------------
 *         |   ------------------
 *         |-->|  SubComponent  |
 *             ------------------
 *
 * C64Object is the base class for all C64 related classes. It provides a
 * a textual description for the object as well as various functions for
 * printing debug information.
 *
 * C64Component defines the base functionality of all hardware components. It
 * comprises functions for initializing, configuring, and serializing the
 * object, as well as functions for powering up and down, running and
 * pausing. Furthermore, a 'SYNCHRONIZED' macro is provided to prevent mutual
 * execution of certain code components.
 *
 * Thread adds the ability to run the component asynchroneously. It implements
 * the emulator's state model (off, paused, running).
 */

enum class Category
{
    BankMap, Checksum, Config, Defaults, Disk, Dma, Events, Layout, Registers, State
};

class C64Object {

protected:

    static bool verbose;

    //
    // Initializing
    //
    
public:
    
    virtual ~C64Object() { };
    
    // Returns the name for this component (e.g., "CPU" or "VICII")
    virtual const char *getDescription() const = 0;
    
    // Called by debug() and trace() to produce a detailed debug output
    virtual void prefix() const;
    
    // Prints debug information about this component
    void dump(Category category, std::ostream& ss) const;
    void dump(Category category) const;
    virtual void _dump(Category category, std::ostream& ss) const { };
};

/* This file provides several macros for printing messages:
 *
 *   - msg    Information message   (Shows up in all builds)
 *   - warn   Warning message       (Shows up in all builds)
 *   - fatal  Error message + Exit  (Shows up in all builds)
 *   - debug  Debug message         (Shows up in debug builds, only)
 *   - plain  Plain debug message   (Shows up in debug builds, only)
 *   - trace  Detailed debug output (Shows up in debug builds, only)
 *
 * Debug messages are prefixed by the component name and a line number. Trace
 * messages are prefixed by a more detailed string description produced by the
 * prefix() function.
 *
 * Debug, plain, and trace messages are accompanied by an optional 'enable'
 * parameter. If 0 is passed in, no output will be generated. In addition,
 * variable 'verbose' is checked which is set to true by default. By setting
 * this variable to false, debug output can be silenced temporarily.
 *
 * Sidenote: In previous releases the printing macros were implemented in form
 * of variadic functions. Although this might seem to be superior at first
 * glance, it is not. Using macros allows modern compilers to verify the format
 * strings against the data types of the provided arguments. This check can't
 * be performed when variadic functions are used.
 */

#define msg(format, ...) \
fprintf(stderr, format, ##__VA_ARGS__);

#define warn(format, ...) \
fprintf(stderr, "Warning: " format, ##__VA_ARGS__);

#define fatal(format, ...) \
{ fprintf(stderr, "Fatal: " format, ##__VA_ARGS__); exit(1); }

#ifndef NDEBUG

#define debug(enable, format, ...) \
if constexpr (enable) { if (verbose) { \
fprintf(stderr, "%s:%d " format, getDescription(), __LINE__, ##__VA_ARGS__); }}

#define plain(enable, format, ...) \
if constexpr (enable) { if (verbose) { \
fprintf(stderr, format, ##__VA_ARGS__); }}

#define trace(enable, format, ...) \
if constexpr (enable) { if (verbose) { \
prefix(); \
fprintf(stderr, "%s:%d " format, getDescription(), __LINE__, ##__VA_ARGS__); }}

#else

#define debug(enable, format, ...)
#define plain(enable, format, ...)
#define trace(enable, format, ...)

#endif

}
