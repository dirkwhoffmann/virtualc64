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
#include "json_fwd.h"
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
// public:
    
    std::unordered_map<isize, T> map;

public:

    const auto &get() { return map; }
    isize size() { return (isize)map.size(); }

    /*
    optional<T> get(isize id) const {
        return map.contains(id) ? std::optional<T>(map.at(id)) : std::nullopt;
    }
    */
    optional<T> operator[](isize id) const {
        return map.contains(id) ? std::optional<T>(map.at(id)) : std::nullopt;
    }
};

class FileMap: public BaseMap<FileEntry> {

public:

    void clear() { map.clear(); }
    void parse(string_view line);

    optional<FileEntry> seek(const string &name) const;
};

class LineMap: public BaseMap<LineEntry> {

public:

    void clear() { map.clear(); }
    void parse(string_view line);

    optional<LineEntry> seek(isize lineId, isize fileId);
};

class SegmentMap : public BaseMap<SegmentEntry> {

public:

    void clear() { map.clear(); }
    void parse(string_view line);
};

class SpanMap : public BaseMap<SpanEntry> {

public:

    void clear() { map.clear(); }
    void parse(string_view line);
};

class SymbolMap: public BaseMap<SymbolEntry> {

    std::unordered_map<u16, isize> valToId;
    std::unordered_map<string, isize> nameToId;

public:

    void clear() { map.clear(); valToId.clear(); nameToId.clear(); }
    void dump();
    void parse(string_view line);

    optional<SymbolEntry> seek(u16 addr) const;
    optional<SymbolEntry> seek(const string &label) const;
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
    optional<u16> resolveAddr(const string &file, isize line);

private:

    void parseLine(string_view line);

};

}
