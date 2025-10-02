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

#include "config.h"
#include "FFmpeg.h"
#include "IOUtils.h"

namespace vc64 {

std::vector<std::filesystem::path> FFmpeg::paths;
std::filesystem::path FFmpeg::exec;

void
FFmpeg::init()
{
    auto add = [&](const std::filesystem::path &path) {
        if (util::getSizeOfFile(path) > 0 && !FORCE_NO_FFMPEG) {
            paths.push_back(path);
        }
    };

    if (paths.empty()) {

        add("/Applications/ffmpeg");
        add("/usr/bin/ffmpeg");
        add("/usr/local/bin/ffmpeg");
        add("/opt/bin/ffmpeg");
        add("/opt/homebrew/bin/ffmpeg");

        // Use the first entry as the default executable
        if (!paths.empty()) exec = paths[0];
    }
}

const fs::path
FFmpeg::getExecPath()
{
    return exec;
}

void
FFmpeg::setExecPath(const std::filesystem::path &path)
{
    // If an empty string is passed, assign the first default location
    if (path == "" && !paths.empty()) {

        exec = paths[0];
        return;
    }

    exec = path;
}

bool
FFmpeg::available()
{
#ifdef _WIN32

    return false;

#else

    return util::getSizeOfFile(exec) > 0;

#endif
}

bool
FFmpeg::launch(const string &args)
{
#ifdef _WIN32

    return false;

#else

    auto cmd = getExecPath().string() + " " + args;
    handle = popen(cmd.c_str(), "w");
    return handle != nullptr;

#endif
}

bool
FFmpeg::isRunning()
{
#ifdef _WIN32

    return false;

#else

    return handle != nullptr;

#endif
}

void
FFmpeg::join()
{
#ifdef _WIN32

#else

    pclose(handle);
    handle = nullptr;

#endif
}

}
