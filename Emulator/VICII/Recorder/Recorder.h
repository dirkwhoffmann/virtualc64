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
#include "Chrono.h"
#include "FFmpeg.h"
#include "NamedPipe.h"

class Recorder : public SubComponent {

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
    
    // All possible recorder states
    enum class State { wait, prepare, record, finalize, abort };

    // The current recorder state
    State state = State::wait;

    
    //
    // Recording parameters
    //
      
    // Frame rate, Bit rate, Sample rate
    isize frameRate = 0;
    isize bitRate = 0;
    isize sampleRate = 0;

    // Sound samples per frame (882 for PAL, 735 for NTSC)
    isize samplesPerFrame = 0;
    
    // The texture cutout that is going to be recorded
    struct { isize x1; isize y1; isize x2; isize y2; } cutout;
            
    // Time stamps
    util::Time recStart;
    util::Time recStop;
    
    
    //
    // Initializing
    //
    
public:
    
    Recorder(C64& ref);

    
    //
    // Methods from C64Object
    //

private:
    
    const char *getDescription() const override { return "Recorder"; }
    void _dump(Category category, std::ostream& os) const override;
    
    
    //
    // Methods from C64Object
    //

private:
    
    void _initialize() override;
    void _reset(bool hard) override;
    
    template <class T>
    void applyToPersistentItems(T& worker) { }

    template <class T>
    void applyToResetItems(T& worker, bool hard = true) { }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    u64 _checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }


    //
    // Querying locations and flags
    //

    // Returns the paths to the two named input pipes
    string videoPipePath();
    string audioPipePath();

    // Return the paths to the two temporary output files
    string videoStreamPath();
    string audioStreamPath();

    //Returns the log level passed to FFmpef
    const string loglevel() { return REC_DEBUG ? "verbose" : "warning"; }


    //
    // Querying recording parameters
    //

public:

    util::Time getDuration() const;
    isize getFrameRate() const { return frameRate; }
    isize getBitRate() const { return bitRate; }
    isize getSampleRate() const { return sampleRate; }


    //
    // Starting and stopping a video capture
    //

public:

    // Checks whether the screen is currently recorded
    bool isRecording() const { return state != State::wait; }

    // Starts the screen recorder
    void startRecording(isize x1, isize y1, isize x2, isize y2,
                        isize bitRate,
                        isize aspectX, isize aspectY) throws;

    // Stops the screen recorder
    void stopRecording();

    // Exports the recorded video
    bool exportAs(const string &path);
        
    
    //
    // Recording a video stream
    //

public:
        
    // Records a single frame
    void vsyncHandler();
    
private:
    
    void prepare();
    void record();
    void recordVideo();
    void recordAudio();
    void finalize();
    void abort();
};
