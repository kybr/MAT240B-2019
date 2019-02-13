#ifndef __240C_SYNTHS__
#define __240C_SYNTHS__

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>

// C++ STD library
#include <iostream>
#include <vector>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include "al/core/math/al_Random.hpp"  // rnd::uniform()

namespace diy {

const int SAMPLE_RATE = 44100;
const int BLOCK_SIZE = 512;
const int OUTPUT_CHANNELS = 2;
const int INPUT_CHANNELS = 2;

float scale(float value, float low, float high, float Low, float High) {
  return Low + (High - Low) * ((value - low) / (high - low));
}
float mtof(float m) { return 8.175799f * powf(2.0f, m / 12.0f); }
float ftom(float f) { return 12.0f * log2f(f / 8.175799f); }
float dbtoa(float db) { return 1.0f * powf(10.0f, db / 20.0f); }
float atodb(float a) { return 20.0f * log10f(a / 1.0f); }
float sigmoid(float x) { return 1 / (1 + expf(-x)); }
float sigmoid_bipolar(float x) { return 2 * sigmoid(x) - 1; }

struct Phasor {
  float phase = 0.0;        // on the interval [0, 1)
  float increment = 0.001;  // led to an low F

  void period(float seconds) { frequency(1 / seconds); }
  void frequency(float hertz) {
    // this function may run per-sample. all this stuff costs performance

    // XXX check for INSANE frequencies
    if (hertz > SAMPLE_RATE) {
      printf("hertz > SAMPLE_RATE\n");
      exit(1);
    }
    if (hertz < -SAMPLE_RATE) {
      printf("hertz < -SAMPLE_RATE\n");
      exit(1);
    }
    increment = hertz / SAMPLE_RATE;
  }
  float frequency() { return SAMPLE_RATE * increment; }
  void modulate(float hertz) { increment += hertz / SAMPLE_RATE; }

  float operator()() {
    // increment and wrap phase; this only works correctly for frequencies in
    // (-SAMPLE_RATE, SAMPLE_RATE) because otherwise increment will be greater
    // than 1 or less than -1 and phase will get away from us.
    //
    phase += increment;
    if (phase > 1) phase -= 1;
    if (phase < 0) phase += 1;
    return phase;
  }
};

struct QuasiBandlimited {
  //
  // from "Synthesis of Quasi-Bandlimited Analog Waveforms Using Frequency
  // Modulation" by Peter Schoffhauzer
  // (http://scp.web.elte.hu/papers/synthesis1.pdf)
  //
  const float a0 = 2.5;   // precalculated coeffs
  const float a1 = -1.5;  // for HF compensation

  // variables
  float osc;      // output of the saw oscillator
  float osc2;     // output of the saw oscillator 2
  float phase;    // phase accumulator
  float w;        // normalized frequency
  float scaling;  // scaling amount
  float DC;       // DC compensation
  float norm;     // normalization amount
  float last;     // delay for the HF filter

  float Frequency, Filter, PulseWidth;

  QuasiBandlimited() {
    reset();
    Frequency = 1.0;
    Filter = 0.85;
    PulseWidth = 0.5;
    recalculate();
  }

  void reset() {
    // zero oscillator and phase
    osc = 0.0;
    osc2 = 0.0;
    phase = 0.0;
  }

  void recalculate() {
    w = Frequency / SAMPLE_RATE;  // normalized frequency
    float n = 0.5 - w;
    scaling = Filter * 13.0f * powf(n, 4.0f);  // calculate scaling
    DC = 0.376 - w * 0.752;                    // calculate DC compensation
    norm = 1.0 - 2.0 * w;                      // calculate normalization
  }

  void frequency(float f) {
    Frequency = f;
    recalculate();
  }

  void filter(float f) {
    Filter = f;
    recalculate();
  }

  void pulseWidth(float w) {
    PulseWidth = w;
    recalculate();
  }

  void step() {
    // increment accumulator
    phase += 2.0 * w;
    if (phase >= 1.0) phase -= 2.0;
    if (phase <= -1.0) phase += 2.0;
  }

  // process loop for creating a bandlimited saw wave
  float saw() {
    step();

    // calculate next sample
    osc = (osc + sinf(2 * M_PI * (phase + osc * scaling))) * 0.5;
    // compensate HF rolloff
    float out = a0 * osc + a1 * last;
    last = osc;
    out = out + DC;     // compensate DC offset
    return out * norm;  // store normalized result
  }

