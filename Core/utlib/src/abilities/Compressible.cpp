// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/abilities/Compressible.h"
#include "utl/support/Bits.h"
#include "utl/io/IOError.h"
#include "lz4.h"

#ifdef USE_ZLIB
#include <zlib.h>
#endif

namespace utl {

#ifdef USE_ZLIB

void
Compressible::gzip(u8 *buffer, isize len, std::vector<u8> &result)
{
    // Only proceed if there is anything to zip
    if (len == 0) return;

    // Remember the initial length of the result vector
    auto initialLen = result.size();

    // Resize the target buffer
    result.resize(initialLen + len + len / 2 + 256);

    // Configure the zlib stream
    z_stream zs {

        .next_in   = (Bytef *)buffer,
        .avail_in  = (uInt)len,
        .next_out  = (Bytef *)result.data() + initialLen,
        .avail_out = (uInt)result.size(),
    };

    // Select the gzip format by choosing adequate window bits
    constexpr int windowBits = MAX_WBITS | 16;

    // Initialize the zlib stream
    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        throw std::runtime_error("Failed to initialize zlib.");
    }

    // Run the zlib decompressor
    if (auto ret = deflate(&zs, Z_FINISH); ret != Z_STREAM_END) {

        deflateEnd(&zs);
        throw std::runtime_error("Zlib error " + std::to_string(ret));
    }

    // Reduce the target buffer to the correct size
    result.resize(initialLen + zs.total_out);
    deflateEnd(&zs);
}

void
Compressible::gunzip(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate)
{
    // Only proceed if there is anything to unzip
    if (len == 0) return;

    // Remember the initial length of the result vector
    auto initialLen = result.size();

    // Configure the zlib stream
    z_stream zs {

        .next_in   = (Bytef *)buffer,
        .avail_in  = (uInt)len,
    };

    // Select the gzip format by choosing adequate window bits
    constexpr int windowBits = MAX_WBITS | 16;

    // Initialize the zlib stream
    if (inflateInit2(&zs, windowBits) != Z_OK) {
        throw std::runtime_error("Failed to initialize zlib.");
    }

    // For speedup: Estimate the size and reserve elements
    result.reserve(initialLen + (sizeEstimate ? sizeEstimate : 2 * len));

    // Decompress in smaller chunks
    std::vector<uint8_t> chunk(8192); int ret;

    do {

        zs.next_out = chunk.data();
        zs.avail_out = static_cast<uInt>(chunk.size());

        switch (ret = inflate(&zs, Z_NO_FLUSH)) {

            case Z_ERRNO:
            case Z_STREAM_ERROR:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
            case Z_BUF_ERROR:
            case Z_VERSION_ERROR:

                inflateEnd(&zs);
                throw IOError(IOError::ZLIB_ERROR, "Zlib error " + std::to_string(ret));

            default:
                break;
        }

        auto count = chunk.size() - zs.avail_out;
        result.insert(result.end(), chunk.begin(), chunk.begin() + count);

    } while (ret != Z_STREAM_END);

    inflateEnd(&zs);
}

#else

void
Compressible::gzip(u8 *buffer, isize len, std::vector<u8> &result) {
    throw std::runtime_error("No zlib support.");
}
void
Compressible::gunzip(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate) {
    throw std::runtime_error("No zlib support.");
}

#endif

void
Compressible::lz4(u8 *buffer, isize len, std::vector<u8> &result)
{
    // Only proceed if there is anything to compress
    if (len == 0) return;

    // Remember the initial length of the result vector
    auto initialLen = result.size();

    // Resize the target buffer
    auto max_size = len + len / 2 + 256;
    result.resize(initialLen + max_size);

    // Run the LZ4 encoder
    auto compressedSize = LZ4_compress_default((const char *)buffer,
                                               (char *)result.data() + initialLen,
                                               (int)len,
                                               (int)max_size);
    if (compressedSize <= 0) {
        throw std::runtime_error("LZ4 error: compression failure");
    }

    // Append the uncompressed size
    W32BE(result.data() + initialLen + compressedSize, len);
    compressedSize += 4;

    // Reduce the target buffer to the correct size
    result.resize(initialLen + compressedSize);
}

