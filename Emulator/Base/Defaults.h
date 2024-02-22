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
#include "EmulatorTypes.h"
#include "Concurrency.h"
//#include "CoreComponent.h"
#include "IOUtils.h"

namespace vc64 {

class Defaults final : public CoreObject, public Dumpable {

    mutable util::ReentrantMutex mutex;

    // Key-value storage
    std::map <string, string> values;

    // Fallback values (used if no value is set)
    std::map <string, string> fallbacks;


    //
    // Initializing
    //

public:

    Defaults();
    Defaults(Defaults const&) = delete;
    void operator=(Defaults const&) = delete;


    //
    // Methods from CoreObject
    //

private:

    const char *getDescription() const override { return "Properties"; }
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
    // Working with key-value pairs
    //

public:

    string getString(const string &key) throws;
    i64 getInt(const string &key) throws;
    i64 get(Option option) throws;
    i64 get(Option option, isize nr) throws;

    string getFallback(const string &key) throws;

    void setString(const string &key, const string &value);
    void set(Option option, i64 value);
    void set(Option option, isize nr, i64 value);
    void set(Option option, std::vector <isize> nrs, i64 value);

    void setFallback(const string &key, const string &value);
    void setFallback(Option option, const string &value);
    void setFallback(Option option, i64 value);
    void setFallback(Option option, isize nr, const string &value);
    void setFallback(Option option, isize nr, i64 value);
    void setFallback(Option option, std::vector <isize> nrs, const string &value);
    void setFallback(Option option, std::vector <isize> nrs, i64 value);

    void remove();
    void remove(const string &key) throws;
    void remove(Option option) throws;
    void remove(Option option, isize nr) throws;
    void remove(Option option, std::vector <isize> nrs) throws;
};

}
