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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "FastSid.h"





// Clockcycles for each dropping bit when write-only register read is done
uint32_t sidreadclocks[9];

float lowPassParam[0x800];
float bandPassParam[0x800];
float filterResTable[16];
const float filterRefFreq = 44100.0;
signed char ampMod1x8[256];



/* manage temporary buffers. if the requested size is smaller or equal to the
 * size of the already allocated buffer, reuse it.  */
int16_t *buf = NULL;
int blen = 0;








void init_filter(sound_t *psid, int freq)
{
    uint16_t uk;
    float rk;
    long int si;

    float yMax = 1.0;
    float yMin = (float)0.01;
    float resDyMax = 1.0;
    float resDyMin = 2.0;
    float resDy = resDyMin;

    float yAdd, yTmp;

    float filterFs = 400.0;
    float filterFm = 60.0;
    float filterFt = (float)0.05;

    float filterAmpl = 1.0;

    psid->filterValue = 0;
    psid->filterType = 0;
    psid->filterCurType = 0;
    psid->filterDy = 0;
    psid->filterResDy = 0;

    for (uk = 0, rk = 0; rk < 0x800; rk++, uk++) {
        float h;

        h = (float)((((exp(rk / 2048 * log(filterFs)) / filterFm) + filterFt) * filterRefFreq) / freq);
        if (h < yMin) {
            h = yMin;
        }
        if (h > yMax) {
            h = yMax;
        }
        lowPassParam[uk] = h;
    }

    yMax = (float)0.22;
    yMin = (float)0.002;
    yAdd = (float)((yMax - yMin) / 2048.0);
    yTmp = yMin;

    for (uk = 0, rk = 0; rk < 0x800; rk++, uk++) {
        bandPassParam[uk] = (yTmp * filterRefFreq) / freq;
        yTmp += yAdd;
    }

    for (uk = 0; uk < 16; uk++) {
        filterResTable[uk] = resDy;
        resDy -= ((resDyMin - resDyMax ) / 15);
    }

    filterResTable[0] = resDyMin;
    filterResTable[15] = resDyMax;

    /* XXX: if psid->emulatefilter = 0, ampMod1x8 is never referenced */
    if (psid->emulatefilter) {
        filterAmpl = (float)0.7;
    } else {
        filterAmpl = (float)1.0;
    }

    for (uk = 0, si = 0; si < 256; si++, uk++) {
        ampMod1x8[uk] = (signed char)((si - 0x80) * filterAmpl);
    }
}


uint8_t fastsid_read(sound_t *psid, uint16_t addr)
{
    uint8_t ret;
    uint16_t ffix;
    uint32_t rvstore;
    // uint64_t tmp;

    switch (addr) {
        case 0x19:
            /* pot/x */
            ret = 0xff;
            break;
        case 0x1a:
            /* pot/y */
            ret = 0xff;
            break;
        case 0x1b:
            /* osc3 / random */
            ffix = (uint16_t)(42 * psid->v[2].vt.fs);
            rvstore = psid->v[2].vt.rv;
            if (psid->v[2].noise && psid->v[2].vt.f + ffix < psid->v[2].vt.f) {
                psid->v[2].vt.rv = NSHIFT(psid->v[2].vt.rv, 16);
            }
            psid->v[2].vt.f += ffix;
            ret = (uint8_t)(psid->v[2].doosc() >> 7);
            psid->v[2].vt.f -= ffix;
            psid->v[2].vt.rv = rvstore;
            break;
        case 0x1c:
            ret = (uint8_t)(psid->v[2].vt.adsr >> 23);
            break;
        default:
            /*
            while ((tmp = psid->laststorebit) &&
                   (tmp = psid->laststoreclk + sidreadclocks[tmp]) < maincpu_clk) {
                psid->laststoreclk = tmp;
                psid->laststore &= 0xfeff >> psid->laststorebit--;
            }
            */
            ret = psid->laststore;
    }

    return ret;
}

void fastsid_store(sound_t *psid, uint16_t addr, uint8_t byte)
{
    switch (addr) {
        case 4:
            if ((psid->d[addr] ^ byte) & 1) {
                psid->v[0].vt.gateflip = 1;
            }
        case 0:
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
            psid->v[0].vt.update = 1;
            break;
        case 11:
            if ((psid->d[addr] ^ byte) & 1) {
                psid->v[1].vt.gateflip = 1;
            }
        case 7:
        case 8:
        case 9:
        case 10:
        case 12:
        case 13:
            psid->v[1].vt.update = 1;
            break;
        case 18:
            if ((psid->d[addr] ^ byte) & 1) {
                psid->v[2].vt.gateflip = 1;
            }
        case 14:
        case 15:
        case 16:
        case 17:
        case 19:
        case 20:
            psid->v[2].vt.update = 1;
            break;
        default:
            psid->update = 1;
    }

    psid->d[addr] = byte;
    psid->laststore = byte;
    psid->laststorebit = 8;
}

