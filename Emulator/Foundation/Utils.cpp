// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "Utils.h"

#include <ctype.h>

bool
releaseBuild()
{
#ifdef RELEASEBUILD
    return true;
#else
    return false;
#endif
}

void
sprint8d(char *s, u8 value)
{
    for (int i = 2; i >= 0; i--) {
        u8 digit = value % 10;
        s[i] = '0' + digit;
        value /= 10;
    }
    s[3] = 0;
}

void
sprint8x(char *s, u8 value)
{
    for (int i = 1; i >= 0; i--) {
        u8 digit = value % 16;
        s[i] = (digit <= 9) ? ('0' + digit) : ('A' + digit - 10);
        value /= 16;
    }
    s[2] = 0;
}

void
sprint8b(char *s, u8 value)
{
    for (int i = 7; i >= 0; i--) {
        s[i] = (value & 0x01) ? '1' : '0';
        value >>= 1;
    }
    s[8] = 0;
}

void
sprint16d(char *s, u16 value)
{
    for (int i = 4; i >= 0; i--) {
        u8 digit = value % 10;
        s[i] = '0' + digit;
        value /= 10;
    }
    s[5] = 0;
}

void
sprint16x(char *s, u16 value)
{
    for (int i = 3; i >= 0; i--) {
        u8 digit = value % 16;
        s[i] = (digit <= 9) ? ('0' + digit) : ('A' + digit - 10);
        value /= 16;
    }
    s[4] = 0;
}

void
sprint16b(char *s, u16 value)
{
    for (int i = 15; i >= 0; i--) {
        s[i] = (value & 0x01) ? '1' : '0';
        value >>= 1;
    }
    s[16] = 0;
}

