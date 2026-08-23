// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "Images/EXE/EXEFile.h"
#include "FileSystems/Amiga/FileSystem.h"
#include "utl/io.h"
#include "utl/support/Strings.h"
#include <format>

namespace retro::vault {

/* Declared here rather than inherited from ADFFile.h, which used to leak these
 * into every file that included it (see the note in that header).
 */
using retro::vault::amiga::FileSystem;
using retro::vault::amiga::FSFormat;
using retro::vault::amiga::BootBlockId;

optional<ImageInfo>
EXEFile::about(const fs::path &path)
{
    // Check file size
    auto suffix = utl::uppercased(path.extension().string());
    if (suffix != ".EXE") return {};

    // Only accept files fitting on a HD disk
    if (utl::getSizeOfFile(path) > 1710000) return {};

    // Check header signature
    u8 signature[] = { 0x00, 0x00, 0x03, 0xF3 };
    if (!utl::matchingFileHeader(path, signature, sizeof(signature))) return {};

    return {{ ImageType::FLOPPY, ImageFormat::EXE }};
}

std::vector<string>
EXEFile::describeImage() const noexcept
{
    return {
        "Amiga Floppy Disk",
        std::format("{} {}",
                    getDiameterStr(), getDensityStr()),
        std::format("{} Cylinders, {} Sides, {} Sectors",
                    numCyls(), numHeads(), numSectors(0))
    };
}

void
EXEFile::didInitialize()
{
    using amiga::FSName;

    // Check if this file requires a high-density disk
    bool hd = data.size > 853000;

    // Create a suitable ADF
    adf.init(Diameter::INCH_35, hd ? Density::HD : Density::DD);

    // Mount a file system on top of it
    auto vol = Volume(adf);
    auto fs = FileSystem(vol);

    // Format the file system and make it bootable
    fs.format(FSFormat::OFS);
    fs.setName(FSName("Disk"));
    fs.makeBootable(BootBlockId::AMIGADOS_13);

    // Add the executable, a script directory, and a script
    fs.createFile(fs.root(), FSName("file"), data);
    fs.createFile(fs.mkdir(fs.root(), FSName("s")), FSName("startup-sequence"), "file");

    // Finalize
    // fs.importer.updateChecksums();
    fs.flush();

    if CONSTEXPR (FS_VERIFY) {

        // Print some debug information about the volume
        fs.dumpState();

        // Check file system integrity
        fs.doctor.xray(true, std::cout, false);
    }
}

}
