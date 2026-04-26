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
#include "utl/io/Parser.h"
#include <memory>

namespace vc64 {

class OptionParser {

public:

    virtual ~OptionParser() = default;

protected:

    Opt opt;
    i64 arg;
    string unit;

    OptionParser(Opt opt) : opt(opt), arg(0) { };
    OptionParser(Opt opt, const string &unit) : opt(opt), arg(0), unit(unit) { };
    OptionParser(Opt opt, i64 arg) : opt(opt), arg(arg) { };
    OptionParser(Opt opt, i64 arg, const string &unit) : opt(opt), arg(arg), unit(unit) { };

    // Factory method for creating the proper parser instance for an option
    static std::unique_ptr<OptionParser> create(Opt opt, i64 arg = 0);

    // Parses an argument provides as string
    virtual i64 parse(const string &arg) { return 0; }

    // Data providers
    virtual std::vector <std::pair<string,long>> pairs() { return {}; }
    virtual string asPlainString() { return asString(); }
    virtual string asString() = 0;
    virtual string keyList() = 0;
    virtual string argList() = 0;
    virtual string help(i64 arg) { return ""; }

public:

    static i64 parse(Opt opt, const string &arg);
    static std::vector<std::pair<string, long>> pairs(Opt opt);
    static string asPlainString(Opt opt, i64 arg);
    static string asString(Opt opt, i64 arg);
    static string keyList(Opt opt);
    static string argList(Opt opt);
    static string help(Opt opt, i64 arg);
};

class BoolParser : public OptionParser {

public:

    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { arg = utl::parseBool(s); return arg; }
    virtual string asString() override { return arg ? "true" : "false"; }
    virtual string keyList() override { return "true, false"; }
    virtual string argList() override { return "{ true | false }"; }
};

class NumParser : public OptionParser {

public:

    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { arg = utl::parseNum(s); return arg; }
    virtual string asPlainString() override { return std::to_string(arg); }
    virtual string asString() override { return asPlainString() + unit; }
    virtual string keyList() override { return "<value>"; }
    virtual string argList() override { return "<value>"; }
};

class HexParser : public OptionParser {

public:

    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { arg = utl::parseNum(s); return arg; }
    virtual string asPlainString() override;
    virtual string asString() override { return asPlainString() + unit; }
    virtual string keyList() override { return "<value>"; }
    virtual string argList() override { return "<value>"; }
};

template <class T, typename E>
class EnumParser : public OptionParser {

    using OptionParser::OptionParser;

    virtual i64 parse(const string &s) override { return (arg = utl::parseEnum<T>(s)); }
    std::vector <std::pair<string,long>> pairs() override { return T::pairs(); }
    virtual string asString() override { return T::key(E(arg)); }
    virtual string keyList() override { return T::keyList(); }
    virtual string argList() override { return T::argList(); }
    virtual string help(i64 arg) override { return T::help(E(arg)); }
};

}
