// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#include "PeddleConfig.h"
#include "SymbolTable.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace vc64::peddle {

void
SymbolTable::clear()
{
    files.clear();
    addrSymbols.clear();
    nameSymbols.clear();
    lines.clear();
}

bool
SymbolTable::loadCS65File(const std::string& path)
{
    clear();

    // Open file
    std::ifstream in(path);
    if (!in.is_open()) return false;

    // Read line by line
    std::string line;
    while (std::getline(in, line)) {

        if (line.empty() || line[0] == '#') continue;
        parseLine(line);
    }

    // Sort line entries by address for binary search
    std::sort(lines.begin(), lines.end(), [](const LineEntry& a, const LineEntry& b) {
        return a.lineNumber < b.lineNumber;
    });

    return true;
}

void SymbolTable::parseLine(const std::string& line)
{
    std::istringstream iss(line);
    std::string key;
    iss >> key;

    if (key == "file") {

        FileEntry entry;
        std::string token;
        while (std::getline(iss, token, ',')) {
            auto pos = token.find('=');
            if (pos == std::string::npos)
                continue;

            std::string field = token.substr(0, pos);
            std::string value = token.substr(pos + 1);

            if (field == "id") entry.id = std::stoi(value);
            else if (field == "name") {
                if (value.size() > 2 && value.front() == '"' && value.back() == '"')
                    value = value.substr(1, value.size() - 2);
                entry.name = value;
            } else if (field == "size") entry.size = std::stoul(value);
            else if (field == "mtime") entry.mtime = std::stoul(value, nullptr, 0);
            else if (field == "mod") entry.mod = std::stoi(value);
        }
        if (entry.id >= 0)
            files[entry.id] = entry;
    }
    else if (key == "sym") {

        SymbolEntry sym;
        std::string token;
        while (std::getline(iss, token, ',')) {
            auto pos = token.find('=');
            if (pos == std::string::npos)
                continue;

            std::string field = token.substr(0, pos);
            std::string value = token.substr(pos + 1);

            if (field == "addr") sym.address = (u16)std::stoul(value, nullptr, 0);
            else if (field == "name") {
                if (value.size() > 2 && value.front() == '"' && value.back() == '"')
                    value = value.substr(1, value.size() - 2);
                sym.name = value;
            } else if (field == "type") sym.type = std::stoi(value);
            else if (field == "scope") sym.scope = std::stoi(value);
            else if (field == "seg") sym.seg = std::stoi(value);
        }
        if (!sym.name.empty()) {
            addrSymbols[sym.address] = sym;
            nameSymbols[sym.name] = sym;
        }
    }
    else if (key == "line") {

        LineEntry le;
        std::string token;
        while (std::getline(iss, token, ',')) {
            auto pos = token.find('=');
            if (pos == std::string::npos)
                continue;

            std::string field = token.substr(0, pos);
            std::string value = token.substr(pos + 1);

            if (field == "file") le.fileId = std::stoi(value);
            else if (field == "line") le.lineNumber = std::stoi(value);
            else if (field == "seg") le.seg = std::stoi(value);
        }
        lines.push_back(le);
    }
}

optional<FileEntry>
SymbolTable::getFileById(isize id) const
{
    if (auto it = files.find(id); it != files.end())
        return it->second;
    return std::nullopt;
}

optional<SymbolEntry>
SymbolTable::findSymbolByAddress(uint16_t addr) const
{
    if (auto it = addrSymbols.find(addr); it != addrSymbols.end())
        return it->second;
    return std::nullopt;
}

optional<SymbolEntry>
SymbolTable::findSymbolByName(const std::string& name) const
{
    if (auto it = nameSymbols.find(name); it != nameSymbols.end())
        return it->second;
    return std::nullopt;
}

optional<LineEntry>
SymbolTable::findLineByNr(isize nr) const
{
    if (lines.empty()) return std::nullopt;

    auto it = std::lower_bound(lines.begin(), lines.end(), nr,
                               [](const LineEntry& entry, uint16_t value) {
        return entry.lineNumber < value;
    });

    if (it != lines.begin()) --it;  // find the line covering the address
    return *it;
}

}
