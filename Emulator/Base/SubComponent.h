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

class C64;
class C64Memory;
class DriveMemory;
class CPU;
class VICII;
class CIA1;
class CIA2;
class Muxer;
class SIDStream;
class Keyboard;
class ControlPort;
class ExpansionPort;
class Host;
class IEC;
class Drive;
class Datasette;
class Mouse;
class Mouse1350;
class Mouse1351;
class NeosMouse;
class MsgQueue;
class PowerSupply;
class ParCable;
class Recorder;
class RegressionTester;
class RetroShell;
class AnyFile;
class AnyCollection;
class TAPFile;
class T64File;
class PRGFile;
class P00File;
class D64File;
class G64File;
class CRTFile;
class RomFile;
class Snapshot;

/* This class extends the CoreComponent class with references to all components
 * that are part of the C64 class.
 */
class SubComponent : public CoreComponent {

protected:

    C64 &c64;
    CIA1 &cia1;
    CIA2 &cia2;
    ControlPort &port1;
    ControlPort &port2;
    CPU &cpu;
    Datasette &datasette;
    Drive &drive8;
    Drive &drive9;
    ExpansionPort &expansionport;
    Host &host;
    IEC &iec;
    Keyboard &keyboard;
    C64Memory &mem;
    MsgQueue &msgQueue;
    PowerSupply &oscillator;
    ParCable &parCable;
    Recorder &recorder;
    RegressionTester &regressionTester;
    RetroShell &retroShell;
    Muxer &muxer;
    VICII &vic;

    Drive *drive[2] = { &drive8, &drive9 };

public:

    SubComponent(C64& ref);
    SubComponent(const SubComponent &other) : SubComponent(other.c64) { }
    
    virtual bool isPoweredOff() const override;
    virtual bool isPoweredOn() const override;
    virtual bool isPaused() const override;
    virtual bool isRunning() const override;
    virtual bool isSuspended() const override;
    virtual bool isHalted() const override;

    virtual void suspend() override;
    virtual void resume() override;

    void prefix() const override;
};

}
