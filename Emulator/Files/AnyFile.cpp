// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "AnyFile.h"
#include "Checksum.h"

AnyFile::AnyFile(usize capacity)
{
    data = new u8[capacity]();
    size = capacity;
}

AnyFile::~AnyFile()
{
    if (data) delete[] data;
}

PETName<16>
AnyFile::getName() const
{
    auto idx = path.rfind('/');
    usize start = idx != std::string::npos ? idx + 1 : 0;
    
    idx = path.rfind('.');
    usize len = idx != std::string::npos ? idx - start : std::string::npos;
    
    return PETName<16>(path.substr(start, len));
}

u64
AnyFile::fnv() const
{
    return data ? util::fnv_1a_64(data, size) : 0;
}

void
AnyFile::flash(u8 *buffer, isize offset)
{
    assert(buffer);
    memcpy(buffer + offset, data, size);
}

isize
AnyFile::readFromStream(std::istream &stream)
{
    // Get stream size
    auto fsize = stream.tellg();
    stream.seekg(0, std::ios::end);
    fsize = stream.tellg() - fsize;
    stream.seekg(0, std::ios::beg);

    // Allocate memory
    assert(data == nullptr);
    data = new u8[fsize]();
    size = fsize;

    // Read data
    stream.read((char *)data, size);
    
    // Fix known inconsistencies
    repair();
    
    return size;
}

isize
AnyFile::readFromFile(const string  &path)
{
    std::ifstream stream(path);

    if (!stream.is_open()) {
        throw VC64Error(ERROR_FILE_CANT_READ);
    }
    
    usize result = readFromStream(stream);
    assert(result == size);
    
    this->path = string(path);
    return result;
}

isize
AnyFile::readFromBuffer(const u8 *buf, isize len)
{
    assert(buf);

    std::istringstream stream(std::string((const char *)buf, len));
    
    usize result = readFromStream(stream);
    assert(result == size);
    
    return result;
}

isize
AnyFile::writeToStream(std::ostream &stream)
{
    stream.write((char *)data, size);
    return size;
}

isize
AnyFile::writeToStream(std::ostream &stream, ErrorCode *err)
{
    try { *err = ERROR_OK; return writeToStream(stream); }
    catch (VC64Error &exception) { *err = exception.data; return 0; }
}

isize
AnyFile::writeToFile(const string &path)
{
    std::ofstream stream(path);

    if (!stream.is_open()) {
        throw VC64Error(ERROR_FILE_CANT_WRITE);
    }
    
    isize result = writeToStream(stream);
    assert(result == (isize)size);
    
    return result;
}

isize
AnyFile::writeToFile(const string &path, ErrorCode *ec)
{
    try { *ec = ERROR_OK; return writeToFile(path); }
    catch (VC64Error &exception) { *ec = exception.data; return 0; }
}

isize
AnyFile::writeToBuffer(u8 *buf)
{
    assert(buf);

    memcpy(buf, data, size);
    return size;
}

isize
AnyFile::writeToBuffer(u8 *buf, ErrorCode *err)
{
    try { *err = ERROR_OK; return writeToBuffer(buf); }
    catch (VC64Error &exception) { *err = exception.data; return 0; }
}
