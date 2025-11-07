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

struct SymbolEntry {

    isize id = -1;
    string name;
    u16 address = 0;
    isize scope = -1;
    isize type = -1;
    isize seg = -1;
};

class SymbolTable {

    unordered_map<isize, FileEntry> files;
    unordered_map<u16, SymbolEntry> addrSymbols;
    unordered_map<std::string, SymbolEntry> nameSymbols;
    vector<LineEntry> lines;

public:

    void clear();
    bool loadCS65File(const std::string& path);

    optional<FileEntry> getFileById(isize id) const;
    optional<SymbolEntry> findSymbolByAddress(u16 addr) const;
    optional<SymbolEntry> findSymbolByName(const string &name) const;
    optional<LineEntry> findLineByNr(isize nr) const;

private:

    void parseLine(const std::string& line);

};

}
