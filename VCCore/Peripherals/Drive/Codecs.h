// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FloppyDisk.h"
#include "Drive.h"
#include "Images/D64/D64File.h"
#include "IMGFile.h"
#include "STFile.h"
#include "DMSFile.h"
#include "EXEFile.h"

using retro::vault::image::D64File;

namespace vc64 {

class Codec {

public:

    // Factory methods
    static std::unique_ptr<D64File> makeD64(FloppyDisk &disk);
    static std::unique_ptr<D64File> makeD64(Drive &drive);

    // Encoders and Decoders
    /*
    static void encodeIMG(const IMGFile &source, Disk &target);
    static void decodeIMG(IMGFile &target, const Disk &source);

    static void encodeST(const STFile &source, Disk &target);
    static void decodeST(STFile &target, const Disk &source);

    static void encodeDMS(const DMSFile &source, Disk &target);

    static void encodeEXE(const EXEFile &source, Disk &target);
    */
};

}

