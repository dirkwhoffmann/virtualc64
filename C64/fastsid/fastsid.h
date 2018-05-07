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

#include "FastSidVoice.h"



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
