// -----------------------------------------------------------------------------
// This file is part of rvlib
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"

/* utlib no longer exports string, vector, span, etc. into the global namespace.
 * The rvlib code relies on these names, so it makes them available inside its
 * own namespace.
 */
namespace retro::vault { using namespace utl; }
