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

#include "CoreComponent.h"
#include "CPUTypes.h"

namespace vc64 {

/* Base class for all subcomponents of the C64.
 */
class SubComponent : public CoreComponent, public References {

public:

    SubComponent(C64& ref);
    SubComponent(C64& ref, isize id);

    void prefix() const override;

    void markAsDirty();
};

}
