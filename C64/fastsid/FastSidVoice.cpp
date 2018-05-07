/*
 * This file belongs to the FastSID implementation of VirtualC64,
 * an adaption of the code used in VICE 3.1, the Versatile Commodore Emulator.
 *
 * Original code written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Michael Schwendt <sidplay@geocities.com>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "FastSid.h"

// Table for pseudo-exponential ADSR calculations
static uint32_t exptable[6] =
{
    0x30000000, 0x1c000000, 0x0e000000, 0x08000000, 0x04000000, 0x00000000
};

uint32_t
Voice::doosc()
{
    uint32_t result;
    
    if (vt.noise) {
        result = ((uint32_t)NVALUE(NSHIFT(vt.rv, vt.f >> 28))) << 7;
    } else {
        result = vt.wt[(vt.f + vt.wtpf) >> vt.wtl] ^ vt.wtr[vt.vprev->f >> 31];
    }
    
    return result;
}

void
Voice::setup()
{
    if (!vt.update) {
        return;
    }
    
    vt.attack = vt.d[5] / 0x10;
    vt.decay = vt.d[5] & 0x0f;
    vt.sustain = vt.d[6] / 0x10;
    vt.release = vt.d[6] & 0x0f;
    vt.sync = vt.d[4] & 0x02 ? 1 : 0;
    vt.fs = vt.s->speed1 * (vt.d[0] + vt.d[1] * 0x100);
    
    if (vt.d[4] & 0x08) {
        vt.f = vt.fs = 0;
        vt.rv = NSEED;
    }
    vt.noise = 0;
    vt.wtl = 20;
    vt.wtpf = 0;
    vt.wtr[1] = 0;
    
    switch ((vt.d[4] & 0xf0) >> 4) {
        case 0:
            vt.wt = wavetable00;
            vt.wtl = 31;
            break;
        case 1:
            vt.wt = wavetable10;
            if (vt.d[4] & 0x04) {
                vt.wtr[1] = 0x7fff;
            }
            break;
        case 2:
            vt.wt = wavetable20;
            break;
        case 3:
            vt.wt = wavetable30;
            if (vt.d[4] & 0x04) {
                vt.wtr[1] = 0x7fff;
            }
            break;
        case 4:
            if (vt.d[4] & 0x08) {
                vt.wt = &wavetable40[4096];
            } else {
                vt.wt = &wavetable40[4096 - (vt.d[2]
                                             + (vt.d[3] & 0x0f) * 0x100)];
            }
            break;
        case 5:
            vt.wt = &wavetable50[vt.wtpf = 4096 - (vt.d[2]
                                                   + (vt.d[3] & 0x0f) * 0x100)];
            vt.wtpf <<= 20;
            if (vt.d[4] & 0x04) {
                vt.wtr[1] = 0x7fff;
            }
            break;
        case 6:
            vt.wt = &wavetable60[vt.wtpf = 4096 - (vt.d[2]
                                                   + (vt.d[3] & 0x0f) * 0x100)];
            vt.wtpf <<= 20;
            break;
        case 7:
            vt.wt = &wavetable70[vt.wtpf = 4096 - (vt.d[2]
                                                   + (vt.d[3] & 0x0f) * 0x100)];
            vt.wtpf <<= 20;
            if (vt.d[4] & 0x04 && vt.s->newsid) {
                vt.wtr[1] = 0x7fff;
            }
            break;
        case 8:
            vt.noise = 1;
            vt.wt = NULL;
            vt.wtl = 0;
            break;
        default:
            /* XXX: noise locking correct? */
            vt.rv = 0;
            vt.wt = wavetable00;
            vt.wtl = 31;
    }
    
    switch (vt.adsrm) {
        case FASTSID_ATTACK:
        case FASTSID_DECAY:
        case FASTSID_SUSTAIN:
            if (vt.d[4] & 0x01) {
                set_adsr((uint8_t)(vt.gateflip ? FASTSID_ATTACK : vt.adsrm));
            } else {
                set_adsr(FASTSID_RELEASE);
            }
            break;
        case FASTSID_RELEASE:
        case FASTSID_IDLE:
            if (vt.d[4] & 0x01) {
                set_adsr(FASTSID_ATTACK);
            } else {
                set_adsr(vt.adsrm);
            }
            break;
    }
    vt.update = 0;
    vt.gateflip = 0;
}

void
Voice::set_adsr(uint8_t fm)
{
    int i;
    
    switch (fm) {
        case FASTSID_ATTACK:
            vt.adsrs = vt.s->adrs[vt.attack];
            vt.adsrz = 0;
            break;
        case FASTSID_DECAY:
            /* XXX: fix this */
            if (vt.adsr <= vt.s->sz[vt.sustain]) {
                set_adsr(FASTSID_SUSTAIN);
                return;
            }
            for (i = 0; vt.adsr < exptable[i]; i++) {}
            vt.adsrs = -vt.s->adrs[vt.decay] >> i;
            vt.adsrz = vt.s->sz[vt.sustain];
            if (exptable[i] > vt.adsrz) {
                vt.adsrz = exptable[i];
            }
            break;
        case FASTSID_SUSTAIN:
            if (vt.adsr > vt.s->sz[vt.sustain]) {
                set_adsr(FASTSID_DECAY);
                return;
            }
            vt.adsrs = 0;
            vt.adsrz = 0;
            break;
        case FASTSID_RELEASE:
            if (!vt.adsr) {
                set_adsr(FASTSID_IDLE);
                return;
            }
            for (i = 0; vt.adsr < exptable[i]; i++) {}
            vt.adsrs = -vt.s->adrs[vt.release] >> i;
            vt.adsrz = exptable[i];
            break;
        case FASTSID_IDLE:
            vt.adsrs = 0;
            vt.adsrz = 0;
            break;
    }
    vt.adsrm = fm;
}

void
Voice::trigger_adsr()
{
    switch (vt.adsrm) {
        case FASTSID_ATTACK:
            vt.adsr = 0x7fffffff;
            set_adsr(FASTSID_DECAY);
            break;
        case FASTSID_DECAY:
        case FASTSID_RELEASE:
            if (vt.adsr >= 0x80000000) {
                vt.adsr = 0;
            }
            set_adsr(vt.adsrm);
            break;
    }
}
