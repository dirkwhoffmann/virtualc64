// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "HardwareComponent.h"

class C64;
class C64Memory;
class DriveMemory;
template <typename M> class CPU;
class ProcessorPort;
class VICII;
class CIA1;
class CIA2;
class SIDBridge;
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
class Oscillator;
class RetroShell;

// class File;
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

//
// Base class for all C64 components
//

class C64Component : public HardwareComponent {

protected:

    C64 &c64;
    C64Memory &mem;
    CPU<C64Memory> &cpu;
    VICII &vic;
    CIA1 &cia1;
    CIA2 &cia2;
    SIDBridge &sid;
    Keyboard &keyboard;
    ControlPort &port1;
    ControlPort &port2;
    ExpansionPort &expansionport;
    IEC &iec;
    Drive &drive8;
    Drive &drive9;
    Datasette &datasette;
    MsgQueue &messageQueue;
    Oscillator &oscillator;
    RetroShell &retroShell;
    
    Drive *drive[2] = { &drive8, &drive9 };

public:

    C64Component(C64& ref);

    void suspend();
    void resume();
    
    void prefix() const override;
};
