// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Recorder.h"
#include "IO.h"
#include "MsgQueue.h"
#include "SIDBridge.h"
#include "VICII.h"

Recorder::Recorder(C64& ref) : C64Component(ref)
{
}

bool
Recorder::hasFFmpeg() const
{
    return util::getSizeOfFile(ffmpegPath()) > 0;
}

void
Recorder::_initialize()
{
        
}

void
Recorder::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
}

void
Recorder::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    os << tab("FFmpeg path") << ffmpegPath() << std::endl;
    os << tab("Installed") << bol(hasFFmpeg()) << std::endl;
    os << tab("Video pipe") << bol(videoPipe != -1) << std::endl;
    os << tab("Audio pipe") << bol(audioPipe != -1) << std::endl;
    os << tab("Recording") << bol(isRecording()) << std::endl;
}
    
bool
Recorder::startRecording(int x1, int y1, int x2, int y2,
                         long bitRate,
                         long aspectX,
                         long aspectY)
{
    synchronized {
        
        debug(REC_DEBUG, "startRecording(%d,%d,%d,%d,%ld,%ld,%ld)\n",
              x1, y1, x2, y2, bitRate, aspectX, aspectY);
        
        if (isRecording()) return false;
        
        // Create pipes
        debug(REC_DEBUG, "Creating pipes...\n");
        
        unlink(videoPipePath().c_str());
        unlink(audioPipePath().c_str());
        if (mkfifo(videoPipePath().c_str(), 0666) == -1) return false;
        if (mkfifo(audioPipePath().c_str(), 0666) == -1) return false;
        
        debug(REC_DEBUG, "Pipes created\n");
        dump();
        
        // Make sure the screen dimensions are even
        if ((x2 - x1) % 2) x2--;
        if ((y2 - y1) % 2) y2--;
        cutout.x1 = x1;
        cutout.x2 = x2;
        cutout.y1 = y1;
        cutout.y2 = y2;
        debug(REC_DEBUG, "Recorded area: (%d,%d) - (%d,%d)\n", x1, y1, x2, y2);
        
        //
        // Assemble the command line arguments for the video encoder
        //
        
        // Path to the FFmpeg executable
        string cmd1 = ffmpegPath() + " -nostdin";
        
        // Verbosity
        cmd1 += " -loglevel " + loglevel();
        
        // Input stream format
        cmd1 += " -f:v rawvideo -pixel_format rgba";
        
        // Frame rate
        cmd1 += " -r " + std::to_string(frameRate);
        
        // Frame size (width x height)
        cmd1 += " -s:v " + std::to_string(x2 - x1) + "x" + std::to_string(y2 - y1);
        
        // Input source (named pipe)
        cmd1 += " -i " + videoPipePath();
        
        // Output stream format
        cmd1 += " -f mp4 -pix_fmt yuv420p";
        
        // Bit rate
        cmd1 += " -b:v " + std::to_string(bitRate) + "k";
        
        // Aspect ratio
        cmd1 += " -bsf:v ";
        cmd1 += "\"h264_metadata=sample_aspect_ratio=";
        cmd1 += std::to_string(aspectX) + "/" + std::to_string(2*aspectY) + "\"";
        
        // Output file
        cmd1 += " -y " + videoStreamPath();
        
        
        //
        // Assemble the command line arguments for the audio encoder
        //
        
        // Path to the FFmpeg executable
        string cmd2 = ffmpegPath() + " -nostdin";
        
        // Verbosity
        cmd2 += " -loglevel " + loglevel();
        
        // Audio format and number of channels
        cmd2 += " -f:a f32le -ac 2";
        
        // Sampling rate
        cmd2 += " -sample_rate " + std::to_string(sampleRate);
        
        // Input source (named pipe)
        cmd2 += " -i " + audioPipePath();
        
        // Output stream format
        cmd2 += " -f mp4";
        
        // Output file
        cmd2 += " -y " + audioStreamPath();
        
        //
        // Launch FFmpeg instances
        //
        
        assert(videoFFmpeg == nullptr);
        assert(audioFFmpeg == nullptr);
        
        msg("\nStarting video encoder with options:\n%s\n", cmd1.c_str());
        videoFFmpeg = popen(cmd1.c_str(), "w");
        if (!videoFFmpeg) {
            msg("Failed to launch the video encoder\n");
            return false;
        }
        
        msg("\nStarting audio encoder with options:\n%s\n", cmd2.c_str());
        audioFFmpeg = popen(cmd2.c_str(), "w");
        if (!audioFFmpeg) {
            msg("Failed to launch the audio encoder\n");
            return false;
        }
        
        // Open pipes
        msg("Opening video pipe\n");
        videoPipe = open(videoPipePath().c_str(), O_WRONLY);
        if (!videoPipe) {
            msg("Failed to launch the video pipe\n");
            return false;
        }
        msg("Opening audio pipe\n");
        audioPipe = open(audioPipePath().c_str(), O_WRONLY);
        if (!audioPipe) {
            msg("Failed to launch the audio pipe\n");
            return false;
        }
        
        msg("Success\n");
        state = State::prepare;
    }
    
    return true;
}

