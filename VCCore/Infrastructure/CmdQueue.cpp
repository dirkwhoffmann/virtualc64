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
#include "CmdQueue.h"
#include <stdexcept>

namespace vc64 {

void
CmdQueue::put(const Command &cmd, const string &str, Completion completion)
{
    bool lost = false;
    bool announce = false;

    {   SYNCHRONIZED

        logmsg(LOG_CMD, "%s [%llx]\n", CmdEnum::key(cmd.type), cmd.value);

        if (isize(entries.size()) < CAPACITY) {

            entries.push_back(Entry { cmd, str, std::move(completion) });

        } else {

            logmsg(LOG_WARN, "Command lost: %s [%llx]\n", CmdEnum::key(cmd.type), cmd.value);
            lost = true;
            announce = !overflowing;
            overflowing = true;
        }

        empty = false;
    }

    // Don't leave a waiting sender hanging
    if (lost && completion) {
        completion(std::make_exception_ptr(std::runtime_error("Command queue is full")));
    }

    // Tell the application once per overflow
    if (announce && onFull) onFull();
}

bool
CmdQueue::poll(Command &cmd, Completion &completion)
{
    if (empty) return false;

    {   SYNCHRONIZED

        if (entries.empty()) { empty = true; return false; }

        auto &entry = entries.front();

        // Keep the attached string alive until the next call
        current = std::move(entry.str);
        cmd = entry.cmd;
        cmd.str = current.c_str();
        completion = std::move(entry.completion);

        entries.pop_front();
        if (isize(entries.size()) < CAPACITY) overflowing = false;
        empty = entries.empty();

        return true;
    }
}

}
