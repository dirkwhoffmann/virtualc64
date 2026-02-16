// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "BlockDevice.h"

namespace retro::vault {

using namespace utl;

class TrackDevice : public BlockDevice {

    // Maps a track to its first block
    mutable std::vector<isize> track2block;

public:

    struct CHS {

        isize cylinder;
        isize head;
        isize sector;
    };

    struct TS {

        isize track;
        isize sector;
    };

    TrackDevice() { }
    virtual ~TrackDevice() = default;

private:

    // Sets up the track map
    void buildTrackMap() const;

    // Maps a block to its track
    isize block2track(isize b) const;


    //
    // Querying capacity information
    //

public:

    virtual isize numCyls() const = 0;
    virtual isize numHeads() const = 0;
    virtual isize numSectors(isize t) const = 0;

    isize numSectors(isize c, isize h) const { return numSectors(c * numHeads() + h); }
    isize numTracks() const { return numHeads() * numCyls(); }
    isize numBlocks() const { return capacity(); }
    isize numBytes() const { return capacity() * bsize(); }


    //
    // Validating indices
    //

public:

    // Non-throwing
    bool isValidBlockNr(isize b) const noexcept;
    bool isValidTrackNr(isize t) const noexcept;
    bool isValidSectorNr(isize t, isize s) const noexcept;
    bool isValidCylinderNr(isize c) const noexcept;
    bool isValidHeadNr(isize h) const noexcept;

    bool isValidTS(TS ts) const noexcept;
    bool isValidCHS(CHS chs) const noexcept;

    // Throwing
    void validateBlockNr(isize b) const;
    void validateTrackNr(isize t) const;
    void validateSectorNr(isize t, isize s) const;
    void validateCylinderNr(isize c) const;
    void validateHeadNr(isize h) const;

    void validateTS(TS ts) const;
    void validateCHS(CHS chs) const;


    //
    // Mapping [c]ylinders, [h]eads, [s]ectors, [b]locks
    //

public:

    // Translates to cylinder/head/sector format
    CHS b2chs(isize b) const;
    CHS ts2chs(isize t, isize s) const;
    CHS ts2chs(const TS &ts) const { return ts2chs(ts.track, ts.sector); }

    // Translates to track/sector format
    TS b2ts(isize b) const;
    TS chs2ts(isize c, isize h, isize s) const;
    TS chs2ts(const CHS &chs) const { return chs2ts(chs.cylinder, chs.head, chs.sector); }

    // Translates to block numbers
    isize bindex(const CHS &chs) const;
    isize bindex(const TS &ts) const;

    // Translates to byte offsets
    isize boffset(const CHS &chs) const { return bindex(chs) * bsize(); }
    isize boffset(const TS &ts) const { return bindex(ts) * bsize(); }


    //
    // Accessing blocks
    //

public:

    // Reads a block
    using BlockDevice::readBlock;
    void readBlock(u8 *dst, CHS chs) const { readBlock(dst, bindex(chs)); }
    void readBlock(u8 *dst, isize c, isize h, isize s) const { readBlock(dst, CHS{c,h,s}); }
    void readBlock(u8 *dst, TS ts) const { readBlock(dst, bindex(ts)); }
    void readBlock(u8 *dst, isize t, isize s) const { readBlock(dst, TS{t,s}); }

    // Writes a block
    using BlockDevice::writeBlock;
    void writeBlock(const u8 *src, CHS chs) { writeBlock(src, bindex(chs)); }
    void writeBlock(const u8 *src, isize c, isize h, isize s) { writeBlock(src, CHS{c,h,s}); }
    void writeBlock(const u8 *src, TS ts) { writeBlock(src, bindex(ts)); }
    void writeBlock(const u8 *src, isize t, isize s) { writeBlock(src, TS{t,s}); }


    //
    // Accessing tracks
    //

    // Reads a track
    virtual void readTrack(u8 *dst, isize nr) const;
    void readTrack(span<u8> dst, isize nr) const;

    // Writes a track
    virtual void writeTrack(const u8 *src, isize nr);
    void writeTrack(span<const u8> src, isize nr);
};

}