void
Recorder::stopRecording()
{
    debug(REC_DEBUG, "stopRecording()\n");

    synchronized {
        
        if (isRecording()) {
            state = State::finalize;
        }
    }
}

bool
Recorder::exportAs(const string &path)
{
    if (isRecording()) return false;
    
    //
    // Assemble the command line arguments for the video encoder
    //
    
    // char cmd[512]; char *ptr = cmd;
    string cmd;
    
    // Path to the FFmpeg executable
    cmd += ffmpegPath();

    // Verbosity
    cmd += " -loglevel " + loglevel();

    // Input streams
    cmd += " -i " + videoStreamPath();
    cmd += " -i " + audioStreamPath();

    // Don't reencode
    cmd += " -c:v copy -c:a copy";

    // Output file
    cmd += " -y " + path;

    //
    // Launch FFmpeg
    //
    
    msg("\nMerging video and audio stream with options:\n%s\n", cmd.c_str());

    if (system(cmd.c_str()) == -1) {
        warn("Failed: %s\n", cmd.c_str());
    }
    
    msg("Done\n");
    return true;
}

void
Recorder::vsyncHandler()
{
    // Quick-exit if the recorder is not active
    if (state == State::wait) return;
    
    synchronized {
        
        switch (state) {
                
            case State::wait: break;
            case State::prepare: prepare(); break;
            case State::record: record(); break;
            case State::finalize: finalize(); break;
        }
    }
}

void
Recorder::prepare()
{
    printf("Prepare\n");
    
    /* The PAL C64 frame rate is 50.125 Hz, derived from a system clock
     * frequency of 0.985248 MHz. Because we record a 50 Hz video stream, we
     * need to scale the sample frequency by (50.125 / 50) to make SID produce
     * the correct number of sound samples per frame (882 samples for a 44100 Hz
     * stream).
     */
    sid.setSampleRate(sampleRate * 50.125 / 50.0);
    
    // Start with a nearly empty buffer
    sid.stream.lock();
    while (sid.stream.count() > 1) sid.stream.read();
    sid.stream.unlock();

    // Switch state and inform the GUI
    state = State::record;
    messageQueue.put(MSG_RECORDING_STARTED);
}

void
Recorder::record()
{
    assert(videoFFmpeg != nullptr);
    assert(audioFFmpeg != nullptr);
    assert(videoPipe != -1);
    assert(audioPipe != -1);
    
    recordVideo();
    recordAudio();
}

void
Recorder::recordVideo()
{
    u32 *texture = (u32 *)vic.stableEmuTexture();
    
    isize width = sizeof(u32) * (cutout.x2 - cutout.x1);
    isize height = cutout.y2 - cutout.y1;
    isize offset = cutout.y1 * TEX_WIDTH + cutout.x1 + FIRST_VISIBLE_PIXEL * 4;

    u8 *data = new u8[width * height];
    u8 *src = (u8 *)(texture + offset);
    u8 *dst = data;
    
    for (isize y = 0; y < height; y++, src += 4 * TEX_WIDTH, dst += width) {
        memcpy(dst, src, width);
    }
    
    // Feed the video pipe
    (void)write(videoPipe, data, width * height);
}

void
Recorder::recordAudio()
{
    // printf("Samples: %zd\n", sid.stream.count());
    assert(sid.stream.count() >= 882);
    
    for (isize i = 0; i < 882; i++) {
    
        // Feed the audio pipe
        SamplePair pair = sid.stream.read();
        (void)write(audioPipe, &pair.left, sizeof(float));
        (void)write(audioPipe, &pair.right, sizeof(float));
    }
    
    sid.stream.clear();
}

void
Recorder::finalize()
{
    recordCounter++;
    
    // Close pipes
    close(videoPipe);
    close(audioPipe);
    videoPipe = -1;
    audioPipe = -1;
    
    // Shut down encoders
    pclose(videoFFmpeg);
    pclose(audioFFmpeg);
    videoFFmpeg = nullptr;
    audioFFmpeg = nullptr;
    
    // Switch state and inform the GUI
    state = State::wait;
    messageQueue.put(MSG_RECORDING_STOPPED);
}
