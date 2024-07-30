// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "BasicTypes.h"
#include <vector>
#include <filesystem>

namespace vc64 {

namespace fs = ::std::filesystem;

class FFmpeg final {

public:

    // A list of available FFmpeg executables (setup in init() )
    static std::vector<std::filesystem::path> paths;

    // Path to the selected FFmpeg executable
    static std::filesystem::path exec;

#ifdef _MSC_VER

#else
    FILE *handle = nullptr;
#endif

    //
    // Locating FFmpeg
    //

    // Sets up the 'path' vector
    static void init();

    // Getter and setter for the FFmpeg executable path
    static const fs::path getExecPath();
    static void setExecPath(const fs::path &path);

    // Checks whether FFmeg is available
    static bool available();


    //
    // Running FFmpeg
    //

    // Launches the FFmpeg instance
    bool launch(const string &args);

    // Returns true if the FFmpeg instance is currently running
    bool isRunning();

    // Waits until the FFmpeg instance has terminated
    void join();
};

}
