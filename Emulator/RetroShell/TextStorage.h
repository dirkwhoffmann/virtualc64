// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Macros.h"
#include <sstream>
#include <fstream>
#include <vector>

namespace vc64 {

class TextStorage {

    // Maximum number of stored lines
    static constexpr usize capacity = 512;

    // The stored lines
    std::vector<string> storage;

public:

    // Optional output stream for debugging
    std::ostream *ostream = nullptr;


    //
    // Reading
    //

public:

    // Returns the number of stored lines
    isize size() const { return (isize)storage.size(); }

    // Returns a single line
    string operator [] (isize i) const;
    string& operator [] (isize i);

    // Returns the whole storage contents
    void text(string &all);


    //
    // Writing
    //

public:

    // Initializes the storage with a single empty line
    void clear();

private:

    // Appends a new line
    void append(const string &line);

public:

    // Appends a single character or a string
    TextStorage &operator<<(char c);
    TextStorage &operator<<(const string &s);
    TextStorage &operator<<(std::stringstream &ss);
};

}
