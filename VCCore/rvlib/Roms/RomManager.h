// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Roms/RomTypes.h"
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

namespace retro::vault {

/* The Rom database. Knows every Rom RetroVault can put a name to, across all
 * machines -- the C64 and Amiga tables used to live in a RomDatabase.h of their
 * own inside VCCore and VACore respectively.
 *
 * Roms are identified by hash, and which hash depends on the machine: C64 Roms
 * carry an FNV-1a, Amiga Roms a CRC32, and each entry leaves the other at zero.
 * The two indices below are therefore disjoint -- every entry appears in
 * exactly one of them -- which is also why the lookups are named rather than
 * overloaded on the key type.
 *
 * Knowing *of* a Rom is separate from having it: the database says what a
 * checksum means, and a scan of the folder list says where the matching file
 * lives on disk. Call addFolder() and scanFolders() to establish the latter,
 * then getRom() to go from a database entry to a real file.
 */
class RomManager {

    // The Rom database, in registration order
    std::vector<RomTraits> database;

    // The same database, indexed by both checksums
    std::unordered_map<u32, RomTraits> crc32Info;
    std::unordered_map<u64, RomTraits> fnv64Info;

    // Folders searched for real Rom files
    std::vector<fs::path> folders;

    // Result of the most recent folder scan
    std::unordered_map<u32, fs::path> crc32Path;
    std::unordered_map<u64, fs::path> fnv64Path;

    // Appends the known Roms to 'database' -- implemented in RomsC64.cpp and
    // RomsAmiga.cpp respectively, to keep this class out of the huge literal
    // tables
    void registerC64Roms();
    void registerAmigaRoms();

public:

    RomManager();

    /* Provides access to the shared instance. The cores reach the database
     * from static context, and the folder list is a per-application setting,
     * so both work off this one.
     */
    static RomManager &shared();


    //
    // Querying the database
    //

    // Looks up a Rom by checksum
    std::optional<RomTraits> resolveCRC32(u32 crc) const;
    std::optional<RomTraits> resolveFNV64(u64 fnv) const;

    /* Looks up a Rom by file. The file is read and both checksums are computed,
     * so this recognizes a Rom the folder scan never saw. Returns nothing if
     * the file is unreadable or matches no database entry.
     */
    std::optional<RomTraits> resolve(const fs::path &path) const;

    // Returns every database entry the filter accepts, e.g.:
    // getRoms([](const RomTraits &t) { return t.vendor == RomVendor::AROS; })
    std::vector<RomTraits> getRoms(const std::function<bool(const RomTraits &)> &filter) const;

    // Indicates whether a checksum belongs to a known Rom
    bool isKnownCRC32(u32 crc) const { return crc32Info.contains(crc); }
    bool isKnownFNV64(u64 fnv) const { return fnv64Info.contains(fnv); }


    //
    // Locating real Rom files
    //

    /* Returns the file holding this Rom, as found by the most recent scan.
     * Returns nothing if no scan has run, or if no scanned folder holds it.
     */
    std::optional<fs::path> getRom(const RomTraits &traits) const;
    std::optional<fs::path> getRomCRC32(u32 crc) const;
    std::optional<fs::path> getRomFNV64(u64 fnv) const;


    //
    // Managing the folder list
    //

    // Provides access to the folder list
    const std::vector<fs::path> &getFolderList() const { return folders; }

    // Empties the folder list (does not discard scan results)
    void clearFolderList();

    // Appends a folder, ignoring duplicates
    void addFolder(const fs::path &path);

    /* Rescans all folders and rebuilds the checksum-to-path maps. Recurses into
     * subdirectories, and skips anything it cannot read rather than failing the
     * whole scan. Earlier folders win: the first file found for a checksum is
     * the one kept, so the folder list doubles as a precedence order. Returns
     * the number of Roms located.
     */
    isize scanFolders();

    // Discards the scan results
    void clearScanResults();
};

}
