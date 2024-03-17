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
#include "TextStorage.h"
#include "C64.h"

namespace vc64 {

string
TextStorage::operator [] (isize i) const
{
    assert(i >= 0 && i < size());
    return storage[i];
}

string&
TextStorage::operator [] (isize i)
{
    assert(i >= 0 && i < size());
    return storage[i];
}

void
TextStorage::text(string &all)
{
    auto count = size();

    all = "";
    for (isize i = 0; i < count; i++) {

        all += storage[i];
        if (i < count - 1) all += '\n';
    }
}

void
TextStorage::clear()
{
    storage.clear();
    storage.push_back("");
}

void
TextStorage::append(const string &line)
{
    storage.push_back(line);

    // Remove old entries if the storage grows too large
    while (storage.size() > capacity) storage.erase(storage.begin());
}

TextStorage&
TextStorage::operator<<(char c)
{
    assert(!storage.empty());

    switch (c) {

        case '\n':

            if (ostream) *ostream << storage.back() << std::endl;
            append("");
            break;

        case '\r':

            storage.back() = "";
            break;

        default:

            if (isprint(c)) storage.back() += c;
            break;
    }

    return *this;
}

TextStorage&
TextStorage::operator<<(const string &s)
{
    for (auto &c : s) *this << c;
    return *this;
}

TextStorage &
TextStorage::operator<<(std::stringstream &ss)
{
    string line;
    while(std::getline(ss, line)) *this << line << '\n';
    return *this;
}

}