  // process loop for creating a bandlimited PWM pulse
  float pulse() {
    step();

    // calculate saw1
    osc = (osc + sinf(2 * M_PI * (phase + osc * scaling))) * 0.5;
    // calculate saw2
    osc2 =
        (osc2 + sinf(2 * M_PI * (phase + osc2 * scaling + PulseWidth))) * 0.5;
    float out = osc - osc2;  // subtract two saw waves
    // compensate HF rolloff
    out = a0 * out + a1 * last;
    last = osc;
    return out * norm;  // store normalized result
  }
};

struct Saw : QuasiBandlimited {
  float operator()() { return saw(); }
};

struct SawAlias : Phasor {
  float operator()() { return Phasor::operator()() * 2 - 1; }
};

struct Rect : QuasiBandlimited {
  float operator()() { return pulse(); }
};

struct RectAlias : Phasor {
  float dutyCycle = 0.5;
  float operator()() { return (Phasor::operator()() < dutyCycle) ? -1 : 1; }
};

// the partials roll off very quickly when compared to naiive Saw and Square; do
// we really need a band-limited triangle?
struct Tri : Phasor {
  float operator()() {
    float f = Phasor::operator()();
    return ((f < 0.5) ? f : 1 - f) * 4 - 1;
  }
};

struct DCblock {
  float x1 = 0, y1 = 0;
  float operator()(float in1) {
    float y = in1 - x1 + y1 * 0.9997;
    x1 = in1;
    y1 = y;
    return y;
  }
};

/*
 * this one is not ready
struct Tri : QuasiBandlimited {
  DCblock block, block2;
  float value = 0;
  float operator()() {
    value += block(pulse());
    return block2(value);
  }
};
*/

struct History {
  float _value = 0;
  float operator()(float value) {
    float returnValue = _value;
    _value = value;
    return returnValue;
  }
};

class Biquad {
  // Audio EQ Cookbook
  // http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

  // x[n-1], x[n-2], y[n-1], y[n-2]
  float x1 = 0, x2 = 0, y1 = 0, y2 = 0;

  // filter coefficients
  float b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0;

 public:
  float operator()(float x0) {
    // Direct Form 1, normalized...
    float y0 = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
    y2 = y1;
    y1 = y0;
    x2 = x1;
    x1 = x0;
    return y0;
  }

  void normalize(float a0) {
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
    // print();
  }

  void print() {
    printf("b0:%f ", b0);
    printf("b1:%f ", b1);
    printf("b2:%f ", b2);
    printf("a1:%f ", a1);
    printf("a2:%f ", a2);
    printf("\n");
  }

  void lpf(float f0, float Q) {
    float w0 = 2 * M_PI * f0 / SAMPLE_RATE;
    float alpha = sin(w0) / (2 * Q);
    b0 = (1 - cos(w0)) / 2;
    b1 = 1 - cos(w0);
    b2 = (1 - cos(w0)) / 2;
    float a0 = 1 + alpha;
    a1 = -2 * cos(w0);
    a2 = 1 - alpha;

    normalize(a0);
  }

  void hpf(float f0, float Q) {
    float w0 = 2 * M_PI * f0 / SAMPLE_RATE;
    float alpha = sin(w0) / (2 * Q);
    b0 = (1 + cos(w0)) / 2;
    b1 = -(1 + cos(w0));
    b2 = (1 + cos(w0)) / 2;
    float a0 = 1 + alpha;
    a1 = -2 * cos(w0);
    a2 = 1 - alpha;

    normalize(a0);
  }

  void bpf(float f0, float Q) {
    float w0 = 2 * M_PI * f0 / SAMPLE_RATE;
    float alpha = sin(w0) / (2 * Q);
    b0 = Q * alpha;
    b1 = 0;
    b2 = -Q * alpha;
    float a0 = 1 + alpha;
    a1 = -2 * cos(w0);
    a2 = 1 - alpha;

    normalize(a0);
  }

  void notch(float f0, float Q) {
    float w0 = 2 * M_PI * f0 / SAMPLE_RATE;
    float alpha = sin(w0) / (2 * Q);
    b0 = 1;
    b1 = -2 * cos(w0);
    b2 = 1;
    float a0 = 1 + alpha;
    a1 = -2 * cos(w0);
    a2 = 1 - alpha;

    normalize(a0);
  }

