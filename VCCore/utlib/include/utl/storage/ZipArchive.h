// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/storage/Buffer.h"
#include <regex>

struct zip_t;

namespace utl {

class ZipArchive {

    // Path to the Zip archive
    fs::path path;

    // KubaZip handle
    zip_t *zip = nullptr;

public:

    ZipArchive(const fs::path &path, char access = 'r');
    ZipArchive(ZipArchive &&other) noexcept;
    ZipArchive &operator=(ZipArchive &&other) noexcept;
    ZipArchive(const ZipArchive &) = delete;
    ZipArchive &operator=(const ZipArchive &) = delete;
    ~ZipArchive() noexcept;

    void close() noexcept;
    void swap(ZipArchive &other) noexcept;

    // Returns the number of items in the archive
    isize size() const;

    // Returns a list of all files in the archive
    vector<string> listFiles() const;

    // Extracts a file from the archive
    std::vector<u8> uncompress(const string &fileName);

    // Read files from the archive
    void uncompress(const string &fileName, const fs::path &targetDir);
    void uncompressAll(const fs::path &targetDir);

    // Write files into the archive with absolute paths
    // void write(const fs::path &file);
    // void write(const std::vector<fs::path> &files);

    // Write files into the archive with relative paths to root
    void write(const fs::path &file, const fs::path &root);
    void write(const std::vector<fs::path> &files, const fs::path &root);

    // Replaced the archive with the provided files
    void replace(const std::vector<fs::path> &files, const fs::path &root);
};

}