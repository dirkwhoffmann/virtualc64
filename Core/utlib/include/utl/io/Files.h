// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Reflectable.h"
#include "utl/io/IOError.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace utl {

//
// Handling files
//

// Makes a file name unique if a file with the provided name already exists
fs::path makeUniquePath(const fs::path &path);

// Returns the size of a file in bytes
isize getSizeOfFile(const fs::path &path);

// Checks if a file exists
bool fileExists(const fs::path &path);

// Checks if a path points to a directory
bool isDirectory(const fs::path &path);

// Creates a directory
bool createDirectory(const fs::path &path);

// Returns the number of files in a directory
isize numDirectoryItems(const fs::path &path);

// Returns a list of files in a directory
std::vector<fs::path> files(const fs::path &path, const string &suffix = "");
std::vector<fs::path> files(const fs::path &path, std::vector <string> &suffixes);

// Checks the header signature (magic bytes) of a file, stream, or buffer
bool matchingFileHeader(const fs::path &path, const u8 *header, isize len, isize offset = 0);
bool matchingFileHeader(const fs::path &path, const string &header, isize offset = 0);
bool matchingStreamHeader(std::istream &is, const u8 *header, isize len, isize offset = 0);
bool matchingStreamHeader(std::istream &is, const string &header, isize offset = 0);
bool matchingBufferHeader(const u8 *buf, const u8 *header, isize len, isize offset = 0);
bool matchingBufferHeader(const u8 *buf, const string &header, isize offset = 0);
bool matchingBufferHeader(const u8 *buf, isize blen, const string &header, isize offset = 0);

}