  void apf(float f0, float Q) {
    float w0 = 2 * M_PI * f0 / SAMPLE_RATE;
    float alpha = sin(w0) / (2 * Q);
    b0 = 1 - alpha;
    b1 = -2 * cos(w0);
    b2 = 1 + alpha;
    float a0 = 1 + alpha;
    a1 = -2 * cos(w0);
    a2 = 1 - alpha;

    normalize(a0);
  }
};

struct Edge {
  float phase = 0.0;        // on the interval [0, 1)
  float increment = 0.001;  // led to an low F

  void frequency(float hertz) { increment = hertz / SAMPLE_RATE; }
  void period(float seconds) { frequency(1 / seconds); }

  bool operator()() {
    phase += increment;
    if (phase > 1) {
      phase -= 1;
      return true;
    }
    return false;
  }
};

struct OnePole {
  float b0 = 1, a1 = 0, yn1 = 0;
  void frequency(float hertz) {
    a1 = exp(-2.0f * 3.14159265358979323846f * hertz / SAMPLE_RATE);
    b0 = 1.0f - a1;
  }
  void period(float seconds) { frequency(1 / seconds); }
  float operator()(float xn) { return yn1 = b0 * xn + a1 * yn1; }
};

struct Array : std::vector<float> {
  void operator()(float f) {
    push_back(f);
    //
  }
  void save(const char* fileName) const {
    drwav_data_format format;
    format.channels = 1;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.sampleRate = 44100;
    format.bitsPerSample = 32;
    drwav* pWav = drwav_open_file_write(fileName, &format);
    if (pWav == nullptr) {
      std::cerr << "failed to write " << fileName << std::endl;
      drwav_close(pWav);
      return;
    }
    drwav_uint64 samplesWritten = drwav_write(pWav, size(), data());
    if (samplesWritten != size()) {
      std::cerr << "failed to write all samples to " << fileName << std::endl;
      drwav_close(pWav);
      return;
    }
    drwav_close(pWav);
  }

  bool load(const char* fileName) {
    drwav* pWav = drwav_open_file(fileName);
    if (pWav == nullptr) return false;

    if (pWav->channels == 1) {
      resize(pWav->totalPCMFrameCount);
      drwav_read_f32(pWav, size(), data());  // XXX does fail?
      drwav_close(pWav);
      return true;
    }

    if (pWav->channels > 1) {
      float* pSampleData = (float*)malloc((size_t)pWav->totalPCMFrameCount *
                                          pWav->channels * sizeof(float));
      drwav_read_f32(pWav, pWav->totalPCMFrameCount, pSampleData);
      drwav_close(pWav);

      resize(pWav->totalPCMFrameCount);
      for (unsigned i = 0; i < size(); ++i)
        at(i) = pSampleData[pWav->channels * i];  // only read the first channel
      return true;
    }

    return false;
  }

  // raw lookup
  // except that i think "at" does bounds checking
  float raw(const float index) const {
    const unsigned i = floor(index);
    const float x0 = at(i);
    const float x1 = at((i == (size() - 1)) ? 0 : i + 1);  // looping semantics
    const float t = index - i;
    return x1 * t + x0 * (1 - t);
  }

  // void resize(unsigned n) { data.resize(n, 0); }
  // float& operator[](unsigned index) { return data[index]; }

  // allow for sloppy indexing (e.g., negative, huge) by fixing the index to
  // within the bounds of the array
  float get(float index) const {
    if (index < 0) index += size();
    if (index > size()) index -= size();
    return raw(index);  // defer to our method without bounds checking
  }
  float operator[](const float index) const { return get(index); }
  float phasor(float index) const { return get(size() * index); }

  void add(const float index, const float value) {
    const unsigned i = floor(index);
    const unsigned j = (i == (size() - 1)) ? 0 : i + 1;  // looping semantics
    const float t = index - i;
    at(i) += value * (1 - t);
    at(j) += value * t;
  }
};

struct Delay : Array {
  float delay;
  unsigned next;
  Delay(float seconds = 2) {
    resize(ceil(seconds * SAMPLE_RATE));
    next = 0;
  }

  void period(float seconds) { delay = seconds * SAMPLE_RATE; }
  void frequency(float hertz) { period(1 / hertz); }

  float operator()(float sample) {
    float index = next - delay;
    if (index < 0) index += size();
    float returnValue = get(index);  // read
    at(next) = sample;               // write
    // data[next] = data[next] * 0.5 + sample; // example of feedback
    next++;
    if (next >= size()) next = 0;
    return returnValue;
  }
};

struct Line {
  float value = 0, target = 0, seconds = 1 / SAMPLE_RATE, increment = 0;

