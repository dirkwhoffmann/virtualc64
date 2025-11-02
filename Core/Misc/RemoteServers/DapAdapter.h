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

#include "SubComponent.h"
#include "DapAdapterTypes.h"
#include "DapServerTypes.h"
#include "json.h"

using json = nlohmann::json;

namespace vc64 {

class DapAdapter final : public CoreObject, public References {

    isize seqCounter;


    //
    // Initializing
    //

public:

    using References::References;


    //
    // Methods from CoreObject
    //

private:

    const char *objectName() const override { return "DapAdapter"; }
    void _dump(Category category, std::ostream &os) const override;


    //
    // Handling packets
    //

public:

    // Processes an DAP command given as string
    void process(const string &packet) throws;

    // Processes an DAP command given as a JSON object
    void process(const json &j) throws;

private:

    // Creates a new seq number
    isize nextSeq() { return seqCounter++; }

    // Processes a single command
    template <dap::Command> void process(isize seq, const string &packet) throws;

    // Sends a packet to the connected client
    void reply(const string &payload);

    void replySuccess(isize seq, const string &command);

    
    //
    // Reading the emulator state
    //

    // Reads a register value
    string readRegister(isize nr);

    // Reads a byte from memory
    string readMemory(isize addr);


    //
    // Delegation methods
    //

public:

    void breakpointReached();
};

}
