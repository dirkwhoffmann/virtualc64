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

#ifdef __APPLE__
#include <sys/xattr.h>
#endif

namespace utl {

fs::path
makeUniquePath(const fs::path &path)
{
    if (!fileExists(path)) return path;

    auto location  = path.parent_path();
    auto name      = path.stem().string();
    auto extension = path.extension().string();

    for (isize nr = 2;; nr++) {

        auto index      = std::to_string(nr);
        fs::path result = location / (name + index + extension);
        if (!fileExists(result)) return result;
    }

    unreachable;
}

fs::path
ensureExtension(const fs::path &path, const string &extension)
{
    string ext = extension;
    fs::path result = path;

    // Add a dot if not provided
    if (!ext.empty() && ext[0] != '.') ext.insert(ext.begin(), '.');

    // Replace the extension
    if (result.extension() != ext) result.replace_extension(ext);

    return result;
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

bool
createEmptyFile(const fs::path &path, isize size)
{
    if (size < 0) return false;

    try {

        std::ofstream os(path, std::ios::binary | std::ios::trunc);
        if (!os) return false;

        if (size > 0) {

            // Note: Only the last byte is written. Seeking past the end of a
            // new file and writing there leaves a hole, which reads back as
            // zeros. This allows the OS to create the file as a sparse file
            // if such files are supported.

            os.seekp(std::streamoff(size - 1));
            os.put('\0');
        }

        os.close();
        return bool(os);

    } catch (...) {

        return false;
    }
}

void
remove(const fs::path &path)
{
    std::error_code ec;

    if (fs::exists(path)) {

        fs::remove_all(path, ec);

        if (ec) {
            throw fs::is_directory(path) ?
            IOError(IOError::DIR_CANT_DELETE, path) :
                  IOError(IOError::FILE_CANT_DELETE, path);
        }
    }
}

/* Marks a directory as a package by hand.
 *
 * Finder shows a directory as a single file in one of two cases: its type
 * conforms to com.apple.package, which needs an installed app declaring that
 * type, or the directory itself carries the kHasBundle flag. Only the flag
 * travels with the folder, so it is what makes a directory look right after
 * being copied to a machine that has never run the app that owns it.
 *
 * The flag lives in the 'com.apple.FinderInfo' extended attribute -- a fixed
 * 32-byte block the kernel refuses at any other size. Its first half is a
 * FileInfo for files and a FolderInfo for directories; the two layouts agree
 * on almost nothing, but both place the 16-bit big-endian finderFlags at
 * offset 8, which is the only field touched here. Whatever else is in there
 * is read back and preserved: overwriting it would throw away the folder's
 * saved window position and its colour tag.
 *
 * Best-effort on purpose. A read-only volume, or a file system with no xattr
 * support, costs an icon -- not a reason to fail whatever we were doing.
 */
bool
setPackageBit(const fs::path &path) noexcept
{
#ifdef __APPLE__

    constexpr isize size = 32;      // The only length the kernel accepts
    constexpr isize flags = 8;      // Offset of finderFlags, in both layouts
    constexpr u8 hasBundle = 0x20;  // High byte of kHasBundle (0x2000)

    u8 info[size] = { };

    /* Read before writing. A missing attribute is not a failure -- it means
     * the folder has no Finder info yet, which is what the zeroed buffer
     * already describes.
     */
    const auto len = ::getxattr(path.c_str(), XATTR_FINDERINFO_NAME, info, size, 0, 0);
    if (len != size && len != -1) return false;

    // Nothing to do if the folder is already marked
    if (info[flags] & hasBundle) return true;

    info[flags] |= hasBundle;

    return ::setxattr(path.c_str(), XATTR_FINDERINFO_NAME, info, size, 0, 0) == 0;

#else

    (void)path;
    return false;

#endif
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

/*
std::vector<fs::path>
files(const fs::path &path, const string &suffix)
{
    std::vector <string> suffixes;
    if (suffix != "") suffixes.push_back(suffix);

    return files(path, suffixes);
}
*/

std::vector<fs::path>
files(const fs::path &path, bool rec, const std::vector <string> &suffixes)
{
    return files(path, rec, {}, suffixes);
}

std::vector<fs::path>
filesRel(const fs::path &path, bool rec, const std::vector <string> &suffixes)
{
    return files(path, rec, path, suffixes);
}

std::vector<fs::path>
files(const fs::path &path, bool rec, const fs::path &root, const vector<string> &suffixes)
{
    std::vector<fs::path> result;

    try {

        auto process_entry = [&](const auto &entry) {

            auto ext = entry.path().extension().string();
            if (suffixes.empty() || std::find(suffixes.begin(), suffixes.end(), ext) != suffixes.end()) {

                auto item = root.empty() ? entry.path() : fs::relative(entry.path(), root);
                result.push_back(item);
            }
        };

        if (rec) {

            for (const auto &entry : fs::recursive_directory_iterator(path))
                process_entry(entry);

        } else {

            for (const auto &entry : fs::directory_iterator(path))
                process_entry(entry);
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

isize
syncDirectory(const fs::path &source, const fs::path &destination)
{
    isize count = 0;

    auto needsUpdate = [](const fs::path &src, const fs::path &dst) -> bool {

      return
          !fs::exists(dst) ||
          fs::file_size(src) != fs::file_size(dst) ||
          fs::last_write_time(src) > fs::last_write_time(dst);
    };

    if (!fs::exists(source) || !fs::is_directory(source))
        throw IOError(IOError::DIR_NOT_FOUND, source);

    fs::create_directories(destination);
    for (const auto &entry : fs::recursive_directory_iterator(source)) {

        auto rel = fs::relative(entry.path(), source);
        auto dst = destination / rel;

        if (entry.is_directory()) {

            fs::create_directories(dst);

        } else if (entry.is_regular_file()) {

            fs::create_directories(dst.parent_path());

            if (needsUpdate(entry.path(), dst)) {

                fs::copy_file(entry.path(), dst, fs::copy_options::overwrite_existing);
                count++;
            }
        }
    }

    return count;
}

std::vector<fs::path>
getPruneList(const fs::path &source, const fs::path &destination)
{
    if (!fs::exists(destination))
        throw IOError(IOError::DIR_NOT_FOUND, destination);

    std::vector<fs::path> result;

    // Locate files that are present in the destination, but not in the source
    for (const auto &entry : fs::recursive_directory_iterator(destination)) {

        auto rel = fs::relative(entry.path(), destination);
        auto src = source / rel;

        if (!fs::exists(src)) result.push_back(entry.path());
    }

    // Sort item by depth, ensuring that a later deletion will process deeper entries first
    std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) {
        return a.native().size() > b.native().size();
    });

    return result;
}

isize
pruneDirectory(const fs::path &source, const fs::path &destination)
{
    if (!fs::exists(destination))
        throw IOError(IOError::DIR_NOT_FOUND, destination);

    isize result = 0;

    for (const auto &path : getPruneList(source, destination)) {

        std::error_code ec;
        if (fs::remove(path, ec)) result++;
    }

    return result;
}

isize
mirrorDirectory(const fs::path &source, const fs::path &destination)
{
    return syncDirectory(source, destination) + pruneDirectory(source, destination);
}

}
