// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"
#include "utl/io/IOError.h"
#include <filesystem>
#include <fstream>
#include <random>

namespace fs = std::filesystem;

namespace utl {

/* A file in the system's temporary directory that deletes itself.
 *
 * The file is created empty and exists from construction until the object
 * goes away; whoever holds the object decides how long that is. Moving hands
 * the file over, so a temporary file can be returned from a function and
 * kept by the caller.
 *
 * A default constructed TempFile holds no file. So does one that was moved
 * from. empty() tells the two apart from one that does.
 *
 * Removal at the end is best effort: it is not worth throwing from a
 * destructor over a file the operating system cleans up anyway.
 */
class TempFile {

    fs::path file;

public:

    // Creates nothing
    TempFile() = default;

    // Creates an empty file, whose name ends in the given suffix
    explicit TempFile(const string &suffix) {

        static std::mt19937_64 rng { std::random_device {} () };

        auto dir = fs::temp_directory_path();

        // Names are picked at random, so several at once cannot collide
        for (isize attempt = 0; attempt < 64; attempt++) {

            auto name = "utl-" + std::to_string(rng()) + suffix;
            auto candidate = dir / name;

            if (fs::exists(candidate)) continue;

            std::ofstream out(candidate, std::ios::binary);
            if (!out.is_open()) break;

            file = candidate;
            return;
        }

        throw IOError(IOError::FILE_CANT_WRITE, dir);
    }

    ~TempFile() { reset(); }

    TempFile(const TempFile &) = delete;
    TempFile &operator=(const TempFile &) = delete;

    TempFile(TempFile &&other) noexcept : file(std::move(other.file)) {

        other.file.clear();
    }

    TempFile &operator=(TempFile &&other) noexcept {

        if (this != &other) {

            reset();
            file = std::move(other.file);
            other.file.clear();
        }
        return *this;
    }

    // Returns the name of the file, empty if there is none
    const fs::path &path() const { return file; }

    // Returns true if this object holds no file
    bool empty() const { return file.empty(); }

    // Deletes the file, leaving this object holding none
    void reset() noexcept {

        if (!file.empty()) {

            std::error_code ec;
            fs::remove(file, ec);
            file.clear();
        }
    }
};

}
