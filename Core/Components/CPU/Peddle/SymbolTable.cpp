// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#include "PeddleConfig.h"
#include "SymbolTable.h"
#include "StringUtils.h"
#include "Parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace vc64::peddle {

inline u16 parseU16(string_view sv) { return (u16)util::parseNum(string(sv)); }
inline u32 parseU32(string_view sv) { return (u32)util::parseNum(string(sv)); }
inline u64 parseU64(string_view sv) { return (u64)util::parseNum(string(sv)); }
inline isize parseNum(string_view sv) { return (isize)util::parseNum(string(sv)); }

inline void parseKeyValueLine(string_view line, auto &&handler) {

    usize start = 0;
    while (start < line.size()) {

        auto comma = line.find(',', start);
        string_view token = line.substr(start, comma - start);
        auto eq = token.find('=');

        if (eq != string_view::npos) {

            string_view field = token.substr(0, eq);
            string_view value = token.substr(eq + 1);
            handler(field, util::unquote(value));
        }

        if (comma == string_view::npos) break;
        start = comma + 1;
    }
}

void
FileMap::parse(string_view line)
{
    FileEntry entry { .id = -1 };

     parseKeyValueLine(line, [&](string_view field, string_view value) {

         if (field == "id") { entry.id = parseNum(value); return; }
         if (field == "name") { entry.name = string(value); return; }
         if (field == "size") { entry.size = parseNum(value); return; }
         if (field == "mtime") { entry.mtime = parseU64(value); return; }
         if (field == "mod") { entry.mod = parseNum(value); return; }

         printf("Unsupported FileEntry field: %.*s\n", (int)field.size(), field.data());
     });

    if (entry.id >= 0) map[entry.id] = entry;
}

optional<FileEntry>
FileMap::seek(const string &name) const {

    for (const auto &[id, entry] : map) {
        if (entry.name == name) return entry;
    }
    return std::nullopt;
}

void
LineMap::parse(string_view line)
{
    LineEntry entry { .id = -1 };

    parseKeyValueLine(line, [&](string_view field, string_view value) {

        if (field == "id")   { entry.id = parseNum(value); return; }
        if (field == "file") { entry.file = parseNum(value); return; }
        if (field == "line") { entry.line = parseNum(value); return; }
        if (field == "span") { entry.span = parseNum(value); return; }

        printf("Unsupported LineEntry field: %.*s\n", (int)field.size(), field.data());
    });

    if (entry.id >= 0) map[entry.id] = entry;
}

optional<LineEntry>
LineMap::seek(isize line, isize file)
{
    for (const auto &[id, entry] : map) {
        if (entry.line == line && entry.file == file) return entry;
    }
    return std::nullopt;
}

void
SegmentMap::parse(string_view line)
{
    SegmentEntry entry { .id = -1 };

    parseKeyValueLine(line, [&](string_view field, string_view value) {

        if (field == "id")       { entry.id = parseNum(value); return; }
        if (field == "name")     { entry.name = string(value); return; }
        if (field == "start")    { entry.start = parseU32(value); return; }
        if (field == "size")     { entry.size = parseU32(value); return; }
        if (field == "addrsize") { entry.addrsize = string(value); return; }
        if (field == "type")     { entry.type = string(value); return; }
        if (field == "oname")    { entry.oname = string(value); return; }
        if (field == "ooffs")    { entry.ooffs = parseU32(value); return; }

        printf("Unsupported SegmentEntry field: %.*s\n", (int)field.size(), field.data());
    });

    if (entry.id >= 0) map[entry.id] = entry;
}

void
SpanMap::parse(string_view line)
{
    SpanEntry entry { .id = -1 };

    parseKeyValueLine(line, [&](string_view field, string_view value) {

        if (field == "id")    { entry.id = parseNum(value); return; }
        if (field == "seg")   { entry.seg = parseNum(value); return; }
        if (field == "start") { entry.start = parseNum(value); return; }
        if (field == "size")  { entry.size = parseNum(value); return; }

        printf("Unsupported SpanEntry field: %.*s\n", (int)field.size(), field.data());
    });

    if (entry.id >= 0) map[entry.id] = entry;
}

void
SymbolMap::parse(string_view line)
{
    SymbolEntry entry { .id = -1 };

    parseKeyValueLine(line, [&](string_view field, string_view value) {

        if (field == "id")       { entry.id = parseNum(value); return; }
        if (field == "name")     { entry.name = string(value); return; }
        if (field == "addrsize") { entry.addrsize = string(value); return; }
        if (field == "size")     { entry.size = parseNum(value); return; }
        if (field == "scope")    { entry.scope = parseNum(value); return; }
        if (field == "val")      { entry.val = parseU16(value); return; }
        if (field == "seg")      { entry.seg = parseNum(value); return; }
        if (field == "type")     { entry.type = string(value); return; }

        printf("Unsupported SymbolEntry field: %.*s\n", (int)field.size(), field.data());
    });

    if (entry.id >= 0) {

        map[entry.id] = entry;
        valToId[entry.val] = entry.id;
        nameToId[entry.name] = entry.id;
    }
}

void
SymbolMap::dump()
{
    for (auto &sym: map) {
        printf("%ld: %s %d\n", sym.first, sym.second.name.c_str(), sym.second.val);
    }
}

optional<SymbolEntry>
SymbolMap::seek(u16 addr) const {

    if (auto it = valToId.find(addr); it != valToId.end()) {
        if (auto mit = map.find(it->second); mit != map.end()) {
            return mit->second;
        }
    }
    return {};
}

optional<SymbolEntry>
SymbolMap::seek(const string &label) const {

    if (auto it = nameToId.find(label); it != nameToId.end()) {
        if (auto mit = map.find(it->second); mit != map.end()) {
            return mit->second;
        }
    }
    return {};
}

void
SymbolTable::clear()
{
    files.clear();
    lines.clear();
    segments.clear();
    spans.clear();
    symbols.clear();
}

bool
SymbolTable::loadCS65File(const fs::path &path)
{
    clear();

    std::ifstream in(path);
    if (!in.is_open()) return false;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        parseLine(line);
    }

    return true;
}

void
SymbolTable::parseLine(string_view line)
{
    // Skip leading whitespace
    line.remove_prefix(std::min(line.find_first_not_of(" \t"), line.size()));

    // Split key and rest
    auto space = line.find_first_of(" \t");
    string_view category = line.substr(0, space);
    string_view rest = (space == string_view::npos) ? "" : line.substr(space + 1);

    // Dispatch
    if (category == "file") { files.parse(rest); return; }
    if (category == "line") { lines.parse(rest); return; }
    if (category == "seg")  { segments.parse(rest); return; }
    if (category == "span") { spans.parse(rest); return; }
    if (category == "sym")  { symbols.parse(rest); return; }

    printf("Unknown category: %.*s\n", (int)category.size(), category.data());
}

optional<u16>
SymbolTable::resolveAddr(const string &file, isize line)
{
    if (auto fileEntry = files.seek(file); fileEntry) {
        if (auto lineEntry = lines.seek(line, fileEntry->id); lineEntry) {
            if (auto spanEntry = spans[lineEntry->span]; spanEntry) {
                return (u16)spanEntry->start;
            }
        }
    }
    return {};
}

}
