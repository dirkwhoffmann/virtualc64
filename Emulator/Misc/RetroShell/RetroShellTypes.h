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
/// @file

#pragma once

#include "Aliases.h"
#include "Reflection.hpp"

//
// Enumerations
//

/// RetroShell special key
enum_long(RSKEY_KEY)
{
	RSKEY_UP,
	RSKEY_DOWN,
	RSKEY_LEFT,
	RSKEY_RIGHT,
	RSKEY_DEL,
    RSKEY_CUT,
	RSKEY_BACKSPACE,
	RSKEY_HOME,
	RSKEY_END,
	RSKEY_TAB,
	RSKEY_RETURN,
    RSKEY_SHIFT_RETURN,
	RSKEY_CR
};
typedef RSKEY_KEY RetroShellKey;
