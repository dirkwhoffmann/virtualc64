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
#include "SampleRateDetector.h"
#include "Emulator.h"
#include <algorithm>

namespace vc64 {

void
SampleRateDetector::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    if (category == Category::State) {

    }
}

void
SampleRateDetector::_didReset(bool hard)
{

}

void
SampleRateDetector::feed(isize samples)
{
    if (count) {

        // Measure how much time has passed since the previous call
        auto delay = delta.restart().asSeconds();

        trace(TIM_DEBUG, "Requested %ld samples in %f seconds (%.0f)\n", samples, delay, samples / delay);

        // Record the measured value
        if (buffer.isFull()) (void)buffer.read();
        buffer.write(count / delay);
    }

    count = samples;
}

double
SampleRateDetector::sampleRate()
{
    double result = 0.0;

    // Get stored samples
    auto samples = buffer.vector();
    auto size = samples.size();

    // Return a default value if we do not have enough samples
    if (size < usize(2 * trash + 1)) { return 44100; }

    // Sort all entries
    std::sort(samples.begin(), samples.end());

    // Compute average (lowest two and highest two value are ignored)
    for (usize i = trash; i < size - trash; i++) result += samples[i];
    result /= size - 2 * trash;

    trace(TIM_DEBUG, "Sample rate = %.2f\n", result);
    return result;
}

}
