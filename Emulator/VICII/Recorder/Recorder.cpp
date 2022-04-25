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
#include "C64.h"

Recorder::Recorder(C64& ref) : SubComponent(ref)
{

}

void
Recorder::_initialize()
{
    FFmpeg::init();
}

void
Recorder::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
}

void
Recorder::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::State) {

        os << tab("FFmpeg path");
        os << FFmpeg::getExecPath() << std::endl;
        os << tab("Installed");
        os << bol(FFmpeg::available()) << std::endl;
        os << tab("Recording");
        os << bol(isRecording()) << std::endl;
    }
}

string
Recorder::videoPipePath()
{
    return c64.tmp("videoPipe").string();
}

string
Recorder::audioPipePath()
{
    return c64.tmp("audioPipe").string();
}

string
Recorder::videoStreamPath()
{
    return c64.tmp("video.mp4").string();
}

string
Recorder::audioStreamPath()
{
    return c64.tmp("audio.mp4").string();
}

util::Time
Recorder::getDuration() const
{
    return (isRecording() ? util::Time::now() : recStop) - recStart;
}

void
Recorder::startRecording(isize x1, isize y1, isize x2, isize y2,
                         isize bitRate,
                         isize aspectX, isize aspectY)
{
    SYNCHRONIZED

    debug(REC_DEBUG, "startRecording(%ld,%ld,%ld,%ld,%ld,%ld,%ld)\n",
          x1, y1, x2, y2, bitRate, aspectX, aspectY);

    if (isRecording()) {
        throw VC64Error(ERROR_REC_LAUNCH, "Recording in progress.");
    }

    // Create pipes
    debug(REC_DEBUG, "Creating pipes...\n");

    if (!videoPipe.create(videoPipePath())) {
        throw VC64Error(ERROR_REC_LAUNCH, "Failed to create the video encoder pipe.");
    }
    if (!audioPipe.create(audioPipePath())) {
        throw VC64Error(ERROR_REC_LAUNCH, "Failed to create the video encoder pipe.");
    }

    debug(REC_DEBUG, "Pipes created\n");
    dump(Category::State);

    debug(REC_DEBUG, "startRecording(%ld,%ld,%ld,%ld,%ld,%ld,%ld)\n",
          x1, y1, x2, y2, bitRate, aspectX, aspectY);

    // Make sure the screen dimensions are even
    if ((x2 - x1) % 2) x2--;
    if ((y2 - y1) % 2) y2--;

    // Remember the cutout
    cutout.x1 = x1;
    cutout.x2 = x2;
    cutout.y1 = y1;
    cutout.y2 = y2;
    debug(REC_DEBUG, "Recorded area: (%ld,%ld) - (%ld,%ld)\n", x1, y1, x2, y2);

    // Set the bit rate, frame rate, and sample rate
    this->bitRate = bitRate;
    frameRate = vic.pal() ? 50 : 60;;
    sampleRate = 44100;
    samplesPerFrame = sampleRate / frameRate;

    // Create temporary buffers
    // debug(REC_DEBUG, "Creating buffers...\n");

    // videoData.alloc((x2 - x1) * (y2 - y1));
    // audioData.alloc(2 * samplesPerFrame);

    //
    // Assemble the command line arguments for the video encoder
    //

    debug(REC_DEBUG, "Assembling command line arguments\n");

    // Console interactions
    string cmd1 = " -nostdin";

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
    cmd1 += std::to_string(aspectX) + "/" + std::to_string(aspectY) + "\"";

    // Output file
    cmd1 += " -y " + videoStreamPath();

    //
    // Assemble the command line arguments for the audio encoder
    //

    // Console interactions
    string cmd2 = " -nostdin";

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

    assert(!videoFFmpeg.isRunning());
    assert(!audioFFmpeg.isRunning());

    // Launch the video encoder
    debug(REC_DEBUG, "\nLaunching video encoder with options:\n");
    debug(REC_DEBUG, "%s\n", cmd1.c_str());

    if (!videoFFmpeg.launch(cmd1)) {
        throw VC64Error(ERROR_REC_LAUNCH, "Unable to launch the FFmpeg video encoder.");
    }

    // Launch the audio encoder
    debug(REC_DEBUG, "\nLaunching audio encoder with options:\n");
    debug(REC_DEBUG, "%s\n", cmd2.c_str());

    if (!audioFFmpeg.launch(cmd2)) {
        throw VC64Error(ERROR_REC_LAUNCH, "Unable to launch the FFmpeg audio encoder.");
    }

    // Open the video pipe
    debug(REC_DEBUG, "Opening video pipe\n");

    if (!videoPipe.open()) {
        throw VC64Error(ERROR_REC_LAUNCH, "Unable to open the video pipe.");
    }

    // Open the audio pipe
    debug(REC_DEBUG, "Opening audio pipe\n");

    if (!audioPipe.open()) {
        throw VC64Error(ERROR_REC_LAUNCH, "Unable to launch the audio pipe.");
    }

    debug(REC_DEBUG, "Success\n");
    state = State::prepare;
}

