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

namespace vc64 {

class PIA6821 : public SubComponent {
    
    friend class Drive;
    friend class ParCable;

    Descriptions descriptions = {{

        .name           = "PIA",
        .description    = "PIA 6821"
    }};

protected:
    
    // Owner of this PIA
    Drive &drive;

    // Peripheral ports (pin values)
    u8 pa;
    u8 pb;

    // Output registers
    u8 ora;
    u8 orb;
    
    // Data direction registers
    u8 ddra;
    u8 ddrb;
    
    // Control registers
    u8 cra;
    u8 crb;
    
    // Interrupt control lines
    bool ca1;
    bool ca2;
    bool cb1;
    bool cb2;

    
    //
    // Methods
    //
    
public:
    
    PIA6821(C64 &ref, Drive &drvref);

    PIA6821& operator= (const PIA6821& other) {

        CLONE(pa)
        CLONE(pb)
        CLONE(ora)
        CLONE(orb)
        CLONE(ddra)
        CLONE(ddrb)
        CLONE(cra)
        CLONE(crb)
        CLONE(ca1)
        CLONE(ca2)
        CLONE(cb1)
        CLONE(cb2)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        worker

        << pa
        << pb
        << ora
        << orb
        << ddra
        << ddrb
        << cra
        << crb
        << ca1
        << ca2
        << cb1
        << cb2;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }


    //
    // Managing interrupts
    //

public:
    
    void setCA1External(bool value);
    void setCA2External(bool value);
    void setCB1External(bool value);
    void setCB2External(bool value);

    void toggleCA1External() { setCA1External(!ca1); }
    void toggleCA2External() { setCA1External(!ca2); }
    void toggleCB1External() { setCA1External(!cb1); }
    void toggleCB2External() { setCA1External(!cb2); }

    void pulseCA1External() { toggleCA1External(); toggleCA1External(); }
    
private:
    
    void setCA2Internal(bool value);
    void setCB2Internal(bool value);

    u8 ca2Control() const { return (cra >> 3) & 0b111; }
    u8 cb2Control() const { return (crb >> 3) & 0b111; }
    bool ca2IsInput() const { return GET_BIT(cra, 5); }
    bool cb2IsInput() const { return GET_BIT(crb, 5); }

    bool isActiveTransitionCA1(bool oldValue, bool newValue) const;
    bool isActiveTransitionCA2(bool oldValue, bool newValue) const;
    bool isActiveTransitionCB1(bool oldValue, bool newValue) const;
    bool isActiveTransitionCB2(bool oldValue, bool newValue) const;

    // Callbacks (delegation methods)
    virtual void ca2HasChangedTo(bool value) { };
    virtual void cb2HasChangedTo(bool value) { };
    virtual void irqAHasOccurred() const { };
    virtual void irqBHasOccurred() const { };


    //
    // Accessing registers
    //
    
public:
    
    // Reads the data bus
    u8 peek(bool rs1, bool rs0);
    u8 spypeek(bool rs1, bool rs0) const { return 0; } // TODO
    
    // Writes the data bus
    void poke(bool rs1, bool rs0, u8 value);

protected:

    // Gets the port values up-to-date
    virtual u8 updatePA();
    virtual u8 updatePB();
};

//
// PIA (DolphinDOS 3)
//

class PiaDolphin final : public PIA6821 {

public:

    using PIA6821::PIA6821;
        
private:

    void ca2HasChangedTo(bool value) override;
    void cb2HasChangedTo(bool value) override;
    void irqAHasOccurred() const override;
    void irqBHasOccurred() const override;
    u8 updatePA() override;
    u8 updatePB() override;
    
public:
    
    u8 peek(u16 addr);
    u8 spypeek(u16 addr) const;
    void poke(u16 addr, u8 value);
};

}
