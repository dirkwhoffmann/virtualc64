// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Types.h"
#include <vector>

class FFmpeg {

public:

    // A list of available FFmpeg executables (setup in init() )
    static std::vector<string> paths;

    // Path to the selected FFmpeg executable
    static string exec;

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
    static const string getExecPath();
    static void setExecPath(const string &path);

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
