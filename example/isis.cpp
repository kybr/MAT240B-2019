/*
 */

#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include <vector>
using namespace std;

struct ISIS {
  Phasor phasor;

  float _{0};

  float carrier{220};
  float stretch{1};

  float operator()(float f) {
    // direct "don't even know what this does" (Auto)GenExpr -> C++ translation

    // History history_1(0);
    // float asin_2 = asin(history_1);
    float asin_2 = asin(_);
    // div_3 = in2 / samplerate;
    float div_3 = carrier / SAMPLE_RATE;
    // asin_4 = asin(in1);
    float asin_4 = asin(f);
    float sub_5 = asin_4 - asin_2;
    float mul_6 = sub_5 * SAMPLE_RATE;
    float div_7 = mul_6 / M_2PI;
    float mul_8 = div_7 * stretch;
    float add_9 = mul_8 + SAMPLE_RATE;
    float mul_10 = add_9 * div_3;
    // float phasor_11 = phasor(mul_10, 0);
    phasor.frequency(mul_10);
    float phasor_11 = phasor();
    // float cycle_12, cycleindex_13 = cycle(phasor_11, index = "phase");
    float cycle_12 = sin(M_2PI * phasor_11);  // sine(phasor_11);
    float add_14 = phasor_11 + asin_2;
    // float wrap_15 = wrap(add_14, 0, 1);
    float wrap_15 =
        add_14 > 1 ? add_14 - 1 : (add_14 < 0 ? add_14 + 1 : add_14);
    // float cycle_16, cycleindex_17 = cycle(wrap_15, index = "phase");
    float cycle_16 = sin(M_2PI * wrap_15);  // sine(wrap_15);
    float sub_18 = cycle_16 - cycle_12;
    float out1 = sub_18;

    // history_1_next_19 = fixdenorm(in1);
    // history_1 = history_1_next_19;

    // fixdenormal
    if (f != 0 && std::fabsf(f) < std::numeric_limits<float>::min()) {
      _ = 0;
    } else {
      _ = f;
    }

    return out1;
    //
  }
};

struct MyApp : App {
  ISIS isis;
  SoundPlayer soundPlayer;

  void onCreate() override {
    soundPlayer.load("../sound/8.wav");
    soundPlayer.rate(1);
    //
  }

  Array recording;

  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = isis(soundPlayer()) / 3;
      recording(f);
      io.out(0) = f;
      io.out(1) = f;
    }
  }

  void onKeyDown(const Keyboard& k) override {
    //
    recording.save("recording.wav");
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
