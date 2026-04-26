// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "FileSystems/CBM/PosixAdapter.h"
#include <sys/stat.h>

namespace retro::vault::cbm {

PosixAdapter::PosixAdapter(FileSystem &fs) : fs(fs)
{

}

NodeMeta *
PosixAdapter::getMeta(BlockNr nr)
{
    auto it = meta.find(nr);
    return it == meta.end() ? nullptr : &it->second;
}

NodeMeta &
PosixAdapter::ensureMeta(BlockNr nr)
{
    auto [it, inserted] = meta.try_emplace(nr);
    return it->second;
}

NodeMeta &
PosixAdapter::ensureMeta(HandleRef ref)
{
    return ensureMeta(getHandle(ref).node);
}

FSPosixStat
PosixAdapter::stat() const noexcept
{
    auto stat = fs.stat();

    return FSPosixStat {

        .name           = stat.name,
        .bsize          = stat.bsize,
        .blocks         = stat.blocks,

        .freeBlocks     = stat.freeBlocks,
        .usedBlocks     = stat.usedBlocks,
        .cachedBlocks   = stat.cachedBlocks,
        .dirtyBlocks    = stat.dirtyBlocks,
        
        .btime          = time_t{0},
        .mtime          = time_t{0},

        .generation     = stat.generation
    };
}

FSPosixAttr
PosixAdapter::attr(const fs::path &path) const
{
    if (auto stat = fs.attr(path)) {

        u32 prot = 0777 | (stat->isDir ? S_IFDIR : S_IFREG);
                
        return FSPosixAttr {

            .size           = stat->size,
            .blocks         = stat->blocks,
            .prot           = prot,
            .isDir          = stat->isDir,

            .btime          = time_t{0},
            .atime          = time_t{0},
            .mtime          = time_t{0},
            .ctime          = time_t{0},
        };
    }

    throw FSError(FSError::FS_NOT_FOUND);
}

void
PosixAdapter::mkdir(const fs::path &path)
{
    if (wp) throw FSError(FSError::FS_READ_ONLY);

    throw FSError(FSError::FS_UNSUPPORTED);
}

void
PosixAdapter::rmdir(const fs::path &path)
{
    if (wp) throw FSError(FSError::FS_READ_ONLY);

    throw FSError(FSError::FS_UNSUPPORTED);
}

std::vector<string>
PosixAdapter::readDir(const fs::path &path) const
{
    std::vector<string> result;

    auto dir = fs.readDir();

    for (auto &item : dir)
        result.push_back(item.getName().str());

    return result;
}

HandleRef
PosixAdapter::open(const fs::path &path, u32 flags)
{
    // Resolve path
    auto node = fs.seek(path);

    // Create a unique identifier
    auto ref = HandleRef { ++nextHandle };

    // Create a new file handle
    handles[ref] = Handle {

        .id = ref,
        .node = node,
        .offset = 0,
        .flags = flags
    };
    auto &handle = handles[ref];
    auto &info = ensureMeta(node);
    info.openHandles.insert(ref);

    // Evaluate flags
    if ((flags & O_TRUNC) && (flags & (O_WRONLY | O_RDWR))) {
        fs.resize(node, 0);
    }
    if (flags & O_APPEND) {
        handle.offset = lseek(ref, 0, SEEK_END);
    }

    return ref;
}

void
PosixAdapter::close(HandleRef ref)
{
    // Lookup handle
    auto &handle = getHandle(ref);
    auto header = handle.node;

    // Remove from metadata
    auto &info = ensureMeta(header);
    info.openHandles.erase(ref);

    // Remove from global handle table
    handles.erase(ref);

    // Attempt deletion after all references are gone
    tryReclaim(header);
}

void
PosixAdapter::unlink(const fs::path &path)
{
    if (wp) throw FSError(FSError::FS_READ_ONLY);

    auto node = fs.seek(path);

    if (auto *info = getMeta(node); info) {

        // Remove directory entry
        fs.unlink(node);

        // Decrement link count
        if (info->linkCount > 0) info->linkCount--;

        // Maybe delete
        tryReclaim(node);
    }
}

void
PosixAdapter::tryReclaim(BlockNr node)
{
    if (auto *info = getMeta(node); info) {

        if (info->linkCount == 0 && info->openCount() == 0) {

            // Delete file
            fs.reclaim(node);

            // Trash meta data
            meta.erase(node);
        }
    }
}

Handle &
PosixAdapter::getHandle(HandleRef ref)
{
    auto it = handles.find(ref);

    if (it == handles.end()) {
        throw FSError(FSError::FS_INVALID_HANDLE, std::to_string(isize(ref)));
    }

    return it->second;
}

BlockNr
PosixAdapter::ensureFile(const fs::path &path)
{
    // TODO
    return 0;
}

/*
BlockNr
PosixAdapter::ensureFileOrDirectory(const fs::path &path)
{
    auto node = fs.seek(path);
    require.fileOrDirectory(node);
    return node;
}

BlockNr
PosixAdapter::ensureDirectory(const fs::path &path)
{
    auto node = fs.seek(path);
    require.directory(node);
    return node;
}
*/

void
PosixAdapter::create(const fs::path &path)
{
    if (wp) throw FSError(FSError::FS_READ_ONLY);
    
    auto rel    = path.relative_path();
    auto parent = rel.parent_path();
    auto name   = rel.filename();

    // Reject nested paths (CBM is flat)
    if (!parent.empty() && parent != ".")
        throw FSError(FSError::FS_INVALID_PATH);

    // Reject invalid names
    if (name.empty() || name == ".")
        throw FSError(FSError::FS_INVALID_PATH);

    // Create file
    auto fdb = fs.createFile(PETName<16>(name));

    // Create meta info
    auto &info = ensureMeta(fdb);
    info.linkCount = 1;
}

isize
PosixAdapter::lseek(HandleRef ref, isize offset, u16 whence)
{
    auto &handle  = getHandle(ref);
    // auto &node    = fs.fetch(handle.node);
    auto fileSize = 0; // TODO // isize(node.getFileSize());

    isize newOffset;

    switch (whence) {

        case SEEK_SET:  newOffset = offset; break;
        case SEEK_CUR:  newOffset = handle.offset + offset; break;
        case SEEK_END:  newOffset = fileSize + offset; break;

        default:
            throw FSError(FSError::FS_INVALID_ARGUMENT, "whence: " + std::to_string(whence));
    }

    // Ensure that the offset is not negative
    newOffset = std::max(newOffset, (isize)0);

    // Update the file handle and return
    handle.offset = newOffset;
    return newOffset;
}

void
PosixAdapter::move(const fs::path &oldPath, const fs::path &newPath)
{
    if (wp) throw FSError(FSError::FS_READ_ONLY);
    
    auto oldName = PETName<16>(oldPath);
    auto newName = PETName<16>(newPath);

    fs.rename(oldName, newName);
}

void
PosixAdapter::chmod(const fs::path &path, u32 mode)
{
    if (wp) throw FSError(FSError::FS_READ_ONLY);
    
    throw FSError(FSError::FS_UNSUPPORTED);
}

void
PosixAdapter::resize(const fs::path &path, isize size)
{
    if (wp) throw FSError(FSError::FS_READ_ONLY);
    
    fs.resize(ensureFile(path), size);
}

isize
PosixAdapter::read(HandleRef ref, std::span<u8> buffer)
{
    auto &handle = getHandle(ref);
    auto &node   = fs.fetch(handle.node);
    auto &meta   = ensureMeta(node.nr);

    // Cache the file if necessary
    if (meta.cache.empty()) { fs.extractData(handle.node, meta.cache); }

    // Check for EOF
    if (handle.offset >= meta.cache.size) return 0;

    // Compute the number of bytes to read
    auto count = std::min(meta.cache.size - handle.offset, (isize)buffer.size());

    // Copy the requested range
    std::memcpy(buffer.data(), meta.cache.ptr + handle.offset, count);

    // Advance the handle offset
    handle.offset += count;

    return count;
}

isize
PosixAdapter::write(HandleRef ref, std::span<const u8> buffer)
{
    if (wp) throw FSError(FSError::FS_READ_ONLY);
    
    auto &handle = getHandle(ref);
    auto &meta   = ensureMeta(handle.node);

    // Cache the file if necessary
    if (meta.cache.empty()) { fs.extractData(handle.node, meta.cache); }

    // Determine the new file size
    auto newSize = std::max(meta.cache.size, handle.offset + (isize)buffer.size());

    // Resize the cached file if necessary (pad with 0)
    meta.cache.resize(newSize, 0);

    // Compute the number of bytes to write
    auto count = buffer.size();

    // Update data
    std::memcpy(meta.cache.ptr + handle.offset, buffer.data(), count);

    // Write back
    fs.replace(handle.node, meta.cache);

    return isize(count);
}

void
PosixAdapter::flush()
{
    fs.flush();
}

void
PosixAdapter::invalidate()
{
    fs.invalidate();
}

}
