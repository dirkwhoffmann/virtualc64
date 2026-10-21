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

    {   SYNCHRONIZED

        logmsg(LOG_CMD, "%s [%llx]\n", CmdEnum::key(cmd.type), cmd.value);

        if (!queue.isFull()) {

            auto w = queue.end();
            queue.write(cmd);
            attachments[w] = str;
            queue.elements[w].str = attachments[w].c_str();
            completions[w] = std::move(completion);

            pending++;

        } else {
            logmsg(LOG_WARN, "Command lost: %s [%llx]\n", CmdEnum::key(cmd.type), cmd.value);
            lost = true;
        }

        empty = false;
    }

    // Don't leave a waiting sender hanging
    if (lost && completion) {
        completion(std::make_exception_ptr(std::runtime_error("Command queue is full")));
    }
}

void
CmdQueue::done()
{
    if (--pending == 0) pending.notify_all();
}

void
CmdQueue::wait()
{
    for (isize n; (n = pending.load()) != 0; ) pending.wait(n);
}

bool
CmdQueue::poll(Command &cmd, Completion &completion)
{
    if (empty) return false;

    {   SYNCHRONIZED

        auto r = queue.begin();

        cmd = queue.read();
        completion = std::move(completions[r]);
        completions[r] = nullptr;
        empty = queue.isEmpty();

        return true;
    }
}

}
