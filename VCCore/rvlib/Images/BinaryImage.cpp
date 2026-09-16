// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "Images/BinaryImage.h"
#include "Devices/LinearDevice.h"
#include "utl/io.h"
#include "utl/support.h"
#include <fstream>

namespace retro::vault {

using utl::ByteView;
using utl::MutableByteView;
using utl::IOError;

void
BinaryImage::init(isize len)
{
    data.init(len, nullptr);
}

void
BinaryImage::init(const fs::path &p)
{
    if (!validateURL(p))
        throw utl::IOError(utl::IOError::FILE_TYPE_MISMATCH, p);

    // Open the file (throws if it does not exist or cannot be read)
    auto backing = makeBacking(p);

    if (backing->size() == 0)
        throw utl::IOError(utl::IOError::FILE_CANT_READ, p);

    // Determine the image size (before the backing is handed over below)
    auto size = imageSize(*backing);

    this->path = p;

    // Put the image on top of it. Nothing is loaded yet.
    data.init(size, std::move(backing));
    didInitialize();
}

void
BinaryImage::init(const LinearDevice &device)
{
    data.init(device.size(), nullptr);

    // Pull in the contents
    auto bytes = data.mutableByteView(0, data.size());
    device.read(bytes.data(), 0, bytes.size());
    didInitialize();
}

void
BinaryImage::init(const u8 *buf, isize len)
{
    assert(buf);

    data.init(len, nullptr);

    if (len) std::memcpy(data.mutableByteView(0, len).data(), buf, size_t(len));
    didInitialize();
}

std::unique_ptr<utl::Backing>
BinaryImage::makeBacking(const fs::path &p) const
{
    return std::make_unique<utl::FileBacking>(p);
}

utl::ByteView
BinaryImage::byteView(isize offset, isize len) const
{
    return data.byteView(offset, len);
}

utl::MutableByteView
BinaryImage::mutableByteView(isize offset, isize len)
{
    return data.mutableByteView(offset, len);
}

void
BinaryImage::detach()
{
    data.detach();
    path.clear();
}

void
BinaryImage::copy(u8 *buf, isize offset, isize len) const
{
    assert(buf);

    std::memcpy(buf, byteView(offset, len).data(), len);
}

void
BinaryImage::copy(u8 *buf, isize offset) const
{
    copy(buf, offset, getSize() - offset);
}

void
BinaryImage::save()
{
    // An image built in memory has nowhere to persist to yet
    if (!data.backed()) { saveAs(path); return; }

    // Write the modified parts back to where the image came from
    data.persist();
}

void
BinaryImage::saveAs(const fs::path &newPath)
{
    // Write the entire image first, so that a failure changes nothing
    writeToFile(newPath);

    // Continue on top of the new file, which now holds exactly this image
    auto size = getSize();
    auto backing = makeBacking(newPath);
    path = newPath;
    data.init(size, std::move(backing));
}

isize
BinaryImage::writeToStream(std::ostream &stream, isize offset, isize len) const
{
    stream.write((const char *)byteView(offset, len).data(), len);

    return len;
}

isize
BinaryImage::writeToFile(const fs::path &p, isize offset, isize len) const
{
    if (utl::isDirectory(p)) {
        throw utl::IOError(utl::IOError::FILE_IS_DIRECTORY);
    }

    /* The target may be the very file this image is loaded from. Opening it
     * for writing truncates it, so whatever is still in there only has to
     * come in first.
     */
    std::error_code ec;
    if (fs::equivalent(p, path, ec)) (void)byteView(0, getSize());

    std::ofstream stream(p, std::ofstream::binary);

    if (!stream.is_open()) {
        throw utl::IOError(utl::IOError::FILE_CANT_WRITE, p);
    }

    isize result = writeToStream(stream, offset, len);
    assert(result == len);

    return result;
}

isize
BinaryImage::writeToStream(std::ostream &stream) const
{
    return writeToStream(stream, 0, getSize());
}

isize
BinaryImage::writeToFile(const fs::path &p) const
{
    return writeToFile(p, 0, getSize());
}

}
