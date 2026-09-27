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

#include "vcconfig.h"
#include "Printer.h"
#include "SerialPort.h"
#include "MsgQueue.h"

namespace vc64 {

void
Printer::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("Connected");
        os << bol(config.connected) << std::endl;
        os << tab("Device number");
        os << dec(config.deviceNum) << std::endl;
    }
}

void
Printer::cacheInfo(PrinterInfo &result) const
{
    result.pages = paper.pages();
    result.rows = paper.rows() - paper.pageStart(paper.pages());
}

i64
Printer::getOption(Opt option) const
{
    switch (option) {

        case Opt::PRT_CONNECTED:    return (i64)config.connected;
        case Opt::PRT_DEVICE_NUM:   return (i64)config.deviceNum;

        default:
            fatalError;
    }
}

void
Printer::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::PRT_CONNECTED:

            return;

        case Opt::PRT_DEVICE_NUM:

            if (value != 4 && value != 5) {
                throw CoreError(CoreError::OPT_INV_ARG, "4, 5");
            }
            return;

        default:
            throw CoreError(CoreError::OPT_UNSUPPORTED);
    }
}

void
Printer::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case Opt::PRT_CONNECTED:

            if (config.connected != (bool)value) {

                config.connected = (bool)value;

                // Connecting or disconnecting mid-transfer must not leave
                // the listener's state machine (e.g. addressed/listening,
                // or a pending ack hold) hanging around: reset it, then ask
                // the bus to recompute its lines, since this device's own
                // contribution may just have appeared or disappeared.
                iec.reset();
                serialPort.setNeedsUpdate();
                msgQueue.put(Msg::PRT_CONNECT, value);
            }
            return;

        case Opt::PRT_DEVICE_NUM:

            config.deviceNum = (isize)value;
            iec.deviceNum = config.deviceNum;
            return;

        default:
            fatalError;
    }
}

}
