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
#include "Recorder.h"
#include "C64.h"
#include "Host.h"

namespace vc64 {

Recorder::Recorder(C64& ref) : SubComponent(ref)
{

}

void
Recorder::_initialize()
{
    FFmpeg::init();
}

void
Recorder::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("FFmpeg path");
        os << FFmpeg::getExecPath() << std::endl;
        os << tab("Available");
        os << bol(FFmpeg::available()) << std::endl;
        os << tab("Recorder state");
        os << bol(RecStateEnum::key(state)) << std::endl;
        os << tab("Duration");
        os << flt(getDuration().asSeconds()) << std::endl;
    }
}

i64
Recorder::getOption(Option option) const
{
    switch (option) {

        case OPT_REC_FRAME_RATE:    return config.frameRate;
        case OPT_REC_BIT_RATE:      return config.bitRate;
        case OPT_REC_SAMPLE_RATE:   return config.sampleRate;
        case OPT_REC_ASPECT_X:      return config.aspectRatio.x;
        case OPT_REC_ASPECT_Y:      return config.aspectRatio.y;

        default:
            fatalError;
    }
}

void
Recorder::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_REC_FRAME_RATE:
        case OPT_REC_BIT_RATE:
        case OPT_REC_SAMPLE_RATE:
        case OPT_REC_ASPECT_X:
        case OPT_REC_ASPECT_Y:

            return;

        default:
            throw Error(VC64ERROR_OPT_UNSUPPORTED);
    }
}

void
Recorder::setOption(Option opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case OPT_REC_FRAME_RATE:

            config.frameRate = isize(value);
            return;

        case OPT_REC_BIT_RATE:

            config.bitRate = isize(value);
            return;

        case OPT_REC_SAMPLE_RATE:

            config.sampleRate = isize(value);
            return;

        case OPT_REC_ASPECT_X:

            config.aspectRatio.x = isize(value);
            return;

        case OPT_REC_ASPECT_Y:

            config.aspectRatio.y = isize(value);
            return;

        default:
            fatalError;
    }
}

void
Recorder::cacheInfo(RecorderInfo &result) const
{
    {   SYNCHRONIZED

        result.available = FFmpeg::available();
        result.state = state;
        result.duration = getDuration().asSeconds();
    }
}

string
Recorder::videoPipePath()
{
    return host.tmp("videoPipe").string();
}

string
Recorder::audioPipePath()
{
    return host.tmp("audioPipe").string();
}

string
Recorder::videoStreamPath()
{
    return host.tmp("video.mp4").string();
}

string
Recorder::audioStreamPath()
{
    return host.tmp("audio.mp4").string();
}

util::Time
Recorder::getDuration() const
{
    return (isRecording() ? util::Time::now() : recStop) - recStart;
}

void
Recorder::startRecording(isize x1, isize y1, isize x2, isize y2)
{
    SYNCHRONIZED

    // Override the frameRate by now (remove this later)
    config.frameRate = vic.pal() ? 50 : 60;
    samplesPerFrame = config.sampleRate / config.frameRate;

    debug(REC_DEBUG, "startRecording(%ld,%ld,%ld,%ld)\n", x1, y1, x2, y2);
    debug(REC_DEBUG, "%ld,%ld,%ld,\n", config.bitRate, config.aspectRatio.x, config.aspectRatio.y);

    // Print some debugging information if requested
    if (REC_DEBUG) dump(Category::State);

    if (isRecording()) {
        throw Error(VC64ERROR_REC_LAUNCH, "Recording in progress.");
    }

    // Create pipes
    debug(REC_DEBUG, "Creating pipes...\n");

    if (!videoPipe.create(videoPipePath())) {
        throw Error(VC64ERROR_REC_LAUNCH, "Failed to create the video encoder pipe.");
    }
    if (!audioPipe.create(audioPipePath())) {
        throw Error(VC64ERROR_REC_LAUNCH, "Failed to create the video encoder pipe.");
    }

    debug(REC_DEBUG, "Pipes created\n");

    // Make sure the screen dimensions are even
    if ((x2 - x1) % 2) x2--;
    if ((y2 - y1) % 2) y2--;

    // Remember the cutout
    cutout.x1 = x1;
    cutout.x2 = x2;
    cutout.y1 = y1;
    cutout.y2 = y2;
    debug(REC_DEBUG, "Recorded area: (%ld,%ld) - (%ld,%ld)\n", x1, y1, x2, y2);


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
    cmd1 += " -r " + std::to_string(config.frameRate);

    // Frame size (width x height)
    cmd1 += " -s:v " + std::to_string(x2 - x1) + "x" + std::to_string(y2 - y1);

    // Input source (named pipe)
    cmd1 += " -i " + videoPipePath();

    // Output stream format
    cmd1 += " -f mp4 -pix_fmt yuv420p";

    // Bit rate
    cmd1 += " -b:v " + std::to_string(config.bitRate) + "k";

    // Aspect ratio
    cmd1 += " -bsf:v ";
    cmd1 += "\"h264_metadata=sample_aspect_ratio=";
    cmd1 += std::to_string(config.aspectRatio.x) + "/";
    cmd1 += std::to_string(config.aspectRatio.y) + "\"";

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
    cmd2 += " -sample_rate " + std::to_string(config.sampleRate);

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
        throw Error(VC64ERROR_REC_LAUNCH, "Unable to launch the FFmpeg video encoder.");
    }

    // Launch the audio encoder
    debug(REC_DEBUG, "\nLaunching audio encoder with options:\n");
    debug(REC_DEBUG, "%s\n", cmd2.c_str());

    if (!audioFFmpeg.launch(cmd2)) {
        throw Error(VC64ERROR_REC_LAUNCH, "Unable to launch the FFmpeg audio encoder.");
    }

    // Open the video pipe
    debug(REC_DEBUG, "Opening video pipe\n");

    if (!videoPipe.open()) {
        throw Error(VC64ERROR_REC_LAUNCH, "Unable to open the video pipe.");
    }

    // Open the audio pipe
    debug(REC_DEBUG, "Opening audio pipe\n");

    if (!audioPipe.open()) {
        throw Error(VC64ERROR_REC_LAUNCH, "Unable to launch the audio pipe.");
    }

    debug(REC_DEBUG, "Success\n");
    state = REC_STATE_PREPARE;
}

