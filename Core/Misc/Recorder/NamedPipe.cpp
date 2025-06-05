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
#include "NamedPipe.h"
#include "IOUtils.h"
#ifndef _WIN32
#include <unistd.h>
#endif

namespace vc64 {

bool
NamedPipe::create(const string &name)
{
    this->name = name;

#ifdef _WIN32

    return false;

#else

    ::unlink(name.c_str());
    return ::mkfifo(name.c_str(), 0666) != -1;

#endif
}

bool
NamedPipe::open()
{
#ifdef _WIN32

    return false;

#else

    pipe = ::open(name.c_str(), O_WRONLY);
    return pipe != -1;

#endif
}

bool
NamedPipe::isOpen()
{
#ifdef _WIN32

    return false;

#else

    return pipe != -1;

#endif
}

bool
NamedPipe::close()
{
#ifdef _WIN32

    return false;

#else

    auto result = ::close(pipe);
    pipe = -1;
    return result == 0;

#endif
}

isize
NamedPipe::write(u8 *buffer, isize length)
{
#ifdef _WIN32

    return 0;

#else

    assert(isOpen());
    return ::write(pipe, (void *)buffer, (size_t)length);

#endif
}

}
