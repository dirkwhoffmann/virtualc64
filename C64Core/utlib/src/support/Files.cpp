// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/common.h"
#include "utl/io.h"
#include <fstream>
#include <cassert>
#include <sys/stat.h>

namespace utl {

fs::path
makeUniquePath(const fs::path &path)
{
    auto location = path.root_path() / path.relative_path();
    auto name = path.stem().string();
    auto extension = path.extension();

    for (isize nr = 2;; nr++) {

        auto index = std::to_string(nr);
        fs::path result = location / fs::path(name + index) / extension;

        if (!fileExists(result)) return result;
    }

    unreachable;
}

isize
getSizeOfFile(const fs::path &path)
{
    struct stat fileProperties;

    if (stat(path.string().c_str(), &fileProperties) != 0)
        return -1;

    return (isize)fileProperties.st_size;
}

bool
fileExists(const fs::path &path)
{
    return getSizeOfFile(path) >= 0;
}

bool
isDirectory(const fs::path &path)
{
    try {

        const auto &entry = fs::directory_entry(path);
        return entry.is_directory();

    } catch (...) {

        return false;
    }
}

bool
createDirectory(const fs::path &path)
{
    try {

        return fs::create_directory(path);

    } catch (...) {

        return false;
    }
}

isize
numDirectoryItems(const fs::path &path)
{
    isize result = 0;

    try {

        for (const auto &entry : fs::directory_iterator(path)) {

            const auto &name = entry.path().filename().string();
            if (name[0] != '.') result++;
        }

    } catch (...) { }

    return result;
}

std::vector<fs::path>
files(const fs::path &path, const string &suffix)
{
    std::vector <string> suffixes;
    if (suffix != "") suffixes.push_back(suffix);

    return files(path, suffixes);
}

std::vector<fs::path>
files(const fs::path &path, std::vector <string> &suffixes)
{
    std::vector<fs::path> result;

    try {

        for (const auto &entry : fs::directory_iterator(path)) {

            const auto &name = entry.path().filename();
            auto suffix = name.extension().string();

            if (std::find(suffixes.begin(), suffixes.end(), suffix) != suffixes.end()) {
                result.push_back(name);
            }
        }

    } catch (...) { }

    return result;
}

bool
matchingFileHeader(const fs::path &path, const u8 *header, isize len, isize offset)
{
    std::ifstream file(path, std::ios::binary);
    return file.is_open() ? matchingStreamHeader(file, header, len, offset) : false;
}

bool
matchingFileHeader(const fs::path &path, const string &header, isize offset)
{
    std::ifstream file(path, std::ios::binary);
    return file.is_open() ? matchingStreamHeader(file, header, offset) : false;
}

bool
matchingStreamHeader(std::istream &is, const u8 *header, isize len, isize offset)
{
    assert(header != nullptr);

    is.seekg(offset, std::ios::beg);

    for (isize i = 0; i < len; i++) {

        if (is.get() != (int)header[i]) {
            is.seekg(0, std::ios::beg);
            return false;
        }
    }
    is.seekg(0, std::ios::beg);
    return true;
}

bool
matchingStreamHeader(std::istream &is, const string &header, isize offset)
{
    return matchingStreamHeader(is, (u8 *)header.c_str(), (isize)header.length(), offset);
}

bool
matchingBufferHeader(const u8 *buf, const u8 *header, isize len, isize offset)
{
    assert(buf != nullptr);
    assert(header != nullptr);

    for (isize i = 0; i < len; i++) {
        if (buf[offset + i] != header[i])
            return false;
    }
    return true;
}

bool
matchingBufferHeader(const u8 *buf, isize blen, const string &header, isize offset)
{
    assert(buf != nullptr);

    isize len = isize(header.length());
    return len + offset <= blen && std::memcmp(buf + offset, (u8 *)header.c_str(), len) == 0;
}

bool
matchingBufferHeader(const u8 *buf, const string &header, isize offset)
{
    auto blen = std::numeric_limits<isize>::max();
    return matchingBufferHeader(buf, blen, header, offset);
}

}
