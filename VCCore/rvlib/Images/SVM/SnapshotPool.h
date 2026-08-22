// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "SVMTypes.h"
#include "utl/types/UUID.h"

namespace retro::vault {

struct SnapshotInfo {

    string version;
    utl::UUID uuid;
    string name;
    Platform platform;
    time_t created = 0;
    time_t modified = 0;
    fs::path screenshot;
    fs::path binary;

    bool isAmiga() const { return platform == Platform::AMIGA; }
    bool isC64() const { return platform == Platform::C64; }
    bool isCompatible() const;
};


class SnapshotPool {

    std::vector<SnapshotInfo> snapshots;

    //
    // Searching the database
    //

  public:

    isize size() const { return isize(snapshots.size()); }
    bool empty() const { return size() == 0; }

    const std::vector<SnapshotInfo> &get() const { return snapshots; }
    std::vector<SnapshotInfo> &get() { return snapshots; }
    const SnapshotInfo *get(isize nr) const;
    SnapshotInfo *get(isize nr);
    const SnapshotInfo *get(utl::UUID uuid) const;
    SnapshotInfo *get(utl::UUID uuid);

    //
    // Adding and removing items
    //

    // Inserts a snapshot, keeping the list sorted by date
    void insert(const SnapshotInfo &snapshot);

    // Removes a snapshot
    bool remove(isize nr);
    bool remove(utl::UUID uuid);
    bool removeFirst();
    bool removeLast();

    // Shrinks the snapshot list (oldest snapshots are deleted first)
    void shrink(isize newSize);

};

}
