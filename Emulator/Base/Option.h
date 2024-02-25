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

#include "OptionTypes.h"
#include "Parser.h"

namespace vc64 {

class OptionParser {

protected:

    Option opt;
    i64 arg;

    OptionParser(Option opt) : opt(opt), arg(0) { };
    virtual ~OptionParser() { };

    // Factory method for creating the proper parser instance
    static OptionParser create(Option opt);

    virtual i64 parse(const string &arg) { return 0; }
    static i64 parse(Option opt, const string &arg) { return 0; }

    virtual string asString() { return "asString: TODO:"; }
    virtual string keyList() { return "keyList: TODO"; }
    virtual string argList() { return "argList: TODO"; }
    const char *help();
};

class BoolParser : public OptionParser {

    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { arg = util::parseBool(s); return arg; }
    virtual string asString() override { return arg ? "true" : "false"; }
    virtual string keyList() override { return "<bool>"; }
    virtual string argList() override { return "<bool>"; }
};

class NumParser : public OptionParser {

    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { arg = util::parseNum(s); return arg; }
    virtual string asString() override { return std::to_string(arg); }
    virtual string keyList() override { return "<num>"; }
    virtual string argList() override { return "<num>"; }
};

template <typename T>
class EnumParser : public OptionParser {

    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { return (arg = util::parseEnum<T>(s)); }
    virtual string asString() override { return T::key(arg); }
    virtual string keyList() override { return T::keyList(); }
    virtual string argList() override { return T::argList(); }
};

}
