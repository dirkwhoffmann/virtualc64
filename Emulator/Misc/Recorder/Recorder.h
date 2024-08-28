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

#pragma once

#include "RecorderTypes.h"
#include "SubComponent.h"
#include "Chrono.h"
#include "FFmpeg.h"
#include "NamedPipe.h"

namespace vc64 {

class Recorder final : public SubComponent, public Inspectable<RecorderInfo> {

    Descriptions descriptions = {{

        .name           = "Recorder",
        .description    = "Screen Recorder",
        .shell          = "recorder"
    }};

    Options options = {

        OPT_REC_FRAME_RATE,
        OPT_REC_BIT_RATE,
        OPT_REC_SAMPLE_RATE,
        OPT_REC_ASPECT_X,
        OPT_REC_ASPECT_Y
    };

    // Current configuration
    RecorderConfig config = { };

    
    //
    // Handles
    //

    // FFmpeg instances
    FFmpeg videoFFmpeg;
    FFmpeg audioFFmpeg;

    // Video and audio pipes
    NamedPipe videoPipe;
    NamedPipe audioPipe;

    
    //
    // Recording status
    //

    // The current recorder state
    RecState state = REC_STATE_WAIT;

    
    //
    // Recording parameters
    //

    // Sound samples per frame (882 for PAL, 735 for NTSC)
    isize samplesPerFrame = 0;
    
    // The texture cutout that is going to be recorded
    struct { isize x1; isize y1; isize x2; isize y2; } cutout;

    // Time stamps
    util::Time recStart;
    util::Time recStop;
    
    
    //
    // Methods
    //
    
public:
    
    Recorder(C64& ref);
    Recorder& operator= (const Recorder& other) { return *this; }


    //
    // Methods from Serializable
    //

public:

    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;
    void _initialize() override;


    //
    // Configuring
    //

public:

    const RecorderConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;


    //
    // Inspecting
    //

public:

    void cacheInfo(RecorderInfo &result) const override;


    //
    // Querying locations and flags
    //

    // Returns the paths to the two named input pipes
    string videoPipePath();
    string audioPipePath();

    // Return the paths to the two temporary output files
    string videoStreamPath();
    string audioStreamPath();

    // Returns the log level passed to FFmpef
    const string loglevel() { return REC_DEBUG ? "verbose" : "warning"; }

private:

    // Returns the length of the recorded video
    util::Time getDuration() const;


    //
    // Starting and stopping a video capture
    //

public:

    // Checks whether the screen is currently recorded
    bool isRecording() const { return state != REC_STATE_WAIT; }

    // Starts the screen recorder
    void startRecording(isize x1, isize y1, isize x2, isize y2);

    // Stops the screen recorder
    void stopRecording();

    // Exports the recorded video
    bool exportAs(const fs::path &path);

    
    //
    // Recording a video stream
    //

public:

    // Records a single frame
    void vsyncHandler();
    
private:
    
    void prepare();
    void recordVideo();
    void recordAudio();
    void finalize();
    void abort();
};

}
