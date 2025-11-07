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

    isize id;
    string name;
    isize size;
    u64 mtime;
    isize mod;
};

struct LineEntry {

    isize id;
    isize file;
    isize line;
    isize span;
};

struct SegmentEntry {

    isize id;
    string name;
    u32 start;
    u32 size;
    string addrsize;
    string type;
    string oname;
    u32 ooffs;
};

struct SpanEntry {

    isize id;
    isize seg;
    isize start;
    isize size;
};

struct SymbolEntry {

    isize id;
    string name;
    string addrsize;
    isize size;
    isize scope;
    u16 val;
    isize seg;
    string type;
};

template <typename T>
class BaseMap {

protected:

    std::unordered_map<isize, T> map;

public:

    const auto &get() { return map; }
    void clear() { map.clear(); }
    isize size() { return (isize)map.size(); }
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
