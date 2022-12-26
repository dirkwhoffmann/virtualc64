// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "SubComponent.h"

namespace vc64 {

/* This class stores some information about the host system. The values have
 * are set the GUI on start and updated on-the-fly when a value changes.
 */
class Host : public SubComponent {

    // Audio sample rate
    double sampleRate = 44100.0;

    // Video refresh rate of the host monitor
    double refreshRate = 60.0;

    // Framebuffer dimensions
    isize frameBufferWidth = 0;
    isize frameBufferHeight = 0;


    //
    // Initializing
    //

public:

    Host(C64& ref);


    //
    // Methods from AmigaObject
    //

private:

    const char *getDescription() const override { return "Host"; }
    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from AmigaComponent
    //

private:

    void _reset(bool hard) override { };
    isize _size() override { return 0; }
    u64 _checksum() override { return 0; }
    isize _load(const u8 *buffer) override { return 0; }
    isize _save(u8 *buffer) override { return 0; }


    //
    // Accessing properties
    //

public:

    double getSampleRate() const { return sampleRate; }
    void setSampleRate(double hz);

    double getHostRefreshRate() const { return refreshRate; }
    void setHostRefreshRate(double fps);

    std::pair<isize, isize> getFrameBufferSize() const;
    void setFrameBufferSize(std::pair<isize, isize> size);

};

}

