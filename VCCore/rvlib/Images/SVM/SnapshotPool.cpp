// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "SnapshotPool.h"

namespace retro::vault {

bool
SnapshotInfo::isCompatible() const
{
    return isC64();
}

const SnapshotInfo *
SnapshotPool::get(isize nr) const
{
    return const_cast<SnapshotPool *>(this)->get(nr);
}

SnapshotInfo *
SnapshotPool::get(isize nr)
{
    return nr >= 0 && nr < size() ? &snapshots[nr] : nullptr;
}

const SnapshotInfo *
SnapshotPool::get(utl::UUID uuid) const
{
    return const_cast<SnapshotPool *>(this)->get(uuid);
}

SnapshotInfo *
SnapshotPool::get(utl::UUID uuid)
{
    for (auto &info : snapshots) {
        if (info.uuid == uuid) return &info;
    }
    return nullptr;
}

void
SnapshotPool::insert(const SnapshotInfo &snapshot)
{
    auto it = std::lower_bound(snapshots.begin(),
                               snapshots.end(),
                               snapshot,
                               [](const SnapshotInfo &a, const SnapshotInfo &b) { return a.created > b.created; });

    snapshots.insert(it, snapshot);
}

bool
SnapshotPool::remove(isize nr)
{
    if (nr >= 0 && nr < (isize)snapshots.size()) {

        snapshots.erase(snapshots.begin() + nr);
        return true;
    }
    return false;
}

bool
SnapshotPool::remove(utl::UUID uuid)
{
    for (auto it = snapshots.begin(); it != snapshots.end(); ++it) {

        if ((*it).uuid == uuid) {

            snapshots.erase(it);
            return true;
        }
    }
    return false;
}

bool
SnapshotPool::removeFirst()
{
    if (snapshots.size() > 0) {

        snapshots.erase(snapshots.begin());
        return true;
    }
    return false;
}

bool
SnapshotPool::removeLast()
{
    if (snapshots.size() > 0) {

        snapshots.pop_back();
        return true;
    }

    return false;
}

void
SnapshotPool::shrink(isize newSize)
{
    while (!empty() && size() > newSize) {
        snapshots.pop_back();
    }
}

}
