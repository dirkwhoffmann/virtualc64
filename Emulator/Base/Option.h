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
/// @file

#pragma once

#include "OptionTypes.h"
#include "Parser.h"
#include <memory>

namespace vc64 {

class OptionParser {

public:

    virtual ~OptionParser() = default;

protected:

    Option opt;
    i64 arg;
    string unit;

    OptionParser(Option opt) : opt(opt), arg(0) { };
    OptionParser(Option opt, const string &unit) : opt(opt), arg(0), unit(unit) { };
    OptionParser(Option opt, i64 arg) : opt(opt), arg(arg) { };
    OptionParser(Option opt, i64 arg, const string &unit) : opt(opt), arg(arg), unit(unit) { };

    // Factory method for creating the proper parser instance for an option
    static std::unique_ptr<OptionParser> create(Option opt, i64 arg = 0);

    // Parses an argument provides as string
    virtual i64 parse(const string &arg) { return 0; }

    // Data providers
    virtual string asPlainString() { return asString(); }
    virtual string asString() = 0;
    virtual string keyList() = 0;
    virtual string argList() = 0;

public:

    static i64 parse(Option opt, const string &arg);
    static string asPlainString(Option opt, i64 arg);
    static string asString(Option opt, i64 arg);
    static string keyList(Option opt);
    static string argList(Option opt);
};

class BoolParser : public OptionParser {

public:

    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { arg = util::parseBool(s); return arg; }
    virtual string asString() override { return arg ? "true" : "false"; }
    virtual string keyList() override { return "true, false"; }
    virtual string argList() override { return "{ true | false }"; }
};

class NumParser : public OptionParser {

public:
    
    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { arg = util::parseNum(s); return arg; }
    virtual string asPlainString() override { return std::to_string(arg); }
    virtual string asString() override { return asPlainString() + unit; }
    virtual string keyList() override { return "<value>"; }
    virtual string argList() override { return "<value>"; }
};

class HexParser : public OptionParser {

public:

    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { arg = util::parseNum(s); return arg; }
    virtual string asPlainString() override;
    virtual string asString() override { return asPlainString() + unit; }
    virtual string keyList() override { return "<value>"; }
    virtual string argList() override { return "<value>"; }
};

template <typename T>
class EnumParser : public OptionParser {

    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { return (arg = util::parseEnum<T>(s)); }
    virtual string asString() override { return T::key(isize(arg)); }
    virtual string keyList() override { return T::keyList(); }
    virtual string argList() override { return T::argList(); }
};

}
