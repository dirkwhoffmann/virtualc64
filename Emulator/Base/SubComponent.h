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

class References {

protected:

    class AudioPort &audioPort;
    class C64 &c64;
    class CIA1 &cia1;
    class CIA2 &cia2;
    class ControlPort &port1;
    class ControlPort &port2;
    class CPU &cpu;
    class Datasette &datasette;
    class Debugger &debugger;
    class Drive &drive8;
    class Drive &drive9;
    class ExpansionPort &expansionPort;
    class Host &host;
    class SerialPort &serialPort;
    class Keyboard &keyboard;
    class C64Memory &mem;
    class Monitor &monitor;
    class MsgQueue &msgQueue;
    class ParCable &parCable;
    class PowerPort &powerSupply;
    class Recorder &recorder;
    class RegressionTester &regressionTester;
    class RetroShell &retroShell;
    class SIDBridge &sidBridge;
    class SID& sid0;
    class SID& sid1;
    class SID& sid2;
    class SID& sid3;
    class VICII &vic;
    class VideoPort &videoPort;

    Drive *drive[2] = { &drive8, &drive9 };

public:

    References(C64& ref);
};


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
