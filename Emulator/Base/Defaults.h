// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"
#include "IOUtils.h"

namespace vc64 {

class Defaults : public C64Object {

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
    // Methods from AmigaObject
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
