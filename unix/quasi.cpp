// Read:
//
// Synthesis of Quasi-Bandlimited Analog Waveforms Using Frequency Modulation
// (Schoffhauzer 2005)
//
// The code below is adapted from the pseudo-code at the end of that paper. We
// found a few problems:
//
// 1. The "scaling" factor needs to be scaled down to avoid the Nyquist
// frequency "ringing" mentioned in the paper. Dividing by 2 seems to work.
//
// 2. The filter at the end of the pulse calculation is unstable. `in_hist =
// out` leads to numerical explosion. `in_hist = out * 0.6` seems stable.
//
// 3. `osc2` should be initialized.

#include <cmath>
#include <iostream>

float samplerate = 44100;
float pi = 3.141592;

struct QBLFM {
  float osc;               // output of the saw oscillator
  float osc2;              // output of the saw oscillator 2
  float phase;             // phase accumulator
  float w;                 // normalized frequency
  float scaling;           // scaling amount
  float DC;                // DC compensation
  float norm;              // normalization amount
  float const a0 = 2.5f;   // precalculated coeffs
  float const a1 = -1.5f;  // for HF compensation
  float in_hist = 0;       // delay for the HF filter

  void recalculate(float freq) {
    // calculate w and scaling
    w = freq / samplerate;  // normalized frequency
    float n = 0.5f - w;
    scaling = 13.0f * n * n * n * n;  // calculate scaling
    DC = 0.376f - w * 0.752f;         // calculate DC compensation
    osc = 0.f;
    phase = 0.f;             // reset oscillator and phase
    norm = 1.0f - 2.0f * w;  // calculate normalization

    // we had to add these
    scaling *= 0.5;
    osc2 = 0.f;
  }

  // process loop for creating a bandlimited saw wave
  float saw() {
    phase += 2.0f * w;  // increment accumulator
    if (phase >= 1.0f)  // phase wrap
      phase -= 2.0f;

    // calculate next sample.
    // uses feedback
    // includes lowpass (2-sample mean) filter
    osc = (osc + sin(2 * pi * (phase + osc * scaling))) * 0.5f;

    // compensate HF rolloff
    float out = a0 * osc + a1 * in_hist;
    in_hist = osc;
    out = out + DC;     // compensate DC offset
    return out * norm;  //  normalized result
  }

  // pulse width of the pulse, 0..1
  float pulse(float pw = 0.5) {
    phase += 2.0f * w;  // increment accumulator
    if (phase >= 1.0f)  // phase wrap
      phase -= 2.0f;

    // calculate saw1
    osc = (osc + sin(2 * pi * (phase + osc * scaling))) * 0.5f;

    // calculate saw2
    // phase offset by pw
    osc2 = (osc2 + sin(2 * pi * (phase + osc2 * scaling + pw))) * 0.5f;

    float out = osc - osc2;  // subtract two saw waves

    // compensate HF rolloff
    out = a0 * out + a1 * in_hist;
    in_hist = out * 0.6;  // we had to add the `* 0.6` for stability

    return out * norm;  // normalized result
  }
};

int main(int argc, char* argv[]) {
  QBLFM q;
  if (argc > 1)
    q.recalculate(atof(argv[1]));
  else
    q.recalculate(220);
  while (true) printf("%f\n", q.pulse() / 4);
}