void hexdump(u8 *p, usize size, usize cols, usize pad)
{
    while (size) {
        
        usize cnt = MIN(size, cols);
        for (usize x = 0; x < cnt; x++) {
            fprintf(stderr, "%02X %s", p[x], ((x + 1) % pad) == 0 ? " " : "");
        }
        
        size -= cnt;
        p += cnt;
        
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}

void hexdump(u8 *p, usize size, usize cols)
{
    hexdump(p, size, cols, cols);
}

void hexdumpWords(u8 *p, usize size, usize cols)
{
    hexdump(p, size, cols, 2);
}

void hexdumpLongwords(u8 *p, usize size, usize cols)
{
    hexdump(p, size, cols, 4);
}

bool isZero(const u8 *ptr, usize size)
{
    for (usize i = 0; i < size; i++) {
        if (ptr[i]) return false;
    }
    return true;
}

string
extractFileName(const string &s)
{
    auto idx = s.rfind('/');
    auto pos = idx != std::string::npos ? idx + 1 : 0;
    auto len = std::string::npos;
    return s.substr(pos, len);
}

string
extractSuffix(const string &s)
{
    auto idx = s.rfind('.');
    auto pos = idx != std::string::npos ? idx + 1 : 0;
    auto len = std::string::npos;
    return s.substr(pos, len);
}

string stripSuffix(const string &s)
{
    auto idx = s.rfind('.');
    auto pos = 0;
    auto len = idx != std::string::npos ? idx : std::string::npos;
    return s.substr(pos, len);
}

/*
string
extractFileNameWithoutSuffix(const string &s)
{
    auto i1 = s.rfind('/');
    auto i2 = s.rfind('.');
    printf("i1 = %ld i2 = %ld\n", i1, i2);
    auto pos = i1 != std::string::npos ? i1 + 1 : 0;
    auto len = i2 != std::string::npos && i2 > i1 ? i2 - i1 : std::string::npos;
    
    return s.substr(pos, len);
}
*/

char *
extractFileName(const char *path)
{
    assert(path);
    
    string s(path);
    return strdup(extractFileName(s).c_str());
    /*
    const char *pos = strrchr(path, '/');
    return pos ? strdup(pos + 1) : strdup(path);
    */
}

char *
extractSuffix(const char *path)
{
    assert(path);
    
    string s(path);
    return strdup(extractSuffix(s).c_str());
    /*
    const char *pos = strrchr(path, '.');
    return pos ? strdup(pos + 1) : strdup("");
    */
}

char *
extractFileNameWithoutSuffix(const char *path)
{
    assert(path);
    
    string s(path);
    return strdup(stripSuffix(extractFileName(s)).c_str());
    /*
    assert(path);
    
    char *result;
    char *filename = extractFileName(path);
    char *suffix   = extractSuffix(filename);
    
    if (strlen(suffix) == 0)
        result = strdup(filename);
    else
        result = strndup(filename, strlen(filename) - strlen(suffix) - 1);
    
    free(filename);
    free(suffix);
    return result;
    */
}

bool
checkFileSuffix(const char *filename, const char *suffix)
{
	assert(filename);
	assert(suffix);
	
	if (strlen(suffix) > strlen(filename))
		return false;
	
	filename += (strlen(filename) - strlen(suffix));
	if (strcmp(filename, suffix) == 0)
		return true;
	else
		return false;
}

std::string
suffix(const std::string &name)
{
    auto idx = name.rfind('.');
    return idx != std::string::npos ? name.substr(idx + 1) : "";
}

bool isDirectory(const std::string &path)
{
    return isDirectory(path.c_str());
}

bool isDirectory(const char *path)
{
    struct stat fileProperties;
    
    if (path == nullptr)
        return -1;
        
    if (stat(path, &fileProperties) != 0)
        return -1;
    
    return S_ISDIR(fileProperties.st_mode);
}

usize numDirectoryItems(const std::string &path)
{
    return numDirectoryItems(path.c_str());
}

usize numDirectoryItems(const char *path)
{
    usize count = 0;
    
    if (DIR *dir = opendir(path)) {
        
        struct dirent *dp;
        while ((dp = readdir(dir))) {
            if (dp->d_name[0] != '.') count++;
        }
    }
    
    return count;
}

long
getSizeOfFile(const char *filename)
{
    struct stat fileProperties;
    
    if (filename == nullptr)
        return -1;
    
    if (stat(filename, &fileProperties) != 0)
        return -1;
    
    return fileProperties.st_size;
}

bool
checkFileSize(const char *filename, long min, long max)
{
    long filesize = getSizeOfFile(filename);
    
    if (filesize == -1) {
        return false;
    }
    if (min > 0 && filesize < min) {
        return false;
    }
    if (max > 0 && filesize > max) {
        return false;
    }
    return true;
}

bool
matchingFileHeader(const char *path, const u8 *header, usize length)
{
    assert(path);
    assert(header);
    
    bool result = true;
    FILE *file;
    
    if ((file = fopen(path, "r")) == nullptr) {
        return false;
    }
    for (usize i = 0; i < length; i++) {
        int c = fgetc(file);
        if (c != (int)header[i]) {
            result = false;
            break;
        }
    }

    fclose(file);
    return result;
}

bool
matchingBufferHeader(const u8 *buffer, const u8 *header, usize length)
{
    assert(buffer);
    assert(header);

    for (usize i = 0; i < length; i++) {
        if (header[i] != buffer[i])
        return false;
    }

    return true;
}

bool matchingStreamHeader(std::istream &stream, const u8 *header, usize length)
{
    stream.seekg(0, std::ios::beg);
    
    for (usize i = 0; i < length; i++) {
        int c = stream.get();
        if (c != (int)header[i]) {
            stream.seekg(0, std::ios::beg);
            return false;
        }
    }
    stream.seekg(0, std::ios::beg);
    return true;
}

bool loadFile(const std::string &path, u8 **bufptr, long *lenptr)
{
    assert(bufptr); assert(lenptr);

    std::ifstream stream(path);
    if (!stream.is_open()) return false;
    
    usize len = streamLength(stream);
    u8 *buf = new u8[len];
    stream.read((char *)buf, len);
    
    *bufptr = buf;
    *lenptr = len;
    return true;
}

bool loadFile(const std::string &path, const std::string &name, u8 **bufptr, long *lenptr)
{
    return loadFile(path + "/" + name, bufptr, lenptr);
}

[[deprecated]] bool
loadFile(const char *path, u8 **bufptr, long *lenptr)
{
    assert(path);
    assert(bufptr);
    assert(lenptr);

    *bufptr = nullptr;
    *lenptr = 0;
    
    // Get file size
    long bytes = getSizeOfFile(path);
    if (bytes == -1) return false;
    
    // Open file
    FILE *file = fopen(path, "r");
    if (file == nullptr) return false;
     
    // Allocate memory
    u8 *data = new u8[bytes];
    if (data == nullptr) { fclose(file); return false; }
    
    // Read data
    for (unsigned i = 0; i < bytes; i++) {
        int c = fgetc(file);
        if (c == EOF) break;
        data[i] = (u8)c;
    }
    
    fclose(file);
    *bufptr = data;
    *lenptr = bytes;
    return true;
}

[[deprecated]] bool
loadFile(const char *path, const char *name, u8 **bufptr, long *lenptr)
{
    assert(path != nullptr);
    assert(name != nullptr);

    char *fullpath = new char[strlen(path) + strlen(name) + 2];
    strcpy(fullpath, path);
    strcat(fullpath, "/");
    strcat(fullpath, name);
    
    return loadFile(fullpath, bufptr, lenptr);
}

usize
streamLength(std::istream &stream)
{
    auto cur = stream.tellg();
    stream.seekg(0, std::ios::beg);
    auto beg = stream.tellg();
    stream.seekg(0, std::ios::end);
    auto end = stream.tellg();
    stream.seekg(cur, std::ios::beg);
    
    return (usize)(end - beg);
}

u32
fnv_1a_32(u8 *addr, usize size)
{
    if (addr == nullptr || size == 0) return 0;

    u32 hash = fnv_1a_init32();

    for (usize i = 0; i < size; i++) {
        hash = fnv_1a_it32(hash, (u32)addr[i]);
    }

    return hash;
}

u64
fnv_1a_64(u8 *addr, usize size)
{
    if (addr == nullptr || size == 0) return 0;

    u64 hash = fnv_1a_init64();

    for (usize i = 0; i < size; i++) {
        hash = fnv_1a_it64(hash, (u64)addr[i]);
    }

    return hash;
}

u32
crc32(const u8 *addr, usize size)
{
    if (addr == nullptr || size == 0) return 0;

    u32 result = 0;

    // Setup lookup table
    u32 table[256];
    for(int i = 0; i < 256; i++) table[i] = crc32forByte(i);

    // Compute CRC-32 checksum
     for(usize i = 0; i < size; i++)
       result = table[(u8)result ^ addr[i]] ^ result >> 8;

    return result;
}

u32
crc32forByte(u32 r)
{
    for(int j = 0; j < 8; ++j)
        r = (r & 1? 0: (u32)0xEDB88320L) ^ r >> 1;
    return r ^ (u32)0xFF000000L;
}
