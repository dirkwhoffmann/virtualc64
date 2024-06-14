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

#include "CoreObject.h"
#include "OptionTypes.h"
#include "Concurrency.h"
#include "IOUtils.h"

namespace vc64 {

namespace fs = ::std::filesystem;

class Defaults final : public CoreObject {

    mutable util::ReentrantMutex mutex;

    /// The key-value storage
    std::map <string, string> values;

    /// The default value storage
    std::map <string, string> fallbacks;


    //
    // Methods
    //

public:

    Defaults();
    Defaults(Defaults const&) = delete;
    const char *objectName() const override { return "Defaults"; }
    void operator=(Defaults const&) = delete;

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Loading and saving the key-value storage
    //

public:

    // Loads a storage file
    void load(const fs::path &path);
    void load(std::ifstream &stream);
    void load(std::stringstream &stream);

    // Saves a storage file
    void save(const fs::path &path);
    void save(std::ofstream &stream);
    void save(std::stringstream &stream);


    //
    // Reading key-value pairs
    //

public:

    // Queries a key-value pair
    string getString(const string &key) const;
    i64 getInt(const string &key) const;
    // i64 get(Option option) const;
    i64 get(Option option, isize nr = 0) const;

    // Queries a fallback key-value pair
    string getFallbackString(const string &key) const;
    i64 getFallbackInt(const string &key) const;
    i64 getFallback(Option option) const;
    i64 getFallback(Option option, isize nr) const;


    //
    // Writing key-value pairs
    //

    // Writes a key-value pair into the user storage
    void setString(const string &key, const string &value);
    void set(Option opt, i64 value, isize nr = 0);
    void set(Option opt, i64 value, std::vector<isize> nrs);

    // Writes a key-value pair into the fallback storage
    void setFallback(const string &key, const string &value);
    void setFallback(Option opt, const string &value, isize nr = 0);
    void setFallback(Option opt, i64 value, isize nr = 0);
    void setFallback(Option opt, const string &value, std::vector <isize> nrs);
    void setFallback(Option opt, i64 value, std::vector <isize> nrs);


    //
    // Deleting key-value pairs
    //

    // Deletes all key-value pairs
    void remove();

    // Deletes selected key-value pairs
    void remove(const string &key) throws;
    void remove(Option option, isize nr = 0) throws;
    void remove(Option option, std::vector <isize> nrs) throws;
};

}