void
Recorder::stopRecording()
{
    debug(REC_DEBUG, "stopRecording()\n");

    {   SYNCHRONIZED

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

    // Verbosity
    string cmd = "-loglevel " + loglevel();

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

    debug(REC_DEBUG, "\nMerging streams with options:\n%s\n", cmd.c_str());

    FFmpeg merger;
    if (!merger.launch(cmd)) {
        warn("Failed to merge video and audio: %s\n", cmd.c_str());
    }
    merger.join();

    debug(REC_DEBUG, "Success\n");
    return true;
}

void
Recorder::vsyncHandler()
{
    // Quick-exit if the recorder is not active
    if (state == State::wait) return;
    
    {   SYNCHRONIZED
        
        switch (state) {
                
            case State::wait: break;
            case State::prepare: prepare(); break;
            case State::record: record(); break;
            case State::finalize: finalize(); break;
            case State::abort: abort(); break;
        }
    }
}

void
Recorder::prepare()
{
    debug(REC_DEBUG, "Prepare\n");
    
    /* Adjust the sampling rate while the recorder is running.
     *
     * Background: Both the PAL and NTSC versions of the C64 diverge from the
     * exact PAL and NTSC frame rates. The PAL C64 frame rate is 50.125 Hz,
     * derived from a system clock frequency of 0.985248 MHz whereas the NTSC
     * frame rate is 59.826 Hz, derived from a clock frequency of 1.023 MHz.
     * Because we record a 50 Hz video stream for PAL machines and a 60 Hz
     * stream for NTSC machines, we need to scale the sample frequency to make
     * SID produce the correct number of sound samples per frame (882 for PAL
     * and 735 for NTSC).
     * Important: Due to scaling, we won't be able to make SID produce the exact
     * number of required samples in each and every frame. It is important to
     * apply a scaling factor that never makes SID produce less than the
     * required amout which would result in a buffer underflow.
     */
    if (vic.pal()) {
        muxer.setSampleRate(sampleRate * 50.125 / 50.0);
        samplesPerFrame = 882;
    } else {
        muxer.setSampleRate(sampleRate * 59.827 / 60.0);
        samplesPerFrame = 735;
    }
    
    // Start with a nearly empty buffer
    muxer.stream.lock();
    while (muxer.stream.count() > 1) muxer.stream.read();
    muxer.stream.unlock();

    // Switch state and inform the GUI
    state = State::record;
    recStart = util::Time::now();
    msgQueue.put(MSG_RECORDING_STARTED);
}

void
Recorder::record()
{
    assert(videoFFmpeg.isRunning());
    assert(audioFFmpeg.isRunning());
    assert(videoPipe.isOpen());
    assert(audioPipe.isOpen());
    
    recordVideo();
    recordAudio();
}

void
Recorder::recordVideo()
{
    u32 *texture = (u32 *)vic.stableEmuTexture();
    
    isize width = sizeof(u32) * (cutout.x2 - cutout.x1);
    isize height = cutout.y2 - cutout.y1;
    isize offset = cutout.y1 * TEX_WIDTH + cutout.x1;

    u8 *data = new u8[width * height];
    u8 *src = (u8 *)(texture + offset);
    u8 *dst = data;
    
    for (isize y = 0; y < height; y++, src += 4 * TEX_WIDTH, dst += width) {
        std::memcpy(dst, src, width);
    }
    
    // Feed the video pipe
    isize length = width * height;
    isize written = videoPipe.write((u8 *)data, length);

    if (written != length || FORCE_RECORDING_ERROR) {
        state = State::abort;
    }
}

void
Recorder::recordAudio()
{
    if (muxer.stream.count() != samplesPerFrame) {
        
        trace(REC_DEBUG, "Samples: %zd\n", muxer.stream.count());
        assert(muxer.stream.count() >= samplesPerFrame);
    }
    
    // Feed the audio pipe
    for (isize i = 0; i < samplesPerFrame; i++) {

        isize written = 0;

        SamplePair pair = muxer.stream.read();
        written += audioPipe.write((u8 *)&pair.left, sizeof(float));
        written += audioPipe.write((u8 *)&pair.right, sizeof(float));

        if (written != 2 * sizeof(float) || FORCE_RECORDING_ERROR) {
            state = State::abort;
        }
    }
    
    muxer.stream.clear();
}

void
Recorder::finalize()
{
    // Close pipes
    videoPipe.close();
    audioPipe.close();

    // Wait for the decoders to terminate
    videoFFmpeg.join();
    audioFFmpeg.join();

    // Switch state and inform the GUI
    state = State::wait;
    recStop = util::Time::now();
    msgQueue.put(MSG_RECORDING_STOPPED);
}

/*
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
    recStop = util::Time::now();
    msgQueue.put(MSG_RECORDING_STOPPED);
}
*/

void
Recorder::abort()
{
    finalize();
    msgQueue.put(MSG_RECORDING_ABORTED);
}
