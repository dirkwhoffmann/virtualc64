// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS Technology 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#pragma once

#include "PeddleTypes.h"
#include "CPUDebugger.h"
#include "SubComponent.h"

class Peddle : public SubComponent {

    friend class CPUDebugger;
    friend class Breakpoints;
    friend class Watchpoints;

    //
    // Configuration
    //

protected:

    // Instance counter (to easily distinguish different CPUs)
    isize id;

    // Emulated CPU model
    CPURevision cpuModel = MOS_6510;


    //
    // Registers
    //

public:

    Registers reg;
    

    //
    // Constructing
    //

public:

    Peddle(C64 &ref);
    virtual ~Peddle();


    //
    // Accessing properties
    //

    bool getN() const { return reg.sr.n; }
    void setN(bool value) { reg.sr.n = value; }

    bool getV() const { return reg.sr.v; }
    void setV(bool value) { reg.sr.v = value; }

    bool getB() const { return reg.sr.b; }
    void setB(bool value) { reg.sr.b = value; }

    bool getD() const { return reg.sr.d; }
    void setD(bool value) { reg.sr.d = value; }

    bool getI() const { return reg.sr.i; }
    void setI(bool value) { reg.sr.i = value; }

    bool getZ() const { return reg.sr.z; }
    void setZ(bool value) { reg.sr.z = value; }

    bool getC() const { return reg.sr.c; }
    void setC(bool value) { reg.sr.c = value; }
    

    //
    // Operating the Arithmetical Logical Unit (ALU)
    //

protected:

    void adc(u8 op);
    void adc_binary(u8 op);
    void adc_bcd(u8 op);
    void sbc(u8 op);
    void sbc_binary(u8 op);
    void sbc_bcd(u8 op);
    void cmp(u8 op1, u8 op2);
};
