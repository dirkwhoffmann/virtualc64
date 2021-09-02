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

class Recorder : public SubComponent {

    //
    // Constants
    //
    
    // Path to the FFmpeg executable
    static string ffmpegPath() { return "/usr/local/bin/ffmpeg"; }

    // Path to the two named input pipes
    static string videoPipePath() { return "/tmp/videoPipe"; }
    static string audioPipePath() { return "/tmp/audioPipe"; }

    // Path to the two temporary output files
    static string videoStreamPath() { return "/tmp/video.mp4"; }
    static string audioStreamPath() { return "/tmp/audio.mp4"; }

    // Log level passed to FFmpef
    static const string loglevel() { return REC_DEBUG ? "verbose" : "warning"; }
    

    //
    // Handles
    //
    
    // File handles to access FFmpeg
    FILE *videoFFmpeg = nullptr;
    FILE *audioFFmpeg = nullptr;

    // Video and audio pipe
    int videoPipe = -1;
    int audioPipe = -1;

    
    //
    // Recording status
    //
    
    // The current recorder state
    enum class State { wait, prepare, record, finalize, abort };
    State state = State::wait;

    // Number of records that have been made
    isize recordCounter = 0;
    
    
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
    
    using SubComponent::SubComponent;
    
    bool hasFFmpeg() const;

    
    //
    // Methods from C64Object
    //

private:
    
    const char *getDescription() const override { return "Recorder"; }
    void _dump(dump::Category category, std::ostream& os) const override;
    
    
    //
    // Methods from C64Object
    //

private:
    
    void _initialize() override;
    void _reset(bool hard) override;
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }

    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }

    
    //
    // Querying recording parameters
    //

public:
    
    isize getRecordCounter() const { return recordCounter; }
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
    bool startRecording(int x1, int y1, int x2, int y2,
                        long bitRate,
                        long aspectX,
                        long aspectY);
    
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
