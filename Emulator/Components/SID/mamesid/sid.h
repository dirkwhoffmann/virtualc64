// license:BSD-3-Clause
// copyright-holders:Peter Trauner
#ifndef MAME_SOUND_SID_H
#define MAME_SOUND_SID_H

#pragma once

/*
 approximation of the sid6581 chip
 this part is for one chip,
 */

#include "sidvoice.h"

// TODO: UNKNOWN SYMBOLS
typedef int *device_t;
typedef int *sound_stream;

/* private area */
struct SID6581_t
{
    static constexpr uint8_t max_voices = 3;

    device_t *device = nullptr;
    sound_stream *mixer_channel = nullptr; // mame stream/ mixer channel

    int type = 0;
    int sample_rate = 0; 
    uint32_t clock = 0;

    uint16_t PCMfreq = 0; // samplerate of the current systems soundcard/DAC
    uint32_t PCMsid = 0, PCMsidNoise = 0;

    int reg[0x20]{ 0 };

    uint8_t masterVolume = 0;
    uint16_t masterVolumeAmplIndex = 0;

    struct
    {
        int Enabled = 0;
        uint8_t Type = 0, CurType = 0;
        float Dy = 0.0, ResDy = 0.0;
        uint16_t Value = 0;
    } filter;

    sidOperator optr[max_voices];
    int optr3_outputmask = 0;

    void init();

    bool reset();

    void postload();

    // int port_r(running_machine &machine, int offset);
    int port_r(int offset);
    void port_w(int offset, int data);

    // void fill_buffer(write_stream_view &buffer);
    void fill_buffer(short *buffer, int samples);

private:
    void syncEm();
};

#endif // MAME_SOUND_SID_H
