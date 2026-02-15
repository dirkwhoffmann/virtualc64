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

#include "AudioPortTypes.h"
#include "SIDTypes.h"
#include "SubComponent.h"
#include "Animated.h"
#include "Utilities/Animated.h" // DEPRECATED
#include "AudioStream.h"
#include "SampleRateDetector.h"

namespace vc64 {

class SIDBridge;

class AudioPort final : public SubComponent, public Inspectable<AudioPortInfo, AudioPortStats> {

    friend class SIDBridge;
    
    Descriptions descriptions = {{

        .type           = Class::AudioPort,
        .name           = "Audio",
        .description    = "Audio Port",
        .shell          = "audio"
    }};

    Options options = {

        Opt::AUD_VOL0,
        Opt::AUD_VOL1,
        Opt::AUD_VOL2,
        Opt::AUD_VOL3,
        Opt::AUD_PAN0,
        Opt::AUD_PAN1,
        Opt::AUD_PAN2,
        Opt::AUD_PAN3,
        Opt::AUD_VOL_L,
        Opt::AUD_VOL_R,
        Opt::AUD_BUFFER_SIZE,
        Opt::AUD_ASR,
    };

    // Current configuration
    AudioPortConfig config = { };

    // Current sample rate
    double sampleRate = 44100.0;

    // Variables needed to implement ASR (Adaptive Sample Rate)
    double sampleRateError = 0.0;
    double sampleRateCorrection = 0.0;

    // Time stamp of the last write pointer alignment
    utl::Time lastAlignment = utl::Time::now();

    // Channel volumes
    float vol[4] = { };

    // Panning factors
    float pan[4] = { };

    // Master volumes (fadable)
    util::Animated<float> volL;
    util::Animated<float> volR;

    // Used to determine if Msg::MUTE should be send
    bool wasMuted = false;


    //
    // Subcomponents
    //

public:

    // Output buffer
    AudioStream stream = AudioStream(4096);

    // Detector for measuring the sample rate
    SampleRateDetector detector = SampleRateDetector(c64);


    //
    // Methods
    //

public:

    AudioPort(C64 &ref) : SubComponent(ref) { };

    AudioPort& operator= (const AudioPort& other) {

        CLONE(config)
        CLONE_ARRAY(pan)
        CLONE_ARRAY(vol)
        CLONE(volL)
        CLONE(volR)

        return *this;
    }

    // Resets the output buffer
    void clear();


    //
    // Methods from Serializable
    //

public:
    
    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << config.vol
        << config.pan
        << config.volL
        << config.volR;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream &os) const override;
    void _didLoad() override;
    void _didReset(bool hard) override;
    void _powerOn() override;
    void _run() override;
    void _pause() override;
    void _warpOn() override;
    void _warpOff() override;
    void _focus() override;
    void _unfocus() override;


    //
    // Methods from Configurable
    //

public:

    const AudioPortConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Opt opt) const override;
    void checkOption(Opt opt, i64 value) override;
    void setOption(Opt opt, i64 value) override;

    void setSampleRate(double hz);


    //
    // Methods from Inspectable
    //

public:

    void cacheInfo(AudioPortInfo &result) const override;
    void cacheStats(AudioPortStats &result) const override;


    //
    // Analyzing
    //

private:

    // Returns true if the output volume is zero
    bool isMuted() const;


    //
    // Generating audio samples
    //

public:

    // Generates samples
    void generateSamples();

    // Returns the sample rate adjustment
    double getSampleRateCorrection() { return sampleRateCorrection; }

private:

    // Runs the ASR algorithms (adaptive sample rate)
    void updateSampleRateCorrection();

    // Generates samples from the audio source with a single active SID
    template <bool fading> void mixSingleSID(isize numSamples);

    // Generates samples from the audio source with multiple active SIDs
    template <bool fading> void mixMultiSID(isize numSamples);

    // Handles a buffer underflow or overflow condition
    void handleBufferUnderflow();
    void handleBufferOverflow();


    //
    // Controlling volume
    //

public:

    // Rescale the existing samples to gradually fade out (to avoid cracks)
    void eliminateCracks();

    // Gradually decrease the master volume to zero
    void mute(isize steps = 0) { volL.fadeOut(steps); volR.fadeOut(steps); }

    // Gradually inrease the master volume to max
    void unmute(isize steps = 0) { volL.fadeIn(steps); volR.fadeIn(steps); }


    //
    // Reading audio samples
    //
    
public:
    
    /* Copies n audio samples into a memory buffer. These functions mark the
     * final step in the audio pipeline. They are used to copy the generated
     * sound samples into the buffers of the native sound device. The function
     * returns the number of copied samples.
     */
    isize copyMono(float *buffer, isize n);
    isize copyStereo(float *left, float *right, isize n);
    isize copyInterleaved(float *buffer, isize n);
};

}
