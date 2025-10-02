//  ---------------------------------------------------------------------------
//  This file is part of reSID, a MOS6581 SID emulator engine.
//  Copyright (C) 2010  Dag Lem <resid@nimrod.no>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  ---------------------------------------------------------------------------

#define RESID_ENVELOPE_CC

#include "envelope.h"
#include "dac.h"

namespace reSID
{

// Rate counter periods are calculated from the Envelope Rates table in
// the Programmer's Reference Guide. The rate counter period is the number of
// cycles between each increment of the envelope counter.
// The rates have been verified by sampling ENV3.
//
// The rate counter is a 16 bit register which is incremented each cycle.
// When the counter reaches a specific comparison value, the envelope counter
// is incremented (attack) or decremented (decay/release) and the
// counter is zeroed.
//
// NB! Sampling ENV3 shows that the calculated values are not exact.
// It may seem like most calculated values have been rounded (.5 is rounded
// down) and 1 has beed added to the result. A possible explanation for this
// is that the SID designers have used the calculated values directly
// as rate counter comparison values, not considering a one cycle delay to
// zero the counter. This would yield an actual period of comparison value + 1.
//
// The time of the first envelope count can not be exactly controlled, except
// possibly by resetting the chip. Because of this we cannot do cycle exact
// sampling and must devise another method to calculate the rate counter
// periods.
//
// The exact rate counter periods can be determined e.g. by counting the number
// of cycles from envelope level 1 to envelope level 129, and dividing the
// number of cycles by 128. CIA1 timer A and B in linked mode can perform
// the cycle count. This is the method used to find the rates below.
//
// To avoid the ADSR delay bug, sampling of ENV3 should be done using
// sustain = release = 0. This ensures that the attack state will not lower
// the current rate counter period.
//
// The ENV3 sampling code below yields a maximum timing error of 14 cycles.
//     lda #$01
// l1: cmp $d41c
//     bne l1
//     ...
//     lda #$ff
// l2: cmp $d41c
//     bne l2
//
// This yields a maximum error for the calculated rate period of 14/128 cycles.
// The described method is thus sufficient for exact calculation of the rate
// periods.
//
reg16 EnvelopeGenerator::rate_counter_period[] = {
      8,  //   2ms*1.0MHz/256 =     7.81
     31,  //   8ms*1.0MHz/256 =    31.25
     62,  //  16ms*1.0MHz/256 =    62.50
     94,  //  24ms*1.0MHz/256 =    93.75
    148,  //  38ms*1.0MHz/256 =   148.44
    219,  //  56ms*1.0MHz/256 =   218.75
    266,  //  68ms*1.0MHz/256 =   265.63
    312,  //  80ms*1.0MHz/256 =   312.50
    391,  // 100ms*1.0MHz/256 =   390.63
    976,  // 250ms*1.0MHz/256 =   976.56
   1953,  // 500ms*1.0MHz/256 =  1953.13
   3125,  // 800ms*1.0MHz/256 =  3125.00
   3906,  //   1 s*1.0MHz/256 =  3906.25
  11719,  //   3 s*1.0MHz/256 = 11718.75
  19531,  //   5 s*1.0MHz/256 = 19531.25
  31250   //   8 s*1.0MHz/256 = 31250.00
};


// For decay and release, the clock to the envelope counter is sequentially
// divided by 1, 2, 4, 8, 16, 30, 1 to create a piece-wise linear approximation
// of an exponential. The exponential counter period is loaded at the envelope
// counter values 255, 93, 54, 26, 14, 6, 0. The period can be different for the
// same envelope counter value, depending on whether the envelope has been
// rising (attack -> release) or sinking (decay/release).
//
// Since it is not possible to reset the rate counter (the test bit has no
// influence on the envelope generator whatsoever) a method must be devised to
// do cycle exact sampling of ENV3 to do the investigation. This is possible
// with knowledge of the rate period for A=0, found above.
//
// The CPU can be synchronized with ENV3 by first synchronizing with the rate
// counter by setting A=0 and wait in a carefully timed loop for the envelope
// counter _not_ to change for 9 cycles. We can then wait for a specific value
// of ENV3 with another timed loop to fully synchronize with ENV3.
//
// At the first period when an exponential counter period larger than one
// is used (decay or relase), one extra cycle is spent before the envelope is
// decremented. The envelope output is then delayed one cycle until the state
// is changed to attack. Now one cycle less will be spent before the envelope
// is incremented, and the situation is normalized.
// The delay is probably caused by the comparison with the exponential counter,
// and does not seem to affect the rate counter. This has been verified by
// timing 256 consecutive complete envelopes with A = D = R = 1, S = 0, using
// CIA1 timer A and B in linked mode. If the rate counter is not affected the
// period of each complete envelope is
// (255 + 162*1 + 39*2 + 28*4 + 12*8 + 8*16 + 6*30)*32 = 756*32 = 32352
// which corresponds exactly to the timed value divided by the number of
// complete envelopes.
// NB! This one cycle delay is only modeled for single cycle clocking.


// From the sustain levels it follows that both the low and high 4 bits of the
// envelope counter are compared to the 4-bit sustain value.
// This has been verified by sampling ENV3.
//
reg8 EnvelopeGenerator::sustain_level[] = {
  0x00,
  0x11,
  0x22,
  0x33,
  0x44,
  0x55,
  0x66,
  0x77,
  0x88,
  0x99,
  0xaa,
  0xbb,
  0xcc,
  0xdd,
  0xee,
  0xff,
};


// DAC lookup tables.
unsigned short EnvelopeGenerator::model_dac[2][1 << 8] = {
  {0},
  {0},
};


// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
EnvelopeGenerator::EnvelopeGenerator()
{
  static bool class_init;

  if (!class_init) {
    // Build DAC lookup tables for 8-bit DACs.
    // MOS 6581: 2R/R ~ 2.20, missing termination resistor.
    build_dac_table(model_dac[0], 8, 2.20, false);
    // MOS 8580: 2R/R ~ 2.00, correct termination.
    build_dac_table(model_dac[1], 8, 2.00, true);

    class_init = true;
  }

  set_chip_model(MOS6581);

  // Counter's odd bits are high on powerup
  envelope_counter = 0xaa;

  // just to avoid uninitialized access with delta clocking
  next_state = RELEASE;

  reset();
}

// ----------------------------------------------------------------------------
// SID reset.
// ----------------------------------------------------------------------------
void EnvelopeGenerator::reset()
{
  // counter is not changed on reset
  envelope_pipeline = 0;
  exponential_pipeline = 0;

  state_pipeline = 0;

  attack = 0;
  decay = 0;
  sustain = 0;
  release = 0;

  gate = 0;

  rate_counter = 0;
  exponential_counter = 0;
  exponential_counter_period = 1;
  new_exponential_counter_period = 0;
  reset_rate_counter = false;

  state = RELEASE;
  rate_period = rate_counter_period[release];
  hold_zero = false;
}


// ----------------------------------------------------------------------------
// Set chip model.
// ----------------------------------------------------------------------------
void EnvelopeGenerator::set_chip_model(chip_model model)
{
  sid_model = model;
}


// ----------------------------------------------------------------------------
// Register functions.
// ----------------------------------------------------------------------------
void EnvelopeGenerator::writeCONTROL_REG(reg8 control)
{
  reg8 gate_next = control & 0x01;

  // The rate counter is never reset, thus there will be a delay before the
  // envelope counter starts counting up (attack) or down (release).

  if (gate != gate_next) {
    // Gate bit on: Start attack, decay, sustain.
    // Gate bit off: Start release.
    next_state = gate_next ? ATTACK : RELEASE;
    if (next_state == ATTACK) {
        // The decay register is "accidentally" activated during first cycle of attack phase
        state = DECAY_SUSTAIN;
        rate_period = rate_counter_period[decay];
        state_pipeline = 2;
        if (reset_rate_counter || exponential_pipeline == 2) {
            envelope_pipeline = exponential_counter_period == 1 || exponential_pipeline == 2 ? 2 : 4;
        }
        else if (exponential_pipeline == 1) { state_pipeline = 3; }
    }
    else {state_pipeline = envelope_pipeline > 0 ? 3 : 2;}
    gate = gate_next;
  }
}

void EnvelopeGenerator::writeATTACK_DECAY(reg8 attack_decay)
{
  attack = (attack_decay >> 4) & 0x0f;
  decay = attack_decay & 0x0f;
  if (state == ATTACK) {
    rate_period = rate_counter_period[attack];
  }
  else if (state == DECAY_SUSTAIN) {
    rate_period = rate_counter_period[decay];
  }
}

void EnvelopeGenerator::writeSUSTAIN_RELEASE(reg8 sustain_release)
{
  sustain = (sustain_release >> 4) & 0x0f;
  release = sustain_release & 0x0f;
  if (state == RELEASE) {
    rate_period = rate_counter_period[release];
  }
}

reg8 EnvelopeGenerator::readENV()
{
  return env3;
}

} // namespace reSID
