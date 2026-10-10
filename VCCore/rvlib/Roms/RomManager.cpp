// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "Roms/RomManager.h"
#include "utl/storage/Buffer.h"
#include <algorithm>

namespace retro::vault {


string
RomDateToString(RomDate date)
{
    static const char *names[] = {
        "", "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    if (date.year == 0) return "";
    if (date.month == 0 || date.month > 12) return std::to_string(date.year);

    return string(names[date.month]) + " " + std::to_string(date.year);
}

RomManager::RomManager()
{
    registerC64Roms();
    registerAmigaRoms();

    /* Index the database by both checksums. An entry carries exactly one of
     * them -- the other is zero -- so skipping zero keys keeps the two indices
     * disjoint. Without that guard, looking up an empty Rom slot (checksum 0)
     * would match the entries of the other machine.
     */
    for (const auto &traits : database) {

        if (traits.crc) crc32Info[traits.crc] = traits;
        if (traits.fnv) fnv64Info[traits.fnv] = traits;
    }
}

RomManager &
RomManager::shared()
{
    static RomManager instance;
    return instance;
}


//
// Querying the database
//

std::optional<RomTraits>
RomManager::resolveCRC32(u32 crc) const
{
    if (auto it = crc32Info.find(crc); it != crc32Info.end()) return it->second;
    return std::nullopt;
}

std::optional<RomTraits>
RomManager::resolveFNV64(u64 fnv) const
{
    if (auto it = fnv64Info.find(fnv); it != fnv64Info.end()) return it->second;
    return std::nullopt;
}

std::vector<RomTraits>
RomManager::getRoms(const std::function<bool(const RomTraits &)> &filter) const
{
    std::vector<RomTraits> result;

    for (const auto &traits : database) {
        if (filter(traits)) result.push_back(traits);
    }

    return result;
}

std::optional<RomTraits>
RomManager::resolve(const fs::path &path) const
{
    try {

        utl::Buffer<u8> buffer(path);
        if (buffer.size == 0) return std::nullopt;

        if (auto traits = resolveCRC32(buffer.crc32())) return traits;
        return resolveFNV64(buffer.fnv64());

    } catch (...) {

        // Unreadable files are simply not Roms as far as this class cares
        return std::nullopt;
    }
}


//
// Locating real Rom files
//

std::optional<fs::path>
RomManager::getRom(const RomTraits &traits) const
{
    if (auto path = getRomCRC32(traits.crc)) return path;
    return getRomFNV64(traits.fnv);
}

std::optional<fs::path>
RomManager::getRomCRC32(u32 crc) const
{
    if (auto it = crc32Path.find(crc); it != crc32Path.end()) return it->second;
    return std::nullopt;
}

std::optional<fs::path>
RomManager::getRomFNV64(u64 fnv) const
{
    if (auto it = fnv64Path.find(fnv); it != fnv64Path.end()) return it->second;
    return std::nullopt;
}


//
// Managing the folder list
//

void
RomManager::clearFolderList()
{
    folders.clear();
}

void
RomManager::addFolder(const fs::path &path)
{
    /* Normalize first, so that the duplicate check is not defeated by a
     * trailing separator or a path spelled relative to the working directory.
     */
    std::error_code ec;
    auto folder = fs::weakly_canonical(path, ec);
    if (ec) folder = path;

    if (std::find(folders.begin(), folders.end(), folder) == folders.end()) {
        folders.push_back(folder);
    }
}

void
RomManager::clearScanResults()
{
    crc32Path.clear();
    fnv64Path.clear();
}

isize
RomManager::scanFolders()
{
    // Roms are small; anything larger is not worth hashing
    constexpr auto maxRomSize = 8 * 1024 * 1024;

    clearScanResults();

    for (const auto &folder : folders) {

        std::error_code ec;
        auto opts = fs::directory_options::skip_permission_denied;
        fs::recursive_directory_iterator it(folder, opts, ec), end;

        // A folder that has gone away is skipped, not an error
        for (; !ec && it != end; it.increment(ec)) {

            /* Errors below use a separate code, so that a single unreadable
             * entry cannot be mistaken for a failure of the walk itself.
             */
            std::error_code fec;

            if (!it->is_regular_file(fec) || fec) continue;

            auto size = it->file_size(fec);
            if (fec || size == 0 || size > maxRomSize) continue;

            u32 crc;
            u64 fnv;

            try {

                utl::Buffer<u8> buffer(it->path());
                crc = buffer.crc32();
                fnv = buffer.fnv64();

            } catch (...) { continue; }

            /* emplace() keeps the entry already present, so the first file
             * found for a checksum wins and the folder list acts as a
             * precedence order.
             */
            if (crc32Info.contains(crc)) crc32Path.emplace(crc, it->path());
            if (fnv64Info.contains(fnv)) fnv64Path.emplace(fnv, it->path());
        }
    }

    return isize(crc32Path.size() + fnv64Path.size());
}

}
