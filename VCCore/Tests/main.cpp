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

#include "TestSupport.h"

namespace vc64::test {

static int failures = 0;

std::vector<TestCase> &
registry()
{
    static std::vector<TestCase> instance;
    return instance;
}

void
fail(const char *file, int line, const std::string &msg)
{
    failures++;
    std::printf("  FAIL %s:%d: %s\n", file, line, msg.c_str());
}

}

int
main()
{
    using namespace vc64::test;

    for (auto &test : registry()) {

        std::printf("%s\n", test.name);
        test.fn();
    }

    std::printf("%zu tests, %d failures\n", registry().size(), failures);
    return failures == 0 ? 0 : 1;
}
