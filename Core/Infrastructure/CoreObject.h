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

#include "Error.h"
#include "CoreError.h"
#include "Dumpable.h"
#include "utl/abilities/Loggable.h"
#include "utl/abilities/Reportable.h"

namespace vc64 {

class CoreObject : public Loggable, Reportable, public Dumpable {
    
protected:
    
    // Verbosity level
    static isize verbosity;
    
    
    //
    // Initializing
    //
    
public:
    
    virtual ~CoreObject() = default;
    
    // Returns the name for this component
    virtual const char *objectName() const = 0;
    
    // Returns a textual description for this component
    virtual const char *description() const { return ""; }
    
    // Called by logging functions to produce detailed debug output
    virtual string prefix(LogLevel, const std::source_location &) const override;

    
    //
    // Reporting state
    //

protected:

    const Report makeReport(isize category) const override { return {{ "Name", objectName() }}; }

};

}
