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

#include "vcdebug.h"

//
// Release settings
//

// Version number
static constexpr int VER_MAJOR      = 6;
static constexpr int VER_MINOR      = 0;
static constexpr int VER_SUBMINOR   = 0;
static constexpr int VER_BETA       = 0;

// Snapshot version number
static constexpr int SNP_MAJOR      = 6;
static constexpr int SNP_MINOR      = 0;
static constexpr int SNP_SUBMINOR   = 0;
static constexpr int SNP_BETA       = 0;


//
// Configuration overrides
//

#define OVERRIDES { }


//
// Debug settings
//

static constexpr bool betaRelease = VER_BETA != 0;

#ifdef NDEBUG
static constexpr bool releaseBuild = 1;
static constexpr bool debugBuild = 0;
typedef const int debugflag;
#else
static constexpr bool releaseBuild = 0;
static constexpr bool debugBuild = 1;
typedef int debugflag;
#endif

#ifdef __EMSCRIPTEN__
static constexpr bool emscripten = 1;
#else
static constexpr bool emscripten = 0;
#endif

#include <assert.h>
