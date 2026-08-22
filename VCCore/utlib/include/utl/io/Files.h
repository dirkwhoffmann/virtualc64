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
// Handling files and directories
//

// Makes a file name unique if a file with the provided name already exists
fs::path makeUniquePath(const fs::path &path);

// Adds the provided extension or replaces the existing one if it does not match
fs::path ensureExtension(const fs::path &path, const string &extension);

// Returns the size of a file in bytes
isize getSizeOfFile(const fs::path &path);

// Checks if a file exists
bool fileExists(const fs::path &path);

// Checks if a path points to a directory
bool isDirectory(const fs::path &path);

// Creates a directory
bool createDirectory(const fs::path &path);

// Removes a file or directory
void remove(const fs::path &path);

// Returns the number of files in a directory
isize numDirectoryItems(const fs::path &path);

// Returns a list of files in a directory (absolute filenames)
std::vector<fs::path> files(const fs::path &path, bool rec = true, const std::vector<string> &suffixes = {});

// Returns a list of files in a directory (relative filenames)
std::vector<fs::path> filesRel(const fs::path &path, bool rec = true, const std::vector<string> &suffixes = {});

// Core routine for the family of file functions
std::vector<fs::path> files(const fs::path &path, bool rec, const fs::path &root, const vector<string> &suffixes);

//
// Analyzing file headers
//

// Checks the header signature (magic bytes) of a file, stream, or buffer
bool matchingFileHeader(const fs::path &path, const u8 *header, isize len, isize offset = 0);
bool matchingFileHeader(const fs::path &path, const string &header, isize offset = 0);
bool matchingStreamHeader(std::istream &is, const u8 *header, isize len, isize offset = 0);
bool matchingStreamHeader(std::istream &is, const string &header, isize offset = 0);
bool matchingBufferHeader(const u8 *buf, const u8 *header, isize len, isize offset = 0);
bool matchingBufferHeader(const u8 *buf, const string &header, isize offset = 0);
bool matchingBufferHeader(const u8 *buf, isize blen, const string &header, isize offset = 0);


//
// Syncing directories
//

// Copies new and updated items from source to destination. Returns the number of copied/updated files
isize syncDirectory(const fs::path &source, const fs::path &destination);

// Identifies all items in destination that do not exist in source
std::vector<fs::path> getPruneList(const fs::path &source, const fs::path &destination);

// Removes all items identified by getPruneList(). Returns the number of deleted items
isize pruneDirectory(const fs::path &source, const fs::path &destination);

// Full rsync-like operation
isize mirrorDirectory(const fs::path &source, const fs::path &destination);

}
