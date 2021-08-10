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
 * pausing the thread temporarily. This is useful in all cases where an
 * operation cannot be performed on a running emulator thread. Such critical
 * code sections can be embedded in a suspend / resume block like so:
 *
 *            suspend();
 *            do something with the internal state;
 *            resume();
 *
 * It it safe to nest multiple suspend() / resume() blocks. It is essential
 * that each
 */
