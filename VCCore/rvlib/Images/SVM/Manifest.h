// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "SnapshotPool.h"
#include "utl/types/UUID.h"

namespace retro::vault {

/* Editorial information about a preinstalled showcase machine: who made the
 * production and what it is. It describes the showcase, not the virtual
 * machine, so it is absent on ordinary machines -- cloning a showcase drops it
 * (see SVMFile::init(CloneTag)), and that is precisely what turns the copy
 * into a standard VM.
 */
struct Meta {

    string author {};
    string description {};
};

struct Manifest {

    // General
    utl::UUID uuid {};
    string version {};
    Platform platform {};
    string name {};
    time_t created = 0;
    time_t modified = 0;

    /* Counts writes to the SVM. Bumped by every persist().
     *
     * Silicium and a running SiC64 instance work on the same root folder but
     * hold their own copy of this manifest, so either can be looking at data
     * the other has already replaced. Comparing generations is how an instance
     * tells: a number below what is on disk (or below what the other side
     * reports over RPC) means the copy in memory is outdated and has to be
     * re-read before it is used or written back.
     *
     * A timestamp would not do -- 'modified' has one-second resolution, and
     * two saves inside the same second are exactly the case that matters.
     */
    isize generation = 0;

    /* Write-protects the machine. Set on the preinstalled showcases, but the
     * two notions are independent: what a machine *is* follows from the meta
     * entry (see isPreinstalled()), whereas this governs only whether it may
     * be written.
     */
    bool readOnly = false;

    // Present on showcase machines only (see the type description)
    optional<Meta> meta;

    // Attachments
    fs::path screenshot {};
    fs::path startup {};

    /* Snapshots, newest first (SnapshotPool::insert keeps that order).
     *
     * One pool for everything. A snapshot taken on hibernation and one the
     * user asked for are the same thing -- a captured machine state -- and
     * telling them apart bought nothing but a second copy of every lookup,
     * every counter and every removal.
     */
    SnapshotPool snapshots;


    //
    // Initializing
    //

    explicit Manifest();
    explicit Manifest(fs::path path) : Manifest() { load(path); }
    explicit Manifest(std::istream &stream) : Manifest() { load(stream); }
    explicit Manifest(const char *buf, isize len) : Manifest() { load(buf, len); }
    explicit Manifest(const std::vector<u8> &vec) : Manifest() { load(vec); }


    //
    // Debugging
    //

    void dump(std::ostream &os) const;
    void dump(std::ostream &os, const SnapshotInfo &info) const;


    //
    // Querying properties
    //

    bool isAmiga() const { return platform == Platform::AMIGA; }
    bool isC64() const { return platform == Platform::C64; }
    bool isCompatible() const { return isC64(); }

    /* Preinstalled showcases are exactly the machines carrying editorial data,
     * so this is derived from the meta entry rather than tracked separately.
     * Cloning drops that entry (see SVMFile::init(CloneTag)), which is what
     * makes the copy an ordinary machine -- with nothing else to keep in sync.
     */
    bool isPreinstalled() const { return meta.has_value(); }

    bool isReadOnly() const { return readOnly; }


    //
    // Managing snapshots
    //

    isize numSnapshots() const { return snapshots.size(); }

    SnapshotInfo *lookupSnapshot(utl::UUID uuid) { return snapshots.get(uuid); }
    const SnapshotInfo *lookupSnapshot(utl::UUID uuid) const { return snapshots.get(uuid); }
    SnapshotInfo *lookupSnapshot(isize nr) { return snapshots.get(nr); }
    const SnapshotInfo *lookupSnapshot(isize nr) const { return snapshots.get(nr); }

    // The pool is ordered newest first, so the latest snapshot is the first one
    SnapshotInfo *lookupLatestSnapshot() { return snapshots.get(isize(0)); }
    const SnapshotInfo *lookupLatestSnapshot() const { return snapshots.get(isize(0)); }

    void appendSnapshot(const SnapshotInfo &info) { snapshots.insert(info); }

    bool removeSnapshot(utl::UUID uuid) { return snapshots.remove(uuid); }
    bool removeSnapshot(isize nr) { return snapshots.remove(nr); }
    bool removeFirstSnapshot() { return snapshots.removeFirst(); }
    bool removeLastSnapshot() { return snapshots.removeLast(); }


    //
    // Loading and saving
    //

    void load(fs::path path);
    void load(std::istream &stream);
    void load(const char *buf, isize len);
    void load(const std::vector<u8> &vec);
    void save(fs::path path) const;
};

}
