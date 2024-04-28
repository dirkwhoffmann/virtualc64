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

#include "config.h"
#include "CmdQueue.h"

namespace vc64 {

void
CmdQueue::put(const Cmd &cmd)
{
    {   SYNCHRONIZED

        debug(CMD_DEBUG, "%s [%llx]\n", CmdTypeEnum::key(cmd.type), cmd.value);

        if (!queue.isFull()) {
            queue.write(cmd);
        } else {
            warn("Command lost: %s [%llx]\n", CmdTypeEnum::key(cmd.type), cmd.value);
        }

        empty = false;
    }
}

bool
CmdQueue::poll(Cmd &cmd)
{
    if (empty) return false;

    {   SYNCHRONIZED

        cmd = queue.read();
        empty = queue.isEmpty();

        return true;
    }
}

}

