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

#include <cstdio>
#include <string>
#include <vector>

namespace vc64::test {

struct TestCase {

    const char *name;
    void (*fn)();
};

// Returns the global registry of test cases
std::vector<TestCase> &registry();

// Records a failure for the currently running test
void fail(const char *file, int line, const std::string &msg);

struct Registrar {

    Registrar(const char *name, void (*fn)()) { registry().push_back({ name, fn }); }
};

}

#define TEST(name)                                                            \
    static void name();                                                       \
    static vc64::test::Registrar reg_##name(#name, name);                     \
    static void name()

#define CHECK(cond)                                                           \
    do {                                                                      \
        if (!(cond)) vc64::test::fail(__FILE__, __LINE__, "CHECK(" #cond ")"); \
    } while (0)

#define CHECK_EQ(a, b)                                                        \
    do {                                                                      \
        auto va_ = (a);                                                       \
        auto vb_ = (b);                                                       \
        if (!(va_ == vb_)) {                                                  \
            vc64::test::fail(__FILE__, __LINE__,                              \
                std::string(#a " == " #b " (got ") +                          \
                std::to_string(va_) + " vs " + std::to_string(vb_) + ")");    \
        }                                                                     \
    } while (0)
