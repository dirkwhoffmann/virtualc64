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

#include "PrinterTypes.h"
#include "SubComponent.h"
#include "IecListener.h"
#include "Paper.h"
#include "Mps803.h"

namespace vc64 {

class Printer final : public SubComponent, public Inspectable<PrinterInfo>, public IecHost {

    Descriptions descriptions = {{

        .type           = Class::Printer,
        .name           = "Printer",
        .description    = "MPS-803 Printer",
        .shell          = "printer"
    }};

    Options options = {

        Opt::PRT_CONNECTED,
        Opt::PRT_DEVICE_NUM
    };

    // Current configuration
    PrinterConfig config = { };

public:

    // Hard cap on how many pages a single print job may accumulate on the
    // (nominally infinite) paper roll. Paper is deliberately unbounded by
    // design (it models a continuous roll -- see the class comment in
    // Paper.h), so a runaway job (e.g. an infinite PRINT# loop in a BASIC
    // program) would otherwise grow Paper::dots without bound and eventually
    // exhaust memory. This is Printer-level policy, not Paper's concern:
    // once the cap is reached, iecByte() silently drops further bytes
    // instead of rendering them (matching how a real printer behaves when it
    // runs out of paper) rather than throwing, which would kill the
    // emulation thread mid-print. 500 pages is roughly 22 MB of dot data --
    // far more than any real print job needs, but small enough to bound
    // worst-case memory use.
    static constexpr isize maxPages = 500;

    // The IEC bus protocol state machine
    IecListener iec = IecListener(*this);

    // The printed output
    Paper paper;

    // The MPS-803 character ROM (user-supplied, like the C64's own ROMs)
    Mps803 mps803 = Mps803(paper);


    //
    // Methods
    //

public:

    Printer(C64 &ref) : SubComponent(ref) { };

    Printer& operator= (const Printer& other) {

        CLONE(config)
        CLONE(iec)
        CLONE(paper)
        CLONE(mps803)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << config.connected
        << config.deviceNum;

        iec.serialize(worker);
        paper.serialize(worker);
        mps803.serialize(worker);

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream &os) const override;
    void _didReset(bool hard) override;


    //
    // Methods from Inspectable
    //

public:

    void cacheInfo(PrinterInfo &result) const override;


    //
    // Methods from Configurable
    //

public:

    const PrinterConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Opt opt) const override;
    void checkOption(Opt opt, i64 value) override;
    void setOption(Opt opt, i64 value) override;


    //
    // Accessing
    //

public:

    bool isConnected() const { return config.connected; }
    isize deviceNum() const { return config.deviceNum; }


    //
    // Methods from IecHost
    //

public:

    void iecSchedule(Cycle delay) override;
    void iecCancel() override;
    void iecLinesChanged() override;
    void iecListen(u8 sa) override;
    void iecUnlisten() override;
    void iecByte(u8 value, bool eoi) override;

    // Fired by the event loop when the PRT_WAKEUP event becomes due
    void processPrtEvent(EventID id);

    // This device's own contribution to the CLOCK/DATA bus lines, in
    // pin-value form (true = pulling the line low). Always false while the
    // printer is not connected.
    bool iecClock() const;
    bool iecData() const;
};

}
