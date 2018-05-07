/*
 * This file belongs to the FastSID implementation of VirtualC64,
 * an adaption of the code used in VICE 3.1, the Versatile Commodore Emulator.
 *
 * Original code written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *
 * Adapted for VirtualC64 by
 *  Dirk Hoffmann
 */
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#ifndef FASTSIDVOICE_H
#define FASTSIDVOICE_H

#include "VirtualComponent.h"

// ADSR state (Attack, Decay, Sustain, Release)
#define FASTSID_ATTACK   0
#define FASTSID_DECAY    1
#define FASTSID_SUSTAIN  2
#define FASTSID_RELEASE  3
#define FASTSID_IDLE     4

// Noise magic
#define NSHIFT(v, n) \
(((v) << (n))    \
| ((((v) >> (23 - (n))) ^ (v >> (18 - (n)))) & ((1 << (n)) - 1)))

#define NVALUE(v)                                   \
(noiseLSB[v & 0xff] | noiseMID[(v >> 8) & 0xff] \
| noiseMSB[(v >> 16) & 0xff])

#define NSEED 0x7ffff8


typedef struct voice_s {
    
    // TODO: REPLACE BY REFERENCE TO FastSID class
    struct sound_s *s;
    
    /* counter value */
    uint32_t f;
    
    /* counter step / sample */
    uint32_t fs;
    
    /* waveform that we use */
    uint8_t fm;
    
    /* pulse threshold compared to the 32-bit counter */
    uint32_t pw;
    
    /* 31-bit adsr counter */
    uint32_t adsr;
    
    /* adsr counter step / sample */
    int32_t adsrs;
    
    /* adsr sustain level compared to the 31-bit counter */
    uint32_t adsrz;
    
    /* does this voice use hard sync? */
    uint8_t sync;
    
    /* does this voice use filter? */
    uint8_t filter;
    
    /* does this structure need updating before next sample? */
    uint8_t update;
    /* did we do multiple gate flips after last calculated sample? */
    uint8_t gateflip;
    
    /* noise shift register. Note! rv may be 0 to 15 shifts 'behind' the
     real noise shift register value. Remaining shifts are done when
     it is referenced */
    uint32_t rv;
    
    /* pointer to wavetable data */
    uint16_t *wt;
    /* 32-bit offset to add to the counter before referencing the wavetable.
     This is used on combined waveforms, when other waveforms are combined
     with pulse */
    uint32_t wtpf;
    /* length of wavetable (actually number of shifts needed for 32-bit
     counter) */
    uint32_t wtl;
    /* kludge for ring modulation. Set wtr[1] = 0x7fff if ring modulation is
     used */
    uint16_t wtr[2];
    
    signed char filtIO;
    float filtLow, filtRef;
} voice_t;

class Voice : public VirtualComponent {
    
private:
    
    //! @brief   Wave tables
    //! @details The first index determines the chip model (0 = old, 1 = new).
    static uint16_t wavetable00[2][2];
    static uint16_t wavetable10[2][4096];
    static uint16_t wavetable20[2][4096];
    static uint16_t wavetable30[2][4096];
    static uint16_t wavetable40[2][8192];
    static uint16_t wavetable50[2][8192];
    static uint16_t wavetable60[2][8192];
    static uint16_t wavetable70[2][8192];
    
    //! @brief   Noise tables
    static uint8_t noiseMSB[256];
    static uint8_t noiseMID[256];
    static uint8_t noiseLSB[256];
    
public:
    
    //! @brief   The SID voice which is represented by this object (1,2, or 3)
    uint8_t nr;
    
    //! @brief   Pointer to SID registers controlling this voice
    uint8_t *sidreg;
    
    //! @brief   Do we have noise enabled?
    bool noise;
    
    //! @brief   Current envelope phase (ATTACK, DECAY, SUSTAIN, RELEASE, or IDLE)
    uint8_t adsrm;
    
    // Voice data (move inside class later)
    voice_t vt;
    
    // 15-bit oscillator value
    uint32_t doosc();
    
    //! @brief    Initializes the wave tables
    /*! @details  This static method needs to be called before using the class.
     */
    static void initWaveTables();

    //! @brief    Initialize
    void init(sound_s *psid, unsigned voiceNr);

    //! @brief    Update internal parameters
    void setup(unsigned chipModel);
    
    //! @brief  Change ADSR state and all related variables
    void set_adsr(uint8_t fm);
    
    //! @brief ADSR counter triggered state change
    void trigger_adsr();
    
    //
    // Querying the current configuration
    //
    
    //! @brief   Returns the currently set oscillator frequency owing equation:
    uint16_t frequency() { return HI_LO(sidreg[0x01], sidreg[0x00]); }
    
    //! @brief   Returns the attack rate for the envelope generator
    /*! @details The attack rate is a 4 bit value which determines how rapidly
     *           the output of the voice rises from zero to peak amplitude when
     *           the envelope generator is gated.
     */
    uint8_t attackRate() { return sidreg[0x05] >> 4; }

    //! @brief   Returns the decay rate for the envelope generator
    /*! @details The decay cycle follows the attack cycle and the decay rate
     *           determines how rapidly the output falls from the peak amplitude
     *           to the selected sustain level.
     */
    uint8_t decayRate() { return sidreg[0x05] & 0x0F; }

    //! @brief   Returns the decay rate for the envelope generator
    /*! @details The sustain cycle follows the decay cycle and the output of
     *           the voice will remain at the selected sustain amplitude as
     *           long as the gate bit remains set. The sustain levels range
     *           from zero to peak amplitude in 16 linear steps, with a sustain
     *           value of 0 selecting zero amplitude and a sustain value of 15
     *           selecting the peak amplitude.
     *           A sustain value of 8 would cause the voice to sustain at an
     *           amplitude one-half the peak amplitude reached by the attack
     *           cycle.
     */
    uint8_t sustainRate() { return sidreg[0x06] >> 4; }

    //! @brief   Returns the release rate for the envelope generator
    /*! @details The release cycle follows the sustain cycle when the Gate bit is
     *           reset to zero. At this time, the output of Voice 1 will fall
     *           from the sustain amplitude to zero amplitude at the selected
     *           release rate. The 16 release rates are identical to the decay
     *           rates.
     */
    uint8_t releaseRate() { return sidreg[0x06] & 0x0F; }
};

#endif
