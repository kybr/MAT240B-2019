#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

struct Filter {
  float x1{0};
  float operator()(float x0) {
    float v = (x0 + x1) / 2;
    x1 = x0;
    return v;
  }
};

Noise noise;  // don't put this in pluck; it loads and unloads memory!

struct PluckedString : DelayLine {
  // Biquad filter;
  Filter filter;

  float gain{1};

  float delayTime;  // in samples

  float frequency(float hertz) {
    // filter.lpf(hertz * 7, 0.67);
    period(1 / hertz);
  }
  float period(float seconds) {
    delayTime = seconds * SAMPLE_RATE;
    recalculate();
  }

  float t60{1};
  void decayTime(float _t60) {
    t60 = _t60;
    recalculate();
  }
  void recalculate() {
    int n = t60 / (delayTime / SAMPLE_RATE);
    gain = pow(dbtoa(-60 / t60), 1.0f / n);
    printf("n:%d g:%f\n", n, gain);
  }

  float operator()() {
    float delayed = filter(read(delayTime)) * gain;
    write(delayed);
    return delayed;
  }

  void pluck() {
    // put noise in the last N sample memory positions. N depends on frequency
    //
    int n = int(ceil(delayTime));
    for (int i = 0; i < n; ++i) {
      int index = next - i;
      if (index < 0)  //
        index += size();
      // at(index) = 2.0f * i / n - 1.0f;  // sawtooth!
      at(index) = noise();
    }
  }
};

struct MyApp : App {
  Edge edge;
  PluckedString string;

  void onCreate() override {
    edge.period(0.9);
    string.decayTime(5);
    string.frequency(mtof(rnd::uniform(21, 60)));
    string.pluck();
    //
  }

  Array recording;
  void onSound(AudioIOData& io) override {
    while (io()) {
      if (edge()) {
        string.frequency(mtof(rnd::uniform(21, 60)));
        float d = rnd::uniform(0.5, 7.0);
        string.decayTime(d);
        string.pluck();
        edge.period(d);
      }
      float f = string();
      recording(f);
      io.out(0) = f;
      io.out(1) = f;
    }
  }

  void onKeyDown(const Keyboard& k) override {
    recording.save("karplus-strong-recording.wav");
    //
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
