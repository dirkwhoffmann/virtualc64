// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include <cstdio>

namespace vc64 {

/** Suspend-resume mechanism */
class Suspendable {

public:
    
    Suspendable() { }
    virtual ~Suspendable() = default;

    /** @brief  Suspends the thread */
    virtual void suspend() = 0;

    /** @brief  Resumes the thread */
    virtual void resume() = 0;
};

struct AutoResume {

    Suspendable *s;
    AutoResume(Suspendable *s) : s(s) { s->suspend(); }
    ~AutoResume() { s->resume(); }
};

#define SUSPENDED AutoResume _ar(this);

}
