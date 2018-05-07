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

#ifndef FASTSID_H
#define FASTSID_H

#include "fastsid.h"

typedef struct voice_s {
    
    struct sound_s      *s;
    struct voice_s      *vprev;
    struct voice_s      *vnext;
    int nr;
    
    /* counter value */
    uint32_t f;
    
    /* counter step / sample */
    uint32_t fs;

    /* do we have noise enabled? */
    uint8_t noise;
    
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
    
    /* ADSR mode */
    uint8_t adsrm;
    /* 4-bit attack value */
    uint8_t attack;
    /* 4-bit decay value */
    uint8_t decay;
    /* 4-bit sustain value */
    uint8_t sustain;
    /* 4-bit release value */
    uint8_t release;
    
    /* pointer to registers of this voice */
    uint8_t *d;
    
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

class Voice {
 
private:
    
    //! @brief  Pointer to next voice (1 -> 2 -> 3 -> 1)
    Voice *next;

    //! @brief  Pointer to previous voice (1 -> 3 -> 2 -> 1)
    Voice *prev;

public:
    
    // Voice data (move inside class later)
    voice_t vt;
    
    // 15-bit oscillator value
    uint32_t doosc();
    
    //! @brief  Update internal parameters
    void setup();
    
    //! @brief  Change ADSR state and all related variables
    void set_adsr(uint8_t fm);
    
    //! @brief ADSR counter triggered state change
    void trigger_adsr();
    
    
};

/* needed data for SID */
struct sound_s {
    /* speed factor */
    int factor;
    
    /* number of voices */
    Voice v[3];
    
    /* SID registers */
    uint8_t d[32];
    /* is voice 3 enabled? */
    uint8_t has3;
    /* 4-bit volume value */
    uint8_t vol;
    
    /* ADSR counter step values for each adsr values */
    int32_t adrs[16];
    /* sustain values compared to 31-bit ADSR counter */
    uint32_t sz[16];
    
    /* internal constant used for sample rate dependent calculations */
    uint32_t speed1;
    
    /* does this structure need updating before next sample? */
    uint8_t update;

    /* do we have a new sid or an old one? */
    uint8_t newsid;

    /* constants needed to implement write-only register reads */
    uint8_t laststore;
    uint8_t laststorebit;
    uint64_t laststoreclk;
    /* do we want to use filters? */
    int emulatefilter;
    
    /* filter variables */
    float filterDy;
    float filterResDy;
    uint8_t filterType;
    uint8_t filterCurType;
    uint16_t filterValue;
};

typedef struct sound_s sound_t;


int fastsid_init(sound_t *psid, int speed, int cycles_per_sec, int factor);
uint8_t fastsid_read(sound_t *psid, uint16_t addr);
void fastsid_store(sound_t *psid, uint16_t addr, uint8_t byte);
int fastsid_calculate_samples(sound_t *psid, int16_t *pbuf, int nr,
                              int interleave, int *delta_t);

/*
extern void fastsid_state_read(struct sound_s *psid, struct sid_fastsid_snapshot_state_s *sid_state);
extern void fastsid_state_write(struct sound_s *psid, struct sid_fastsid_snapshot_state_s *sid_state);
*/

#endif
