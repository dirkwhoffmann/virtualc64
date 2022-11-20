// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FFmpeg.h"
#include "IOUtils.h"

std::vector<string> FFmpeg::paths;
string FFmpeg::exec;

void
FFmpeg::init()
{
    auto add = [&](const string &path) {
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

const string
FFmpeg::getExecPath()
{
    return exec;
}

void
FFmpeg::setExecPath(const string &path)
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

    auto cmd = getExecPath() + " " + args;
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
