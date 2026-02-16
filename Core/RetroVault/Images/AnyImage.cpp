// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "AnyImage.h"
#include "ADFFile.h"
#include "EADFFile.h"
#include "IMGFile.h"
#include "STFile.h"
#include "DMSFile.h"
#include "EXEFile.h"
#include "HDFFile.h"
#include "utl/io.h"
#include "utl/support.h"
#include <fstream>

namespace retro::vault {

optional<ImageInfo>
AnyImage::about(const fs::path& url)
{
    if (auto info = DiskImage::about(url)) return info;
    return {};
}

std::unique_ptr<AnyImage>
AnyImage::tryMake(const fs::path& path)
{
    if (auto img = DiskImage::tryMake(path)) return img;
    return nullptr;
}

std::unique_ptr<AnyImage>
AnyImage::make(const fs::path& path)
{
    if (auto img = tryMake(path)) return img;
    throw IOError(IOError::FILE_TYPE_UNSUPPORTED);
}

void
AnyImage::init(isize len)
{
    data.init(len);
}

void
AnyImage::init(const Buffer<u8> &buffer)
{
    init(buffer.ptr, buffer.size);
}

/*
void
AnyImage::init(const string &str)
{
    init((const u8 *)str.c_str(), (isize)str.length());
}
*/

void
AnyImage::init(const fs::path &path)
{
    if (!validateURL(path))
        throw IOError(IOError::FILE_TYPE_MISMATCH, path);

    std::fstream stream(path, std::ios::binary | std::ios::in | std::ios::out);
    
    if (!stream)
        throw IOError(IOError::FILE_NOT_FOUND, path);

    // Read file into a vector
    std::vector<u8> buffer((std::istreambuf_iterator<char>(stream)),
                           std::istreambuf_iterator<char>());
    
    if (buffer.empty() && file.fail())
        throw IOError(IOError::FILE_CANT_READ, path);
    
    // Initialize image with the vector contents
    init(buffer.data(), isize(buffer.size()));
    this->path = path;
    file = std::move(stream);
}

void
AnyImage::init(const u8 *buf, isize len)
{
    assert(buf);

    // Allocate memory
    data.alloc(len);

    // Copy data
    std::memcpy(data.ptr, buf, data.size);
    didInitialize();
}

void
AnyImage::copy(u8 *buf, isize offset, isize len) const
{
    assert(buf);
    assert(offset >= 0 && offset < data.size);
    assert(len >= 0 && offset + len <= data.size);

    std::memcpy(buf + offset, data.ptr, len);
}

ByteView
AnyImage::byteView(isize offset) const
{
    return byteView(offset, data.size - offset);
}

ByteView
AnyImage::byteView(isize offset, isize len) const
{
    assert(offset >= 0 && offset < data.size);
    assert(len >= 0 && offset + len <= data.size);

    return ByteView(data.ptr + offset, len);
}

MutableByteView
AnyImage::byteView(isize offset)
{
    return byteView(offset, data.size - offset);
}

MutableByteView
AnyImage::byteView(isize offset, isize len)
{
    assert(offset >= 0 && offset < data.size);
    assert(len >= 0 && offset + len <= data.size);

    return MutableByteView(data.ptr + offset, len);
}

void
AnyImage::copy(u8 *buf, isize offset) const
{
    copy (buf, offset, data.size);
}

void
AnyImage::save()
{
    save(Range<isize>{0,size()});
}

void
AnyImage::save(const Range<BlockNr> range)
{
    if (!file) throw IOError(IOError::FILE_NOT_FOUND, path);
    
    printf("Saving range %ld - %ld...\n", range.lower, range.upper - 1);
    
    // Move to the correct position
    file.seekp(range.lower, std::ios::beg);
    
    // Write the data to the stream
    file.write((char *)(data.ptr + range.lower), range.size());
    
    // Update the file on disk
    file.flush();
}

void
AnyImage::save(const std::vector<Range<BlockNr>> ranges)
{
    for (auto &range: ranges) save(range);
}

void
AnyImage::saveAs(const fs::path &newPath)
{
    // Fallback to the standard save function of paths match
    if (newPath == path) { save(); return; }

    // Make sure pending writes hit the disk
    file.flush();
    if (!file)
        throw IOError(IOError::FILE_CANT_CREATE, path);

    // Copy file
    try {

        fs::copy_file(path, newPath,
                      fs::copy_options::overwrite_existing);

    } catch (const fs::filesystem_error &) {

        throw IOError(IOError::FILE_CANT_WRITE, newPath);
    }

    // Close current stream
    file.close();

    // Reopen the stream with the new file
    file.open(newPath, std::ios::binary | std::ios::in | std::ios::out);
    if (!file)
        throw IOError(IOError::FILE_CANT_READ, newPath);

    path = newPath;
}

isize
AnyImage::writeToStream(std::ostream &stream, isize offset, isize len) const
{
    assert(offset >= 0 && len >= 0 && offset + len <= data.size);

    stream.write((char *)data.ptr + offset, len);

    return data.size;
}

isize
AnyImage::writeToFile(const fs::path &path, isize offset, isize len) const
{
    if (utl::isDirectory(path)) {
        throw IOError(IOError::FILE_IS_DIRECTORY);
    }

    std::ofstream stream(path, std::ofstream::binary);

    if (!stream.is_open()) {
        throw IOError(IOError::FILE_CANT_WRITE, path);
    }

    isize result = writeToStream(stream, offset, len);
    assert(result == data.size);

    return result;
}

isize
AnyImage::writeToStream(std::ostream &stream) const
{
    return writeToStream(stream, 0, data.size);
}

isize
AnyImage::writeToFile(const fs::path &path) const
{
    return writeToFile(path, 0, data.size);
}

}

