// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "Images/AnyImage.h"
#include "Images/DiskImage.h"
#include "Images/SVM/SVMFile.h"
#include "utl/io.h"

namespace retro::vault {

/* SVMs are probed first.
 *
 * Not for precedence -- no disk format claims a '.svm', since every one of them
 * tests its own suffix before anything else -- but because an SVM is the only
 * image that can be a *directory*, and the disk probes go on to size and read
 * the path as a regular file. Asking the cheapest and most specific question
 * first keeps a folder from ever reaching them.
 */
optional<ImageInfo>
AnyImage::about(const fs::path& url)
{
    if (auto info = SVMFile::about(url))  return info;
    if (auto info = DiskImage::about(url)) return info;
    return {};
}

/* Note that this can throw rather than answer nullptr, and deliberately so.
 * SVMFile::about accepts a '.svm' on its suffix alone (opening the archive to
 * answer "what is this file" is what the class exists to avoid), so a corrupt
 * machine is only diagnosed when the constructor reads its manifest. Letting
 * VM_NO_MANIFEST out beats reporting a broken machine as an unknown file type.
 * DiskImage::tryMake behaves the same way -- HDFFile::about throws on a .hdf
 * of the wrong size.
 */
std::unique_ptr<AnyImage>
AnyImage::tryMake(const fs::path& path)
{
    if (SVMFile::about(path).has_value()) return std::make_unique<SVMFile>(path);
    if (auto img = DiskImage::tryMake(path)) return img;
    return nullptr;
}

std::unique_ptr<AnyImage>
AnyImage::make(const fs::path& path)
{
    if (auto img = tryMake(path)) return img;
    throw utl::IOError(utl::IOError::FILE_TYPE_UNSUPPORTED);
}

}
