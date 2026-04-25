// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "D64File.h"
#include "C64Encoder.h"
#include "C64Decoder.h"
#include "utl/io.h"
#include "utl/support/Strings.h"
#include <format>

namespace retro::vault::image {

optional<ImageInfo>
D64File::about(const fs::path &path)
{
    // Check suffix
    if (utl::uppercased(path.extension().string()) != ".D64") return {};

    // Check file size
    auto len = utl::getSizeOfFile(path);

    bool match =
    len == D64_683_SECTORS ||
    len == D64_683_SECTORS_ECC ||
    len == D64_768_SECTORS ||
    len == D64_768_SECTORS_ECC ||
    len == D64_802_SECTORS ||
    len == D64_802_SECTORS_ECC;
    if (!match) return {};

    return {{ ImageType::FLOPPY, ImageFormat::D64 }};
}

const TrackDefaults &
D64File::trackDefaults(isize t)
{
    static constexpr TrackDefaults trackDefaults[42] = {

        // Speedzone 3 (outer tracks)                   // Index    Track   Halftracks
        { 21, 3, 7693, 7693 * 8,   0,  8, 0.268956 },   // 0,       1,      1,  2
        { 21, 3, 7693, 7693 * 8,  21,  8, 0.724382 },   // 1,       2,      3,  4
        { 21, 3, 7693, 7693 * 8,  42,  8, 0.177191 },   // 2,       3,      5,  6
        { 21, 3, 7693, 7693 * 8,  63,  8, 0.632698 },   // 3,       4,      7,  8
        { 21, 3, 7693, 7693 * 8,  84,  8, 0.088173 },   // 4,       5,      9,  10
        { 21, 3, 7693, 7693 * 8, 105,  8, 0.543583 },   // 5,       6,      11, 12
        { 21, 3, 7693, 7693 * 8, 126,  8, 0.996409 },   // 6,       7,      13, 14
        { 21, 3, 7693, 7693 * 8, 147,  8, 0.451883 },   // 7,       8,      15, 16
        { 21, 3, 7693, 7693 * 8, 168,  8, 0.907342 },   // 8,       9,      17, 18
        { 21, 3, 7693, 7693 * 8, 289,  8, 0.362768 },   // 9,       10,     19, 20
        { 21, 3, 7693, 7693 * 8, 210,  8, 0.815512 },   // 10,      11,     21, 22
        { 21, 3, 7693, 7693 * 8, 231,  8, 0.268338 },   // 11,      12,     23, 24
        { 21, 3, 7693, 7693 * 8, 252,  8, 0.723813 },   // 12,      13,     25, 26
        { 21, 3, 7693, 7693 * 8, 273,  8, 0.179288 },   // 13,      14,     27, 28
        { 21, 3, 7693, 7693 * 8, 294,  8, 0.634779 },   // 14,      15,     29, 30
        { 21, 3, 7693, 7693 * 8, 315,  8, 0.090253 },   // 15,      16,     31, 32
        { 21, 3, 7693, 7693 * 8, 336,  8, 0.545712 },   // 16,      17,     33, 34

        // Speedzone 2
        { 19, 2, 7143, 7143 * 8, 357, 17, 0.945418 },   // 17,      18,     35, 36
        { 19, 2, 7143, 7143 * 8, 376, 17, 0.506081 },   // 18,      19,     37, 38
        { 19, 2, 7143, 7143 * 8, 395, 17, 0.066622 },   // 19,      20,     39, 40
        { 19, 2, 7143, 7143 * 8, 414, 17, 0.627303 },   // 20,      21,     41, 42
        { 19, 2, 7143, 7143 * 8, 433, 17, 0.187862 },   // 21,      22,     43, 44
        { 19, 2, 7143, 7143 * 8, 452, 17, 0.748403 },   // 22,      23,     45, 46
        { 19, 2, 7143, 7143 * 8, 471, 17, 0.308962 },   // 23,      24,     47, 48

        // Speedzone 1
        { 18, 1, 6667, 6667 * 8, 490, 12, 0.116926 },   // 24,      25,     49, 50
        { 18, 1, 6667, 6667 * 8, 508, 12, 0.788086 },   // 25,      26,     51, 52
        { 18, 1, 6667, 6667 * 8, 526, 12, 0.459190 },   // 26,      27,     53, 54
        { 18, 1, 6667, 6667 * 8, 544, 12, 0.130238 },   // 27,      28,     55, 56
        { 18, 1, 6667, 6667 * 8, 562, 12, 0.801286 },   // 28,      29,     57, 58
        { 18, 1, 6667, 6667 * 8, 580, 12, 0.472353 },   // 29,      30,     59, 60

        // Speedzone 0 (inner tracks)
        { 17, 0, 6250, 6250 * 8, 598,  9, 0.834120 },   // 30,      31,     61, 62
        { 17, 0, 6250, 6250 * 8, 615,  9, 0.614880 },   // 31,      32,     63, 64
        { 17, 0, 6250, 6250 * 8, 632,  9, 0.395480 },   // 32,      33,     65, 66
        { 17, 0, 6250, 6250 * 8, 649,  9, 0.176140 },   // 33,      34,     67, 68
        { 17, 0, 6250, 6250 * 8, 666,  9, 0.956800 },   // 34,      35,     69, 70

        // Speedzone 0 (usually unused tracks)
        { 17, 0, 6250, 6250 * 8, 683,  9, 0.300 },      // 35,      36,     71, 72
        { 17, 0, 6250, 6250 * 8, 700,  9, 0.820 },      // 36,      37,     73, 74
        { 17, 0, 6250, 6250 * 8, 717,  9, 0.420 },      // 37,      38,     75, 76
        { 17, 0, 6250, 6250 * 8, 734,  9, 0.940 },      // 38,      39,     77, 78
        { 17, 0, 6250, 6250 * 8, 751,  9, 0.540 },      // 39,      40,     79, 80
        { 17, 0, 6250, 6250 * 8, 768,  9, 0.130 },      // 40,      41,     81, 82
        { 17, 0, 6250, 6250 * 8, 785,  9, 0.830 }       // 41,      42,     83, 84
    };

    assert(t >= 0 && t < 42);
    return trackDefaults[t];
}

std::vector<string>
D64File::describeImage() const noexcept
{
    return {
        "Commodore 64 Floppy Disk",
        std::format("{} {}",
                    getDiameterStr(), getDensityStr()),
        std::format("{} Tracks, {} Blocks",
                    numTracks(), numBlocks())
    };
}

isize
D64File::numCyls() const noexcept
{
    switch (data.size) {

        case D64_683_SECTORS:
        case D64_683_SECTORS_ECC:   return 35;
        case D64_768_SECTORS:
        case D64_768_SECTORS_ECC:   return 40;
        case D64_802_SECTORS:
        case D64_802_SECTORS_ECC:   return 42;

        default:
            fatalError;
    }
}

isize
D64File::numHeads() const noexcept
{
    return 1;
}

isize
D64File::numSectors(isize t) const noexcept
{
    if (t < 0 || t >= numTracks()) return 0;

    return trackDefaults(t).sectors;
}

Diameter
D64File::getDiameter() const noexcept
{
    return Diameter::INCH_525;
}

Density
D64File::getDensity() const noexcept
{
    return Density::DD;
}

BitView
D64File::encode(TrackNr t) const
{
    validateTrackNr(t);
    auto &track = gcrTracks.at(t);

    // Get the disk id bytes from the BAM
    auto *bam = data.ptr + 357 * bsize();
    u8 id1 = bam[0xA2];
    u8 id2 = bam[0xA3];

    // Setup the encoder
    auto encoder = C64Encoder(id1, id2, ecc());

    // Encode track
    auto gcr = encoder.encodeTrack(byteView(t), t);

    // Copy the encoded track data
    track.assign(gcr.data(), gcr.data() + gcr.byteView().size());

    // Return a bit view with the proper size
    return BitView(track.data(), gcr.size());
}

void
D64File::decode(TrackNr t, BitView bits)
{
    validateTrackNr(t);

    C64Decoder decoder;

    // Decode track
    auto bytes = decoder.decodeTrack(bits, t);
    assert(bytes.size() == D64File::trackDefaults(t).sectors * 256);

    // Copy back decoded bytes
    memcpy(byteView(t).data(), bytes.data(), bytes.size());
}

bool
D64File::hasEcc() const noexcept
{
    switch (data.size) {

        case D64_683_SECTORS_ECC: return true;
        case D64_768_SECTORS_ECC: return true;
        case D64_802_SECTORS_ECC: return true;

        default:
            return false;
    }
}

std::vector<u8>
D64File::ecc() const noexcept
{
    if (!hasEcc()) return {};

    return std::vector<u8>(data.ptr + bsize() * numBlocks(),
                           data.ptr + bsize() * numBlocks() + numBlocks());
}

u8
D64File::getErrorCode(isize b) const
{
    validateBlockNr(b);

    auto codes = ecc();
    return b < isize(codes.size()) ? codes[b] : 0;
}

isize
D64File::tsOffset(isize t, isize s) const
{
    return (trackDefaults(t).firstSectorNr + s) * bsize();
}

}