/* ---------------------------------------------------------------------*/

/*
void fastsid_state_read(struct sound_s *psid, struct sid_fastsid_snapshot_state_s *sid_state)
{
    int i;

    sid_state->factor = (uint32_t)psid->factor;

    for (i = 0; i < 32; ++i) {
        sid_state->d[i] = psid->d[i];
    }

    sid_state->has3 = psid->has3;
    sid_state->vol = psid->vol;

    for (i = 0; i < 16; ++i) {
        sid_state->adrs[i] = psid->adrs[i];
        sid_state->sz[i] = psid->sz[i];
    }

    sid_state->speed1 = psid->speed1;
    sid_state->update = psid->update;
    sid_state->newsid = psid->newsid;
    sid_state->laststore = psid->laststore;
    sid_state->laststorebit = psid->laststorebit;
    sid_state->laststoreclk = (uint32_t)psid->laststoreclk;
    sid_state->emulatefilter = (uint32_t)psid->emulatefilter;
    sid_state->filterDy = (float)psid->filterDy;
    sid_state->filterResDy = (float)psid->filterResDy;
    sid_state->filterType = psid->filterType;
    sid_state->filterCurType = psid->filterCurType;
    sid_state->filterValue = psid->filterValue;

    for (i = 0; i < 3; ++i) {
        sid_state->v_nr[i] = (uint32_t)psid->v[i].nr;
        sid_state->v_f[i] = psid->v[i].f;
        sid_state->v_fs[i] = psid->v[i].fs;
        sid_state->v_noise[i] = psid->v[i].noise;
        sid_state->v_adsr[i] = psid->v[i].adsr;
        sid_state->v_adsrs[i] = psid->v[i].adsrs;
        sid_state->v_adsrz[i] = psid->v[i].adsrz;
        sid_state->v_sync[i] = psid->v[i].sync;
        sid_state->v_filter[i] = psid->v[i].filter;
        sid_state->v_update[i] = psid->v[i].update;
        sid_state->v_gateflip[i] = psid->v[i].gateflip;
        sid_state->v_adsrm[i] = psid->v[i].adsrm;
        sid_state->v_attack[i] = psid->v[i].attack;
        sid_state->v_decay[i] = psid->v[i].decay;
        sid_state->v_sustain[i] = psid->v[i].sustain;
        sid_state->v_release[i] = psid->v[i].release;
        sid_state->v_rv[i] = psid->v[i].rv;

        if (psid->v[i].wt >= &wavetable00[0] && psid->v[i].wt <= &wavetable00[1]) {
            sid_state->v_wt[i] = 0;
            sid_state->v_wt_offset[i] = psid->v[i].wt - &wavetable00[0];
        } else if (psid->v[i].wt >= &wavetable10[0] && psid->v[i].wt <= &wavetable10[4095]) {
            sid_state->v_wt[i] = 1;
            sid_state->v_wt_offset[i] = psid->v[i].wt - &wavetable10[0];
        } else if (psid->v[i].wt >= &wavetable20[0] && psid->v[i].wt <= &wavetable20[4095]) {
            sid_state->v_wt[i] = 2;
            sid_state->v_wt_offset[i] = psid->v[i].wt - &wavetable20[0];
        } else if (psid->v[i].wt >= &wavetable30[0] && psid->v[i].wt <= &wavetable30[4095]) {
            sid_state->v_wt[i] = 3;
            sid_state->v_wt_offset[i] = psid->v[i].wt - &wavetable30[0];
        } else if (psid->v[i].wt >= &wavetable40[0] && psid->v[i].wt <= &wavetable40[8191]) {
            sid_state->v_wt[i] = 4;
            sid_state->v_wt_offset[i] = psid->v[i].wt - &wavetable40[0];
        } else if (psid->v[i].wt >= &wavetable50[0] && psid->v[i].wt <= &wavetable50[8191]) {
            sid_state->v_wt[i] = 5;
            sid_state->v_wt_offset[i] = psid->v[i].wt - &wavetable50[0];
        } else if (psid->v[i].wt >= &wavetable60[0] && psid->v[i].wt <= &wavetable60[8191]) {
            sid_state->v_wt[i] = 6;
            sid_state->v_wt_offset[i] = psid->v[i].wt - &wavetable60[0];
        } else if (psid->v[i].wt >= &wavetable70[0] && psid->v[i].wt <= &wavetable70[8191]) {
            sid_state->v_wt[i] = 7;
            sid_state->v_wt_offset[i] = psid->v[i].wt - &wavetable70[0];
        } else {
            sid_state->v_wt[i] = 0;
            sid_state->v_wt_offset[i] = 0;
        }

        sid_state->v_wtpf[i] = psid->v[i].wtpf;
        sid_state->v_wtl[i] = psid->v[i].wtl;
        sid_state->v_wtr[0][i] = psid->v[i].wtr[0];
        sid_state->v_wtr[1][i] = psid->v[i].wtr[1];
        sid_state->v_filtIO[i] = (BYTE)psid->v[i].filtIO;
        sid_state->v_filtLow[i] = (float)psid->v[i].filtLow;
        sid_state->v_filtRef[i] = (float)psid->v[i].filtRef;
    }
}

void fastsid_state_write(struct sound_s *psid, struct sid_fastsid_snapshot_state_s *sid_state)
{
    int i;

    psid->factor = (int)sid_state->factor;

    for (i = 0; i < 32; ++i) {
        psid->d[i] = sid_state->d[i];
    }

    psid->has3 = sid_state->has3;
    psid->vol = sid_state->vol;

    for (i = 0; i < 16; ++i) {
        psid->adrs[i] = sid_state->adrs[i];
        psid->sz[i] = sid_state->sz[i];
    }

    psid->speed1 = sid_state->speed1;
    psid->update = sid_state->update;
    psid->newsid = sid_state->newsid;
    psid->laststore = sid_state->laststore;
    psid->laststorebit = sid_state->laststorebit;
    psid->laststoreclk = (CLOCK)sid_state->laststoreclk;
    psid->emulatefilter = (int)sid_state->emulatefilter;
    psid->filterDy = (float)sid_state->filterDy;
    psid->filterResDy = (float)sid_state->filterResDy;
    psid->filterType = psid->filterType;
    psid->filterCurType = sid_state->filterCurType;
    psid->filterValue = sid_state->filterValue;

    for (i = 0; i < 3; ++i) {
        psid->v[i].nr = (int)sid_state->v_nr[i];
        psid->v[i].f = sid_state->v_f[i];
        psid->v[i].fs = sid_state->v_fs[i];
        psid->v[i].noise = sid_state->v_noise[i];
        psid->v[i].adsr = sid_state->v_adsr[i];
        psid->v[i].adsrs = sid_state->v_adsrs[i];
        psid->v[i].adsrz = sid_state->v_adsrz[i];
        psid->v[i].sync = sid_state->v_sync[i];
        psid->v[i].filter = sid_state->v_filter[i];
        psid->v[i].update = sid_state->v_update[i];
        psid->v[i].gateflip = sid_state->v_gateflip[i];
        psid->v[i].adsrm = sid_state->v_adsrm[i];
        psid->v[i].attack = sid_state->v_attack[i];
        psid->v[i].decay = sid_state->v_decay[i];
        psid->v[i].sustain = sid_state->v_sustain[i];
        psid->v[i].release = sid_state->v_release[i];
        psid->v[i].rv = sid_state->v_rv[i];

        switch ((int)sid_state->v_wt[i]) {
            case 0:
                psid->v[i].wt = &wavetable00[sid_state->v_wt_offset[i]];
                break;
            case 1:
                psid->v[i].wt = &wavetable10[sid_state->v_wt_offset[i]];
                break;
            case 2:
                psid->v[i].wt = &wavetable20[sid_state->v_wt_offset[i]];
                break;
            case 3:
                psid->v[i].wt = &wavetable30[sid_state->v_wt_offset[i]];
                break;
            case 4:
                psid->v[i].wt = &wavetable40[sid_state->v_wt_offset[i]];
                break;
            case 5:
                psid->v[i].wt = &wavetable50[sid_state->v_wt_offset[i]];
                break;
            case 6:
                psid->v[i].wt = &wavetable60[sid_state->v_wt_offset[i]];
                break;
            case 7:
                psid->v[i].wt = &wavetable70[sid_state->v_wt_offset[i]];
                break;
        }

        psid->v[i].wtpf = sid_state->v_wtpf[i];
        psid->v[i].wtl = sid_state->v_wtl[i];
        psid->v[i].wtr[0] = sid_state->v_wtr[0][i];
        psid->v[i].wtr[1] = sid_state->v_wtr[1][i];
        psid->v[i].filtIO = (signed char)sid_state->v_filtIO[i];
        psid->v[i].filtLow = (float)sid_state->v_filtLow[i];
        psid->v[i].filtRef = (float)sid_state->v_filtRef[i];
    }
}
*/