  void set() {
    if (seconds <= 0) seconds = 1 / SAMPLE_RATE;
    // slope per sample
    increment = (target - value) / (seconds * SAMPLE_RATE);
  }
  void set(float v, float t, float s) {
    value = v;
    target = t;
    seconds = s;
    set();
  }
  void set(float t, float s) {
    target = t;
    seconds = s;
    set();
  }
  void set(float t) {
    target = t;
    set();
  }

  bool done() { return value == target; }

  float operator()() {
    if (value != target) {
      value += increment;
      if ((increment < 0) ? (value < target) : (value > target)) value = target;
    }
    return value;
  }
};

struct AttackDecay {
  Line attack, decay;

  void set(float riseTime, float fallTime, float peakValue) {
    attack.set(0, peakValue, riseTime);
    decay.set(peakValue, 0, fallTime);
  }

  float operator()() {
    if (!attack.done()) return attack();
    return decay();
  }
};

struct ADSR {
  Line attack, decay, release;
  int state = 0;

  void set(float a, float d, float s, float r) {
    attack.set(0, 1, a);
    decay.set(1, s, d);
    release.set(s, 0, r);
  }

  void on() {
    attack.value = 0;
    decay.value = 1;
    state = 1;
  }

  void off() {
    release.value = decay.target;
    state = 3;
  }

  float operator()() {
    switch (state) {
      default:
      case 0:
        return 0;
      case 1:
        if (!attack.done()) return attack();
        if (!decay.done()) return decay();
        state = 2;
      case 2:  // sustaining...
        return decay.target;
      case 3:
        return release();
    }
  }
  void print() {
    printf("  state:%d\n", state);
    printf("  attack:%f\n", attack.seconds);
    printf("  decay:%f\n", decay.seconds);
    printf("  sustain:%f\n", decay.target);
    printf("  release:%f\n", release.seconds);
  }
};

struct Table : Phasor, Array {
  Table(unsigned size = 4096) { resize(size); }

  virtual float operator()() {
    const float index = phase * size();
    const float v = get(index);
    Phasor::operator()();
    return v;
  }
};

struct SoundPlayer : Phasor, Array {
  float sampleRate;

  void load(float* _data, int frameCount, float _sampleRate) {
    resize(frameCount);
    for (int i = 0; i < frameCount; ++i) at(i) = _data[i];
    sampleRate = _sampleRate;
    rate(1);
  }

  void rate(float ratio) { period((size() / sampleRate) / ratio); }

  virtual float operator()() {
    const float index = phase * size();
    const float v = get(index);
    Phasor::operator()();
    return v;
  }
};

struct Noise : Table {
  Noise(unsigned size = 20 * 44100) {
    resize(size);
    for (unsigned i = 0; i < size; ++i) at(i) = al::rnd::uniformS();
  }
};

struct Normal : Table {
  Normal(unsigned size = 20 * 44100) {
    resize(size);
    for (unsigned i = 0; i < size; ++i) at(i) = al::rnd::normal();
    float maximum = 0;
    for (unsigned i = 0; i < size; ++i)
      if (abs(at(i)) > maximum) maximum = abs(at(i));
    for (unsigned i = 0; i < size; ++i) at(i) /= maximum;
  }
};

struct Sine : Table {
  Sine(unsigned size = 10000) {
    const float pi2 = M_PI * 2;
    resize(size);
    for (unsigned i = 0; i < size; ++i) at(i) = sinf(i * pi2 / size);
  }
};

struct SineArray : Array {
  SineArray(unsigned size = 10000) {
    const float pi2 = M_PI * 2;
    resize(size);
    for (unsigned i = 0; i < size; ++i) at(i) = sinf(i * pi2 / size);
  }
  float operator()(float phase) { return phasor(phase); }
};

float sine(float phase) {
  static SineArray instance;
  return instance(phase);
}

//
// RAII
struct BlockTimer {
  std::chrono::high_resolution_clock::time_point begin;

  BlockTimer() : begin(std::chrono::high_resolution_clock::now()) {}

  ~BlockTimer() {
    double t = std::chrono::duration<double>(
                   std::chrono::high_resolution_clock::now() - begin)
                   .count();
    if (t > 0) std::cout << "...took " << t << " seconds." << std::endl;
  }
};

}  // namespace diy
#endif  // __240C_SYNTHS__