void
Recorder::stopRecording()
{
    debug(REC_DEBUG, "stopRecording()\n");

    {   SYNCHRONIZED

        if (isRecording()) {
            state = REC_STATE_FINALIZE;
        }
    }
}

bool
Recorder::exportAs(const fs::path &path)
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
    cmd += " -y " + path.string();

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
    if (state == REC_STATE_WAIT) return;

    {   SYNCHRONIZED
        
        switch (state) {
                
            case REC_STATE_WAIT:     break;
            case REC_STATE_PREPARE:  prepare(); break;
            case REC_STATE_RECORD:   recordVideo(); recordAudio(); break;
            case REC_STATE_FINALIZE: finalize(); break;
            case REC_STATE_ABORT:    abort(); break;
        }
    }
}

void
Recorder::prepare()
{
    debug(REC_DEBUG, "Prepare\n");
    
    /* Adjust the sampling rate.
     *
     * Background: Both the PAL and NTSC versions of the C64 diverge from the
     * exact PAL and NTSC frame rates. The PAL C64 frame rate is 50.125 Hz,
     * derived from a system clock frequency of 0.985248 MHz whereas the NTSC
     * frame rate is 59.826 Hz, derived from a clock frequency of 1.023 MHz.
     * Because we record a 50 Hz video stream for PAL machines and a 60 Hz
     * stream for NTSC machines, we need to scale the sample frequency to make
     * SID produce the correct number of sound samples per frame (882 for PAL
     * and 735 for NTSC).
     */
    if (vic.pal()) {
        host.setOption(OPT_HOST_SAMPLE_RATE, i64(config.sampleRate * 50.125 / 50.0));
        samplesPerFrame = 882;
    } else {
        host.setOption(OPT_HOST_SAMPLE_RATE, i64(config.sampleRate * 59.827 / 60.0));
        samplesPerFrame = 735;
    }
    
    // Start with a nearly empty buffer
    audioPort.clamp(1);

    // Switch state and inform the GUI
    state = REC_STATE_RECORD;
    recStart = util::Time::now();
    msgQueue.put(MSG_RECORDING_STARTED);
}

void
Recorder::recordVideo()
{
    assert(videoFFmpeg.isRunning());
    assert(videoPipe.isOpen());

    u32 *texture = (u32 *)videoPort.getTexture();
    
    isize width = sizeof(u32) * (cutout.x2 - cutout.x1);
    isize height = cutout.y2 - cutout.y1;
    isize offset = cutout.y1 * Texture::width + cutout.x1;

    u8 *data = new u8[width * height];
    u8 *src = (u8 *)(texture + offset);
    u8 *dst = data;
    
    for (isize y = 0; y < height; y++, src += 4 * Texture::width, dst += width) {
        std::memcpy(dst, src, width);
    }
    
    // Feed the video pipe
    isize length = width * height;
    isize written = videoPipe.write((u8 *)data, length);

    if (written != length || FORCE_RECORDING_ERROR) {
        state = REC_STATE_ABORT;
    }
}

void
Recorder::recordAudio()
{
    assert(audioFFmpeg.isRunning());
    assert(audioPipe.isOpen());

    if (audioPort.count() != samplesPerFrame) {

        // trace(REC_DEBUG, "Samples: %ld (expected: %ld)\n", audioPort.count(), samplesPerFrame);
        assert(audioPort.count() >= samplesPerFrame);
    }
    
    // Feed the audio pipe
    for (isize i = 0; i < samplesPerFrame; i++) {

        isize written = 0;

        SamplePair pair = audioPort.read();
        written += audioPipe.write((u8 *)&pair.l, sizeof(float));
        written += audioPipe.write((u8 *)&pair.r, sizeof(float));

        if (written != 2 * sizeof(float) || FORCE_RECORDING_ERROR) {
            state = REC_STATE_ABORT;
        }
    }
    
    audioPort.clear();
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
    state = REC_STATE_WAIT;
    recStop = util::Time::now();
    msgQueue.put(MSG_RECORDING_STOPPED);
}

void
Recorder::abort()
{
    finalize();
    msgQueue.put(MSG_RECORDING_ABORTED);
}

}
