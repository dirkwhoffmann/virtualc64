// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Thread.h"

/* This class extends the Thread class by a suspend-resume mechanism for
 * pausing the thread temporarily. This functionality is utilized frequently
 * by the GUI to perform atomic state-change operations that require a paused
 * emulator. Such code sections can be embedded in a suspend / resume block
 * like this:
 *
 *        suspend();
 *        do something with the internal state;
 *        resume();
 *
 * It it safe to nest multiple suspend() / resume() blocks. It is essential
 * that each call to suspend() must be followed by a call to resume() which
 * means that the function must not exit in the middle of the block. An
 * exit-safe version is provided by the "suspended" macro which is the
 * recommended way to safely pause the emulator:
 *
 *        suspended {
 *            do something with the internal state;
 *        }
 */

class SuspendableThread : public Thread {
    
private:
    
    isize suspendCounter = 0;

public:

    void suspend() override;
    void resume() override;
};

class AutoResume {

    C64Component *comp;
    
public:

    bool active = true;

    AutoResume(C64Component *c) : comp(c) { comp->suspend(); }
    ~AutoResume() { comp->resume(); }
};

#define suspended \
for (AutoResume _ar(this); _ar.active; _ar.active = false)
