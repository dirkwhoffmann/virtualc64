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
#include "Synchronizable.h"

namespace vc64 {

namespace fs = ::std::filesystem;

class Defaults final : public CoreObject, public Synchronizable {

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
    void operator=(Defaults const&) = delete;


    //
    // Methods from CoreObject
    //

private:

    const char *objectName() const override { return "Defaults"; }
    void _dump(Category category, std::ostream& os) const override;


    //
    // Loading and saving
    //

public:

    // Loads a properties file from disk
    void load(const fs::path &path) throws;
    void load(std::ifstream &stream) throws;
    void load(std::stringstream &stream) throws;

    // Saves a properties file to disk
    void save(const fs::path &path) throws;
    void save(std::ofstream &stream) throws;
    void save(std::stringstream &stream) throws;


    //
    // Reading key-value pairs
    //

public:

    // Queries a key-value pair
    string getRaw(const string &key) const;
    i64 get(const string &key) const;
    i64 get(Option option, isize nr = 0) const;

    // Queries a fallback key-value pair
    string getFallbackRaw(const string &key) const;
    i64 getFallback(const string &key) const;
    i64 getFallback(Option option, isize nr = 0) const;


    //
    // Writing key-value pairs
    //

    // Writes a key-value pair into the user storage
    void set(const string &key, const string &value);
    void set(Option option, const string &value);
    void set(Option option, const string &value, std::vector<isize> objids);
    void set(Option option, i64 value);
    void set(Option option, i64 value, std::vector<isize> objids);

    // Writes a key-value pair into the fallback storage
    void setFallback(const string &key, const string &value);
    void setFallback(Option option, const string &value);
    void setFallback(Option option, const string &value, std::vector<isize> objids);
    void setFallback(Option option, i64 value);
    void setFallback(Option option, i64 value, std::vector<isize> objids);


    //
    // Deleting key-value pairs
    //

    // Deletes all key-value pairs
    void remove();

    // Deletes selected key-value pairs
    void remove(const string &key) throws;
    void remove(Option option) throws;
    void remove(Option option, std::vector <isize> objids) throws;
};

}
