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

#define RESID_SID_CC

#ifdef _M_ARM
#undef _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE
#define _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE 1
#endif

#include "sid.h"
#include <cmath>

#include <iostream>
#include <fstream>
using namespace std;
/*
#ifndef round
#define round(x) (x>=0.0?floor(x+0.5):ceil(x-0.5))
#endif
*/
// DIRK
#define resid_round(x) (x>=0.0?floor(x+0.5):ceil(x-0.5))

namespace reSID
{

inline short clip(int input)
{
    // Saturated arithmetics to guard against 16 bit sample overflow.
    if (unlikely(input > 32767)) {
      return 32767;
    }
    if (unlikely(input < -32768)) {
      return -32768;
    }
    return (short)input;
}

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
SID::SID()
{
  // Initialize pointers.
  sample = 0;
  fir = 0;
  fir_N = 0;
  fir_RES = 0;
  fir_beta = 0;
  fir_f_cycles_per_sample = 0;
  fir_filter_scale = 0;

  sid_model = MOS6581;
  voice[0].set_sync_source(&voice[2]);
  voice[1].set_sync_source(&voice[0]);
  voice[2].set_sync_source(&voice[1]);

  set_sampling_parameters(985248, SAMPLE_FAST, 44100);

  bus_value = 0;
  bus_value_ttl = 0;
  write_pipeline = 0;

  databus_ttl = 0;

  raw_debug_output = false;
}


// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
SID::~SID()
{
  delete[] sample;
  delete[] fir;
}


// ----------------------------------------------------------------------------
// Set chip model.
// ----------------------------------------------------------------------------
void SID::set_chip_model(chip_model model)
{
  sid_model = model;

  /*
    results from real C64 (testprogs/SID/bitfade/delayfrq0.prg):

    (new SID) (250469/8580R5) (250469/8580R5)
    delayfrq0    ~7a000        ~108000

    (old SID) (250407/6581)
    delayfrq0    ~01d00

   */
  databus_ttl = sid_model == MOS8580 ? 0xa2000 : 0x1d00;

  for (int i = 0; i < 3; i++) {
    voice[i].set_chip_model(model);
  }

  filter.set_chip_model(model);
}


// ----------------------------------------------------------------------------
// SID reset.
// ----------------------------------------------------------------------------
void SID::reset()
{
  for (int i = 0; i < 3; i++) {
    voice[i].reset();
  }
  filter.reset();
  extfilt.reset();

  bus_value = 0;
  bus_value_ttl = 0;
}


// ----------------------------------------------------------------------------
// Write 16-bit sample to audio input.
// Note that to mix in an external audio signal, the signal should be
// resampled to 1MHz first to avoid sampling noise.
// ----------------------------------------------------------------------------
void SID::input(short sample)
{
  // The input can be used to simulate the MOS8580 "digi boost" hardware hack.
  filter.input(sample);
}


// ----------------------------------------------------------------------------
// Read registers.
//
// Reading a write only register returns the last byte written to any SID
// register. The individual bits in this value start to fade down towards
// zero after a few cycles. All bits reach zero within approximately
// $2000 - $4000 cycles.
// It has been claimed that this fading happens in an orderly fashion, however
// sampling of write only registers reveals that this is not the case.
// NB! This is not correctly modeled.
// The actual use of write only registers has largely been made in the belief
// that all SID registers are readable. To support this belief the read
// would have to be done immediately after a write to the same register
// (remember that an intermediate write to another register would yield that
// value instead). With this in mind we return the last value written to
// any SID register for $4000 cycles without modeling the bit fading.
// ----------------------------------------------------------------------------
reg8 SID::read(reg8 offset)
{
  switch (offset) {
  case 0x19:
    bus_value = potx.readPOT();
    bus_value_ttl = databus_ttl;
    break;
  case 0x1a:
    bus_value = poty.readPOT();
    bus_value_ttl = databus_ttl;
    break;
  case 0x1b:
    bus_value = voice[2].wave.readOSC();
    bus_value_ttl = databus_ttl;
    break;
  case 0x1c:
    bus_value = voice[2].envelope.readENV();
    bus_value_ttl = databus_ttl;
    break;
  }
  return bus_value;
}


// ----------------------------------------------------------------------------
// Write registers.
// Writes are one cycle delayed on the MOS8580. This is only modeled for
// single cycle clocking.
// ----------------------------------------------------------------------------
void SID::write(reg8 offset, reg8 value)
{
  write_address = offset;
  bus_value = value;
  bus_value_ttl = databus_ttl;

  if (unlikely(sampling == SAMPLE_FAST) && (sid_model == MOS8580)) {
    // Fake one cycle pipeline delay on the MOS8580
    // when using non cycle accurate emulation.
    // This will make the SID detection method work.
    write_pipeline = 1;
  }
  else {
    write();
  }
}


// ----------------------------------------------------------------------------
// Write registers.
// ----------------------------------------------------------------------------
void SID::write()
{
  switch (write_address) {
  case 0x00:
    voice[0].wave.writeFREQ_LO(bus_value);
    break;
  case 0x01:
    voice[0].wave.writeFREQ_HI(bus_value);
    break;
  case 0x02:
    voice[0].wave.writePW_LO(bus_value);
    break;
  case 0x03:
    voice[0].wave.writePW_HI(bus_value);
    break;
  case 0x04:
    voice[0].writeCONTROL_REG(bus_value);
    break;
  case 0x05:
    voice[0].envelope.writeATTACK_DECAY(bus_value);
    break;
  case 0x06:
    voice[0].envelope.writeSUSTAIN_RELEASE(bus_value);
    break;
  case 0x07:
    voice[1].wave.writeFREQ_LO(bus_value);
    break;
  case 0x08:
    voice[1].wave.writeFREQ_HI(bus_value);
    break;
  case 0x09:
    voice[1].wave.writePW_LO(bus_value);
    break;
  case 0x0a:
    voice[1].wave.writePW_HI(bus_value);
    break;
  case 0x0b:
    voice[1].writeCONTROL_REG(bus_value);
    break;
  case 0x0c:
    voice[1].envelope.writeATTACK_DECAY(bus_value);
    break;
  case 0x0d:
    voice[1].envelope.writeSUSTAIN_RELEASE(bus_value);
    break;
  case 0x0e:
    voice[2].wave.writeFREQ_LO(bus_value);
    break;
  case 0x0f:
    voice[2].wave.writeFREQ_HI(bus_value);
    break;
  case 0x10:
    voice[2].wave.writePW_LO(bus_value);
    break;
  case 0x11:
    voice[2].wave.writePW_HI(bus_value);
    break;
  case 0x12:
    voice[2].writeCONTROL_REG(bus_value);
    break;
  case 0x13:
    voice[2].envelope.writeATTACK_DECAY(bus_value);
    break;
  case 0x14:
    voice[2].envelope.writeSUSTAIN_RELEASE(bus_value);
    break;
  case 0x15:
    filter.writeFC_LO(bus_value);
    break;
  case 0x16:
    filter.writeFC_HI(bus_value);
    break;
  case 0x17:
    filter.writeRES_FILT(bus_value);
    break;
  case 0x18:
    filter.writeMODE_VOL(bus_value);
    break;
  default:
    break;
  }

  // Tell clock() that the pipeline is empty.
  write_pipeline = 0;
}


// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
SID::State::State()
{
  int i;

  for (i = 0; i < 0x20; i++) {
    sid_register[i] = 0;
  }

  bus_value = 0;
  bus_value_ttl = 0;
  write_pipeline = 0;
  write_address = 0;
  voice_mask = 0xff;

  for (i = 0; i < 3; i++) {
    accumulator[i] = 0;
    shift_register[i] = 0x7fffff;
    shift_register_reset[i] = 0;
    shift_pipeline[i] = 0;
    pulse_output[i] = 0;
    floating_output_ttl[i] = 0;

    rate_counter[i] = 0;
    rate_counter_period[i] = 9;
    exponential_counter[i] = 0;
    exponential_counter_period[i] = 1;
    envelope_counter[i] = 0;
    envelope_state[i] = EnvelopeGenerator::RELEASE;
    hold_zero[i] = true;
    envelope_pipeline[i] = 0;
  }
}


// ----------------------------------------------------------------------------
// Read state.
// ----------------------------------------------------------------------------
SID::State SID::read_state()
{
  State state;
  int i, j;

  for (i = 0, j = 0; i < 3; i++, j += 7) {
    WaveformGenerator& wave = voice[i].wave;
    EnvelopeGenerator& envelope = voice[i].envelope;
    state.sid_register[j + 0] = wave.freq & 0xff;
    state.sid_register[j + 1] = wave.freq >> 8;
    state.sid_register[j + 2] = wave.pw & 0xff;
    state.sid_register[j + 3] = wave.pw >> 8;
    state.sid_register[j + 4] =
      (wave.waveform << 4)
      | (wave.test ? 0x08 : 0)
      | (wave.ring_mod ? 0x04 : 0)
      | (wave.sync ? 0x02 : 0)
      | (envelope.gate ? 0x01 : 0);
    state.sid_register[j + 5] = (envelope.attack << 4) | envelope.decay;
    state.sid_register[j + 6] = (envelope.sustain << 4) | envelope.release;
  }

  state.sid_register[j++] = filter.fc & 0x007;
  state.sid_register[j++] = filter.fc >> 3;
  state.sid_register[j++] = (filter.res << 4) | filter.filt;
  state.sid_register[j++] = filter.mode | filter.vol;

  // These registers are superfluous, but are included for completeness.
  for (; j < 0x1d; j++) {
    state.sid_register[j] = read(j);
  }
  for (; j < 0x20; j++) {
    state.sid_register[j] = 0;
  }

  state.bus_value = bus_value;
  state.bus_value_ttl = bus_value_ttl;
  state.write_pipeline = write_pipeline;
  state.write_address = write_address;
  state.voice_mask = filter.voice_mask;

  for (i = 0; i < 3; i++) {
    state.accumulator[i] = voice[i].wave.accumulator;
    state.shift_register[i] = voice[i].wave.shift_register;
    state.shift_register_reset[i] = voice[i].wave.shift_register_reset;
    state.shift_pipeline[i] = voice[i].wave.shift_pipeline;
    state.pulse_output[i] = voice[i].wave.pulse_output;
    state.floating_output_ttl[i] = voice[i].wave.floating_output_ttl;

    state.rate_counter[i] = voice[i].envelope.rate_counter;
    state.rate_counter_period[i] = voice[i].envelope.rate_period;
    state.exponential_counter[i] = voice[i].envelope.exponential_counter;
    state.exponential_counter_period[i] = voice[i].envelope.exponential_counter_period;
    state.envelope_counter[i] = voice[i].envelope.envelope_counter;
    state.envelope_state[i] = voice[i].envelope.state;
    state.hold_zero[i] = voice[i].envelope.hold_zero;
    state.envelope_pipeline[i] = voice[i].envelope.envelope_pipeline;
  }

  return state;
}


// ----------------------------------------------------------------------------
// Write state.
// ----------------------------------------------------------------------------
void SID::write_state(const State& state)
{
  int i;
  sampling_method tmp;

  /* HACK: remember sampling mode and set it to resampling incase it was fast,
           else the write() call will not work correctly */
  tmp = sampling;
  if (unlikely(sampling == SAMPLE_FAST) && (sid_model == MOS8580)) {
    sampling = SAMPLE_RESAMPLE;
  }
  for (i = 0; i <= 0x18; i++) {
    write(i, state.sid_register[i]);
  }
  sampling = tmp;   /* restore original mode */

  bus_value = state.bus_value;
  bus_value_ttl = state.bus_value_ttl;
  write_pipeline = state.write_pipeline;
  write_address = state.write_address;
  filter.set_voice_mask(state.voice_mask);

  for (i = 0; i < 3; i++) {
    voice[i].wave.accumulator = state.accumulator[i];
    voice[i].wave.shift_register = state.shift_register[i];
    voice[i].wave.shift_register_reset = state.shift_register_reset[i];
    voice[i].wave.shift_pipeline = state.shift_pipeline[i];
    voice[i].wave.pulse_output = state.pulse_output[i];
    voice[i].wave.floating_output_ttl = state.floating_output_ttl[i];

    voice[i].envelope.rate_counter = state.rate_counter[i];
    voice[i].envelope.rate_period = state.rate_counter_period[i];
    voice[i].envelope.exponential_counter = state.exponential_counter[i];
    voice[i].envelope.exponential_counter_period = state.exponential_counter_period[i];
    voice[i].envelope.envelope_counter = state.envelope_counter[i];
    voice[i].envelope.state = state.envelope_state[i];
    voice[i].envelope.hold_zero = state.hold_zero[i];
    voice[i].envelope.envelope_pipeline = state.envelope_pipeline[i];
  }
}


// ----------------------------------------------------------------------------
// Mask for voices routed into the filter / audio output stage.
// Used to physically connect/disconnect EXT IN, and for test purposed
// (voice muting).
// ----------------------------------------------------------------------------
void SID::set_voice_mask(reg4 mask)
{
  filter.set_voice_mask(mask);
}


// ----------------------------------------------------------------------------
// Enable filter.
// ----------------------------------------------------------------------------
void SID::enable_filter(bool enable)
{
  filter.enable_filter(enable);
}


// ----------------------------------------------------------------------------
// Adjust the DAC bias parameter of the filter.
// This gives user variable control of the exact CF -> center frequency
// mapping used by the filter.
// The setting is currently only effective for 6581.
// ----------------------------------------------------------------------------
void SID::adjust_filter_bias(double dac_bias) {
  filter.adjust_filter_bias(dac_bias);
}


// ----------------------------------------------------------------------------
// Enable external filter.
// ----------------------------------------------------------------------------
void SID::enable_external_filter(bool enable)
{
  extfilt.enable_filter(enable);
}

// ----------------------------------------------------------------------------
// write raw output to a file
// ----------------------------------------------------------------------------
void SID::debugoutput(void)
{
    static int recording = -1;
    static ofstream myFile;
    static int lastn;
    int n = filter.output();
    if (recording == -1) {
        /* the first call opens the file */
        recording = 0;
        myFile.open ("resid.raw", ios::out | ios::binary);
        lastn = n;
        std::cout << "reSID: waiting for output to change..." << std::endl;
    } else if ((recording == 0) && (lastn != n)) {
        /* start recording when the reSID output changes */
        recording = 1;
        std::cout << "reSID: starting recording..." << std::endl;
    }
    /* write 16bit little endian signed data */
    if (recording) {
        myFile.put(n & 0xff);
        myFile.put((n >> 8) & 0xff);
    }
}

// ----------------------------------------------------------------------------
// Enable raw debug output
// ----------------------------------------------------------------------------
void SID::enable_raw_debug_output(bool enable)
{
    raw_debug_output = enable;
    if (enable) {
        std::cout << "reSID: raw output enabled." << std::endl;
    }
}

// ----------------------------------------------------------------------------
// I0() computes the 0th order modified Bessel function of the first kind.
// This function is originally from resample-1.5/filterkit.c by J. O. Smith.
// ----------------------------------------------------------------------------
double SID::I0(double x)
{
  // Max error acceptable in I0.
  const double I0e = 1e-6;

  double sum, u, halfx, temp;
  int n;

  sum = u = n = 1;
  halfx = x/2.0;

  do {
    temp = halfx/n++;
    u *= temp*temp;
    sum += u;
  } while (u >= I0e*sum);

  return sum;
}


// ----------------------------------------------------------------------------
// Setting of SID sampling parameters.
//
// Use a clock freqency of 985248Hz for PAL C64, 1022730Hz for NTSC C64.
// The default end of passband frequency is pass_freq = 0.9*sample_freq/2
// for sample frequencies up to ~ 44.1kHz, and 20kHz for higher sample
// frequencies.
//
// For resampling, the ratio between the clock frequency and the sample
// frequency is limited as follows:
//   125*clock_freq/sample_freq < 16384
// E.g. provided a clock frequency of ~ 1MHz, the sample frequency can not
// be set lower than ~ 8kHz. A lower sample frequency would make the
// resampling code overfill its 16k sample ring buffer.
//
// The end of passband frequency is also limited:
//   pass_freq <= 0.9*sample_freq/2

// E.g. for a 44.1kHz sampling rate the end of passband frequency is limited
// to slightly below 20kHz. This constraint ensures that the FIR table is
// not overfilled.
// ----------------------------------------------------------------------------
bool SID::set_sampling_parameters(double clock_freq, sampling_method method,
                        double sample_freq, double pass_freq, double filter_scale)
{
  // Check resampling constraints.
  if (method == SAMPLE_RESAMPLE || method == SAMPLE_RESAMPLE_FASTMEM)
  {
    // Check whether the sample ring buffer would overfill.
    if (double(FIR_N)*clock_freq/sample_freq >= double(RINGSIZE)) {
      return false;
    }

    // The default passband limit is 0.9*sample_freq/2 for sample
    // frequencies below ~ 44.1kHz, and 20kHz for higher sample frequencies.
    if (pass_freq < 0) {
      pass_freq = 20000;
      if (2*pass_freq/sample_freq >= 0.9) {
        pass_freq = 0.9*sample_freq/2;
      }
    }
    // Check whether the FIR table would overfill.
    else if (pass_freq > 0.9*sample_freq/2) {
      return false;
    }

    // The filter scaling is only included to avoid clipping, so keep
    // it sane.
    if (filter_scale < 0.9 || filter_scale > 1.0) {
      return false;
    }
  }

  clock_frequency = clock_freq;
  sampling = method;

  cycles_per_sample =
    cycle_count(clock_freq/sample_freq*(1 << FIXP_SHIFT) + 0.5);

  sample_offset = 0;
  sample_prev = 0;
  sample_now = 0;

  // FIR initialization is only necessary for resampling.
  if (method != SAMPLE_RESAMPLE && method != SAMPLE_RESAMPLE_FASTMEM)
  {
    delete[] sample;
    delete[] fir;
    sample = 0;
    fir = 0;
    return true;
  }

  // Allocate sample buffer.
  if (!sample) {
    sample = new short[RINGSIZE*2];
  }
  // Clear sample buffer.
  for (int j = 0; j < RINGSIZE*2; j++) {
    sample[j] = 0;
  }
  sample_index = 0;

  const double pi = 3.1415926535897932385;

  // 16 bits -> -96dB stopband attenuation.
  const double A = -20*log10(1.0/(1 << 16));
  // A fraction of the bandwidth is allocated to the transition band,
  double dw = (1 - 2*pass_freq/sample_freq)*pi*2;
  // The cutoff frequency is midway through the transition band (nyquist)
  double wc = pi;

  // For calculation of beta and N see the reference for the kaiserord
  // function in the MATLAB Signal Processing Toolbox:
  // http://www.mathworks.com/access/helpdesk/help/toolbox/signal/kaiserord.html
  const double beta = 0.1102*(A - 8.7);
  const double I0beta = I0(beta);

  // The filter order will maximally be 124 with the current constraints.
  // N >= (96.33 - 7.95)/(2.285*0.1*pi) -> N >= 123
  // The filter order is equal to the number of zero crossings, i.e.
  // it should be an even number (sinc is symmetric about x = 0).
  int N = int((A - 7.95)/(2.285*dw) + 0.5);
  N += N & 1;

  double f_samples_per_cycle = sample_freq/clock_freq;
  double f_cycles_per_sample = clock_freq/sample_freq;

  // The filter length is equal to the filter order + 1.
  // The filter length must be an odd number (sinc is symmetric about x = 0).
  int fir_N_new = int(N*f_cycles_per_sample) + 1;
  fir_N_new |= 1;

  // We clamp the filter table resolution to 2^n, making the fixed point
  // sample_offset a whole multiple of the filter table resolution.
  int res = method == SAMPLE_RESAMPLE ?
    FIR_RES : FIR_RES_FASTMEM;
  int n = (int)ceil(log(res/f_cycles_per_sample)/log(2.0f));
  int fir_RES_new = 1 << n;

  /* Determine if we need to recalculate table, or whether we can reuse earlier cached copy.
   * This pays off on slow hardware such as current Android devices.
   */
  if (fir && fir_RES_new == fir_RES && fir_N_new == fir_N && beta == fir_beta && f_cycles_per_sample == fir_f_cycles_per_sample && fir_filter_scale == filter_scale) {
      return true;
  }
  fir_RES = fir_RES_new;
  fir_N = fir_N_new;
  fir_beta = beta;
  fir_f_cycles_per_sample = f_cycles_per_sample;
  fir_filter_scale = filter_scale;

  // Allocate memory for FIR tables.
  delete[] fir;
  fir = new short[fir_N*fir_RES];

  // Calculate fir_RES FIR tables for linear interpolation.
  for (int i = 0; i < fir_RES; i++) {
    int fir_offset = i*fir_N + fir_N/2;
    double j_offset = double(i)/fir_RES;
    // Calculate FIR table. This is the sinc function, weighted by the
    // Kaiser window.
    for (int j = -fir_N/2; j <= fir_N/2; j++) {
      double jx = j - j_offset;
      double wt = wc*jx/f_cycles_per_sample;
      double temp = jx/(fir_N/2);
      double Kaiser = fabs(temp) <= 1 ? I0(beta*sqrt(1 - temp*temp))/I0beta : 0;
      double sincwt = fabs(wt) >= 1e-6 ? sin(wt)/wt : 1;
      double val = (1 << FIR_SHIFT)*filter_scale*f_samples_per_cycle*wc/pi*sincwt*Kaiser;
      fir[fir_offset + j] = (short)resid_round(val);
    }
  }

  return true;
}


// ----------------------------------------------------------------------------
// Adjustment of SID sampling frequency.
//
// In some applications, e.g. a C64 emulator, it can be desirable to
// synchronize sound with a timer source. This is supported by adjustment of
// the SID sampling frequency.
//
// NB! Adjustment of the sampling frequency may lead to noticeable shifts in
// frequency, and should only be used for interactive applications. Note also
// that any adjustment of the sampling frequency will change the
// characteristics of the resampling filter, since the filter is not rebuilt.
// ----------------------------------------------------------------------------
void SID::adjust_sampling_frequency(double sample_freq)
{
  cycles_per_sample =
    cycle_count(clock_frequency/sample_freq*(1 << FIXP_SHIFT) + 0.5);
}


// ----------------------------------------------------------------------------
// SID clocking - delta_t cycles.
// ----------------------------------------------------------------------------
void SID::clock(cycle_count delta_t)
{
  int i;

  // Pipelined writes on the MOS8580.
  if (unlikely(write_pipeline) && likely(delta_t > 0)) {
    // Step one cycle by a recursive call to ourselves.
    write_pipeline = 0;
    clock(1);
    write();
    delta_t -= 1;
  }

  if (unlikely(delta_t <= 0)) {
    return;
  }

  // Age bus value.
  bus_value_ttl -= delta_t;
  if (unlikely(bus_value_ttl <= 0)) {
    bus_value = 0;
    bus_value_ttl = 0;
  }

  // Clock amplitude modulators.
  for (i = 0; i < 3; i++) {
    voice[i].envelope.clock(delta_t);
  }

  // Clock and synchronize oscillators.
  // Loop until we reach the current cycle.
  cycle_count delta_t_osc = delta_t;
  while (delta_t_osc) {
    cycle_count delta_t_min = delta_t_osc;

    // Find minimum number of cycles to an oscillator accumulator MSB toggle.
    // We have to clock on each MSB on / MSB off for hard sync to operate
    // correctly.
    for (i = 0; i < 3; i++) {
      WaveformGenerator& wave = voice[i].wave;

      // It is only necessary to clock on the MSB of an oscillator that is
      // a sync source and has freq != 0.
      if (likely(!(wave.sync_dest->sync && wave.freq))) {
        continue;
      }

      reg16 freq = wave.freq;
      reg24 accumulator = wave.accumulator;

      // Clock on MSB off if MSB is on, clock on MSB on if MSB is off.
      reg24 delta_accumulator =
        (accumulator & 0x800000 ? 0x1000000 : 0x800000) - accumulator;

      cycle_count delta_t_next = delta_accumulator/freq;
      if (likely(delta_accumulator%freq)) {
        ++delta_t_next;
      }

      if (unlikely(delta_t_next < delta_t_min)) {
        delta_t_min = delta_t_next;
      }
    }

    // Clock oscillators.
    for (i = 0; i < 3; i++) {
      voice[i].wave.clock(delta_t_min);
    }

    // Synchronize oscillators.
    for (i = 0; i < 3; i++) {
      voice[i].wave.synchronize();
    }

    delta_t_osc -= delta_t_min;
  }

  // Calculate waveform output.
  for (i = 0; i < 3; i++) {
    voice[i].wave.set_waveform_output(delta_t);
  }

  // Clock filter.
  filter.clock(delta_t, voice[0].output(), voice[1].output(), voice[2].output());

  // Clock external filter.
  extfilt.clock(delta_t, filter.output());
}


// ----------------------------------------------------------------------------
// SID clocking with audio sampling.
// Fixed point arithmetics are used.
//
// The example below shows how to clock the SID a specified amount of cycles
// while producing audio output:
//
// while (delta_t) {
//   bufindex += sid.clock(delta_t, buf + bufindex, buflength - bufindex);
//   write(dsp, buf, bufindex*2);
//   bufindex = 0;
// }
//
// ----------------------------------------------------------------------------
int SID::clock(cycle_count& delta_t, short* buf, int n, int interleave)
{
  switch (sampling) {
  default:
  case SAMPLE_FAST:
    return clock_fast(delta_t, buf, n, interleave);
  case SAMPLE_INTERPOLATE:
    return clock_interpolate(delta_t, buf, n, interleave);
  case SAMPLE_RESAMPLE:
    return clock_resample(delta_t, buf, n, interleave);
  case SAMPLE_RESAMPLE_FASTMEM:
    return clock_resample_fastmem(delta_t, buf, n, interleave);
  }
}


// ----------------------------------------------------------------------------
// SID clocking with audio sampling - delta clocking picking nearest sample.
// ----------------------------------------------------------------------------
int SID::clock_fast(cycle_count& delta_t, short* buf, int n, int interleave)
{
  int s;

  for (s = 0; s < n; s++) {
    cycle_count next_sample_offset = sample_offset + cycles_per_sample + (1 << (FIXP_SHIFT - 1));
    cycle_count delta_t_sample = next_sample_offset >> FIXP_SHIFT;

    if (delta_t_sample > delta_t) {
      delta_t_sample = delta_t;
    }

    clock(delta_t_sample);

    if ((delta_t -= delta_t_sample) == 0) {
      sample_offset -= delta_t_sample << FIXP_SHIFT;
      break;
    }

    sample_offset = (next_sample_offset & FIXP_MASK) - (1 << (FIXP_SHIFT - 1));
    buf[s*interleave] = output();
  }

  return s;
}


// ----------------------------------------------------------------------------
// SID clocking with audio sampling - cycle based with linear sample
// interpolation.
//
// Here the chip is clocked every cycle. This yields higher quality
// sound since the samples are linearly interpolated, and since the
// external filter attenuates frequencies above 16kHz, thus reducing
// sampling noise.
// ----------------------------------------------------------------------------
int SID::clock_interpolate(cycle_count& delta_t, short* buf, int n, int interleave)
{
  int s;

  for (s = 0; s < n; s++) {
    cycle_count next_sample_offset = sample_offset + cycles_per_sample;
    cycle_count delta_t_sample = next_sample_offset >> FIXP_SHIFT;

    if (delta_t_sample > delta_t) {
      delta_t_sample = delta_t;
    }

    for (int i = delta_t_sample; i > 0; i--) {
      clock();
      if (unlikely(i <= 2)) {
        sample_prev = sample_now;
        sample_now = output();
      }
    }

    if ((delta_t -= delta_t_sample) == 0) {
      sample_offset -= delta_t_sample << FIXP_SHIFT;
      break;
    }

    sample_offset = next_sample_offset & FIXP_MASK;

    buf[s*interleave] =
      sample_prev + (sample_offset*(sample_now - sample_prev) >> FIXP_SHIFT);
  }

  return s;
}


// ----------------------------------------------------------------------------
// SID clocking with audio sampling - cycle based with audio resampling.
//
// This is the theoretically correct (and computationally intensive) audio
// sample generation. The samples are generated by resampling to the specified
// sampling frequency. The work rate is inversely proportional to the
// percentage of the bandwidth allocated to the filter transition band.
//
// This implementation is based on the paper "A Flexible Sampling-Rate
// Conversion Method", by J. O. Smith and P. Gosset, or rather on the
// expanded tutorial on the "Digital Audio Resampling Home Page":
// http://www-ccrma.stanford.edu/~jos/resample/
//
// By building shifted FIR tables with samples according to the
// sampling frequency, the implementation below dramatically reduces the
// computational effort in the filter convolutions, without any loss
// of accuracy. The filter convolutions are also vectorizable on
// current hardware.
//
// Further possible optimizations are:
// * An equiripple filter design could yield a lower filter order, see
//   http://www.mwrf.com/Articles/ArticleID/7229/7229.html
// * The Convolution Theorem could be used to bring the complexity of
//   convolution down from O(n*n) to O(n*log(n)) using the Fast Fourier
//   Transform, see http://en.wikipedia.org/wiki/Convolution_theorem
// * Simply resampling in two steps can also yield computational
//   savings, since the transition band will be wider in the first step
//   and the required filter order is thus lower in this step.
//   Laurent Ganier has found the optimal intermediate sampling frequency
//   to be (via derivation of sum of two steps):
//     2 * pass_freq + sqrt [ 2 * pass_freq * orig_sample_freq
//       * (dest_sample_freq - 2 * pass_freq) / dest_sample_freq ]
//
// NB! the result of right shifting negative numbers is really
// implementation dependent in the C++ standard.
// ----------------------------------------------------------------------------
int SID::clock_resample(cycle_count& delta_t, short* buf, int n, int interleave)
{
  int s;

  for (s = 0; s < n; s++) {
    cycle_count next_sample_offset = sample_offset + cycles_per_sample;
    cycle_count delta_t_sample = next_sample_offset >> FIXP_SHIFT;

    if (delta_t_sample > delta_t) {
      delta_t_sample = delta_t;
    }

    for (int i = 0; i < delta_t_sample; i++) {
      clock();
      sample[sample_index] = sample[sample_index + RINGSIZE] = clip(output());
      ++sample_index &= RINGMASK;
    }

    if ((delta_t -= delta_t_sample) == 0) {
      sample_offset -= delta_t_sample << FIXP_SHIFT;
      break;
    }

    sample_offset = next_sample_offset & FIXP_MASK;

    int fir_offset = sample_offset*fir_RES >> FIXP_SHIFT;
    int fir_offset_rmd = sample_offset*fir_RES & FIXP_MASK;
    short* fir_start = fir + fir_offset*fir_N;
    short* sample_start = sample + sample_index - fir_N - 1 + RINGSIZE;

    // Convolution with filter impulse response.
    int v1 = 0;
    for (int j = 0; j < fir_N; j++) {
      v1 += sample_start[j]*fir_start[j];
    }

    // Use next FIR table, wrap around to first FIR table using
    // next sample.
    if (unlikely(++fir_offset == fir_RES)) {
      fir_offset = 0;
      ++sample_start;
    }
    fir_start = fir + fir_offset*fir_N;

    // Convolution with filter impulse response.
    int v2 = 0;
    for (int k = 0; k < fir_N; k++) {
      v2 += sample_start[k]*fir_start[k];
    }

    // Linear interpolation.
    // fir_offset_rmd is equal for all samples, it can thus be factorized out:
    // sum(v1 + rmd*(v2 - v1)) = sum(v1) + rmd*(sum(v2) - sum(v1))
    int v = v1 + int((unsigned(fir_offset_rmd)*unsigned(v2 - v1)) >> FIXP_SHIFT);

    v >>= FIR_SHIFT;

    buf[s*interleave] = clip(v);
  }

  return s;
}


// ----------------------------------------------------------------------------
// SID clocking with audio sampling - cycle based with audio resampling.
// ----------------------------------------------------------------------------
int SID::clock_resample_fastmem(cycle_count& delta_t, short* buf, int n, int interleave)
{
  int s;

  for (s = 0; s < n; s++) {
    cycle_count next_sample_offset = sample_offset + cycles_per_sample;
    cycle_count delta_t_sample = next_sample_offset >> FIXP_SHIFT;

    if (delta_t_sample > delta_t) {
      delta_t_sample = delta_t;
    }

    for (int i = 0; i < delta_t_sample; i++) {
      clock();
      sample[sample_index] = sample[sample_index + RINGSIZE] = output();
      ++sample_index &= RINGMASK;
    }

    if ((delta_t -= delta_t_sample) == 0) {
      sample_offset -= delta_t_sample << FIXP_SHIFT;
      break;
    }

    sample_offset = next_sample_offset & FIXP_MASK;

    int fir_offset = sample_offset*fir_RES >> FIXP_SHIFT;
    short* fir_start = fir + fir_offset*fir_N;
    short* sample_start = sample + sample_index - fir_N + RINGSIZE;

    // Convolution with filter impulse response.
    int v = 0;
    for (int j = 0; j < fir_N; j++) {
      v += sample_start[j]*fir_start[j];
    }

    v >>= FIR_SHIFT;

    buf[s*interleave] = clip(v);
  }

  return s;
}

} // namespace reSID