void
Compressible::unlz4(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate)
{
    // Only proceed if there is anything to uncompress
    if (len == 0) return;
    if (len <= 3) throw std::runtime_error("LZ4 error: impossible length");

    // Remember the initial length of the result vector
    auto initialLen = result.size();

    // Resize the result vector to the correct size
    int decompressedSize = R32BE(buffer + len - 4);
    result.resize(initialLen + decompressedSize);
    char *decompressed = (char *) &result[initialLen];

    auto compressedSize = len - 4;
    auto size = LZ4_decompress_safe((const char *)buffer,
                                                 (char *)decompressed,
                                                 (int)compressedSize,
                                                 (int)decompressedSize);

    // Perform sanity checks
    if (size < 0) {
        throw std::runtime_error("LZ4 error: decompression failure");
    }
    if (size != decompressedSize) {
        throw std::runtime_error("LZ4 error: inconsistent lengths");
    }
}

void
Compressible::rle(isize n, u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate)
{
    /* This function performs a run-length encoding according to the following
     * scheme:
     *
     * k < n:   AA ... AA B   ->   AA ... AA B
     *          <-- k -->          <-- k -->
     *
     * k >= n:  AA ... AA B   ->   AA ... AA [max] ... [max] [l] B
     *          <-- k -->          <-- n --> <--- sum = k-n --->
     *
     *                             with max = std::numeric_limits<T>::max()
     *
     * Note: For k = n, the second case implies:
     *
     *          AA ... AA B   ->   AA ... AA [0] B
     *          <-- n -->          <-- n -->
     *
     * In this case, compression has a negative effect. Examples:
     *
     *      ABBCCCDDDDEEEEE   ->   ABB0CC1DD2EE3
     *         AABBCCDDEEFF   ->   AA0BB0CC0DD0EE0FF0
     */

    const auto max = isize(std::numeric_limits<u8>::max());
    u8 prev = 0;
    isize repetitions = 0;

    // For speedup: Start with a container with a decent capacity
    if (sizeEstimate) result.reserve(sizeEstimate);

    auto encode = [&](u8 element, isize count) {

        // Encode up to n symbols in plain text
        for (isize k = 0; k < std::min(n, count); k++) result.push_back(element);

        count -= n;

        // Append the run length sequence
        while (count >= 0) {

            auto runlength = std::min(count, max);
            result.push_back(u8(runlength));
            count -= runlength;

            if (runlength != max) break;
        }
    };

    // Perform run-length encoding
    for (isize i = 0; i < len; i++) {

        if (buffer[i] == prev) {

            repetitions++;

        } else {

            encode(prev, repetitions);
            prev = buffer[i];
            repetitions = 1;
        }
    }
    encode(prev, repetitions);
}

void
Compressible::unrle(isize n, u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate)
{
    const auto max = isize(std::numeric_limits<u8>::max());
    u8 prev = 0;
    isize repetitions = 0;

    // For speedup: Start with a container with a decent capacity
    result.reserve(sizeEstimate ? sizeEstimate : 2 * len);

    for (isize i = 0; i < len; i++) {

        result.push_back(buffer[i]);
        repetitions = prev != buffer[i] ? 1 : repetitions + 1;
        prev = buffer[i];

        if (repetitions == n) {

            while (i < len - 1) {

                auto runlength = isize(buffer[++i]);
                result.insert(result.end(), runlength, prev);
                if (runlength != max) break;
            }
            repetitions = 0;
        }
    }
}

void
Compressible::rle2(u8 *buffer, isize len, std::vector<u8> &result) {
    rle(2, buffer, len, result);
}

void
Compressible::unrle2(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate) {
    unrle(2, buffer, len, result);
}

void
Compressible::rle3(u8 *buffer, isize len, std::vector<u8> &result) {
    unrle(3, buffer, len, result);
}

void
Compressible::unrle3(u8 *buffer, isize len, std::vector<u8> &result, isize sizeEstimate) {
    unrle(3, buffer, len, result);
}

}
