// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"
#include "CPUTypes.h"

class C64;
class C64Memory;
class DriveMemory;
template <CPURevision C> class CPU;
class ProcessorPort;
class VICII;
class CIA1;
class CIA2;
class Muxer;
class SIDStream;
class Keyboard;
class ControlPort;
class ExpansionPort;
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

/* This class extends the C64Component class with references to all components
 * that are part of the C64 class.
 */
class SubComponent : public C64Component {

protected:

    C64 &c64;
    CIA1 &cia1;
    CIA2 &cia2;
    ControlPort &port1;
    ControlPort &port2;
    CPU<MOS_6510> &cpu;
    Datasette &datasette;
    Drive &drive8;
    Drive &drive9;
    ExpansionPort &expansionport;
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
