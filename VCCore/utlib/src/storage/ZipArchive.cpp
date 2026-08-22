// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

// #include "utl/common.h"
#include "utl/storage/ZipArchive.h"
#include "utl/io/IOError.h"
#include "utl/storage/ZipError.h"
#include <fstream>
#include <sstream>
#include <zip.h>

namespace utl {

struct ZipEntry {

    zip_t *zip = nullptr;

    ZipEntry(zip_t *zip, const string &fileName) : zip(zip)
    {
        if (auto ec = zip_entry_open(zip, fileName.c_str()); ec)
            throw ZipError(ZipError::KUBA_ZIP_ERROR, ec);
    }

    ~ZipEntry()
    {
        if (zip) zip_entry_close(zip);
    }

    ZipEntry(const ZipEntry&) = delete;
    ZipEntry& operator=(const ZipEntry&) = delete;
    ZipEntry(ZipEntry&&) = delete;
    ZipEntry& operator=(ZipEntry&&) = delete;

};

ZipArchive::ZipArchive(const fs::path &path, char access) : path(path)
{
    if (access == 'r' && !fs::exists(path)) {
        throw IOError(IOError::FILE_NOT_FOUND, path);
    }

    switch (access) {

        case 'r': zip = zip_open(path.string().c_str(), 0, 'r'); break;
        case 'w': zip = zip_open(path.string().c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w'); break;
        case 'a': zip = zip_open(path.string().c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'a'); break;
        default:  throw ZipError(ZipError::INVALID_ZIP_MODE, "'" + string(1, access) + "'");
    }

    if (zip == nullptr) {
        throw ZipError(ZipError::INVALID_ARCHIVE, path);
    }
}

ZipArchive::ZipArchive(ZipArchive &&other) noexcept
{
    path = other.path;
    zip = other.zip;
    other.path = fs::path {};
    other.zip = nullptr;
}

ZipArchive &
ZipArchive::operator=(ZipArchive &&other) noexcept
{
    if (this != &other) {

        ZipArchive temporary(std::move(other));
        this->swap(temporary);
    }
    return *this;
}

ZipArchive::~ZipArchive() noexcept
{
    close();
}

void
ZipArchive::close() noexcept
{
    if (zip) {

        zip_close(zip);
        zip = nullptr;
    }
}

void
ZipArchive::swap(ZipArchive &other) noexcept
{
    std::swap(path, other.path);
    std::swap(zip, other.zip);
}

isize
ZipArchive::size() const
{
    return zip ? isize(zip_entries_total(zip)) : 0;
}

vector<string>
ZipArchive::listFiles() const
{
    assert(zip);
    vector<string> result;

    for (int i = 0, n = (int)size(); i < n; ++i) {

        auto ec = zip_entry_openbyindex(zip, i);
        if (ec != 0) throw ZipError(ZipError::KUBA_ZIP_ERROR, ec);

        const char *name = zip_entry_name(zip);
        if (name) result.emplace_back(name);
        zip_entry_close(zip);
    }

    return result;
}

std::vector<u8>
ZipArchive::uncompress(const string &fileName)
{
    assert(zip);
    ZipEntry open(zip, fileName);

    void *buf = nullptr;
    size_t bufsize;

    if (auto ec = zip_entry_read(zip, &buf, &bufsize); ec < 0) {

        free(buf);
        throw ZipError(ZipError::KUBA_ZIP_ERROR, ec);
    }

    std::vector<u8> result((u8 *)buf, (u8 *)buf + bufsize);
    free(buf);
    return result;
}

void
ZipArchive::uncompress(const string &fileName, const fs::path &targetDir)
{
    if (!fs::is_directory(targetDir)) {
        throw IOError(ZipError::NOT_A_DIRECTORY, targetDir);
    }

    // Assemble target file name
    auto targetFile = targetDir / fs::path(fileName);

    // Create all parent directories (if needed)
    fs::create_directories(targetFile.parent_path());

    // Unzip file
    ZipEntry open(zip, fileName);
    if (auto ec = zip_entry_fread(zip, targetFile.string().c_str()); ec < 0) {
        throw ZipError(ZipError::KUBA_ZIP_ERROR, ec);
    }
}

void
ZipArchive::uncompressAll(const fs::path &targetDir)
{
    for (auto &fileName : listFiles()) {
        uncompress(fileName, targetDir);
    }
}

/*
void
ZipArchive::write(const fs::path &file)
{
    writeRelative( {file}, {} );
}

void
ZipArchive::write(const std::vector<fs::path> &files)
{
    writeRelative (files, {});
}
*/

void
ZipArchive::write(const fs::path &file, const fs::path &root)
{
    const std::vector<fs::path> files = { file };
    write(files, root);
}

void
ZipArchive::write(const std::vector<fs::path> &files, const fs::path &root)
{
    for (auto &item : files) {

        auto rel = fs::relative(item, root);

        if (rel.empty() || *rel.begin() == "..")
            throw IOError(IOError::FILE_NOT_FOUND, item);

        ZipEntry open(zip, rel.generic_string().c_str());

        if (auto ec = zip_entry_fwrite(zip, item.string().c_str()); ec < 0)
            throw ZipError(ZipError::KUBA_ZIP_ERROR, ec);
    }
}

void
ZipArchive::replace(const std::vector<fs::path> &files, const fs::path &root)
{
    // Create a new archive
    ZipArchive temporary(path.concat(".tmp"), 'w');

    // Add all files
    temporary.write(files, root);

    // Close both archives
    temporary.close();
    close();

    // Replace the old archive with the new one on disk
    fs::rename(temporary.path, path);

    // Reopen the archive
    if (zip = zip_open(path.string().c_str(), 0, 'r'); zip == nullptr)
        throw ZipError(ZipError::INVALID_ARCHIVE, path);
}

}