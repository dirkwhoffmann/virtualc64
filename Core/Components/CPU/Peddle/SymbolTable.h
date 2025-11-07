#pragma once

#include <string>
// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#pragma once

#include "PeddleTypes.h"
#include <unordered_map>

using std::unordered_map;

namespace vc64::peddle {

struct FileEntry {

    isize id = -1;
    string name;
    isize size = 0;
    u64 mtime = 0;
    isize mod = -1;
};

struct LineEntry {

    isize id = -1;
    isize fileId = -1;
    isize lineNumber = 0;
    isize seg = -1;
};

struct SegmentEntry {

    isize id = -1;
    string name;
    u32 start = 0;
    u32 size = 0;
    string addrsize;
    string type;
    string oname;
    u32 ooffs = 0;
};

struct SpanEntry {

    isize id = -1;
    isize seg = -1;
    isize start = 0;
    isize size = 0;
};

struct SymbolEntry {

    isize id = -1;
    string name;
    u16 address = 0;
    isize scope = -1;
    isize type = -1;
    isize seg = -1;
};

template <typename T>
class BaseMap {

protected:

    std::unordered_map<isize, T> map;

public:

    const auto &get() { return map; }
    void clear() { map.clear(); }
    optional<std::reference_wrapper<const T>> get(isize id) const {
        return map.contains(id) ? map.at(id) : std::nullopt;
    }
};

class FileMap: public BaseMap<FileEntry> {

public:

    void parse(string_view line);
};

class LineMap: public BaseMap<LineEntry> {

public:

    void parse(string_view line);
};

class SegmentMap : public BaseMap<SegmentEntry> {

public:

    void parse(string_view line);
};

class SpanMap : public BaseMap<SpanEntry> {

public:

    void parse(string_view line);
};

class SymbolMap: public BaseMap<SymbolEntry> {

public:
    
    void parse(string_view line);

    optional<SymbolEntry> seek(u16 addr);
    optional<SymbolEntry> seek(const string &label);
};

class SymbolTable {

public:

    FileMap files;
    LineMap lines;
    SegmentMap segments;
    SpanMap spans;
    SymbolMap symbols;

    void clear();
    bool loadCS65File(const fs::path &path);

private:

    void parseLine(string_view line);

};

}
