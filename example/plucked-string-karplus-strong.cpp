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

struct PluckedString : DelayLine {
  Filter filter;

  float gain{1};
  float t60{1};
  float delayTime{1};  // in seconds

  void frequency(float hertz) { period(1 / hertz); }
  void period(float seconds) {
    delayTime = seconds;
    recalculate();
  }

  void decayTime(float _t60) {
    t60 = _t60;
    recalculate();
  }

  void set(float frequency, float decayTime) {
    delayTime = 1 / frequency;
    t60 = decayTime;
    recalculate();
  }

  // given t60 and frequency (seconds and Hertz), calculate the gain...
  //
  // for a given frequency, our algorithm applies *gain* frequency-many times
  // per second. given a t60 time we can calculate how many times (n) gain will
  // be applied in those t60 seconds. we want to reduce the signal by 60dB over
  // t60 seconds or over n-many applications. this means that we want gain to be
  // a number that, when multiplied by itself n times, becomes 60 dB quieter.
  //
  void recalculate() {
    int n = t60 / delayTime;
    gain = pow(dbtoa(-60), 1.0f / n);
    printf("t:%f\tf:%f\tn:%d\tg:%f\n", t60, 1 / delayTime, n, gain);
    fflush(stdout);
  }

  float operator()() {
    float v = filter(read(delayTime)) * gain;
    write(v);
    return v;
  }

  void pluck() {
    // put noise in the last N sample memory positions. N depends on frequency
    //
    int n = int(ceil(delayTime * SAMPLE_RATE));
    for (int i = 0; i < n; ++i) {
      int index = next - i;
      if (index < 0)  //
        index += size();
      at(index) = noise(float(i) / n);
    }
  }
};

struct MyApp : App {
  Edge edge;
  PluckedString string;

  void onCreate() override {
    float d = rnd::uniform(0.5, 7.0);
    float f = mtof(rnd::uniform(21, 60));
    string.set(f, d);
    string.pluck();
    edge.period(d);
  }

  Array recording;
  void onSound(AudioIOData& io) override {
    while (io()) {
      if (edge()) {
        float d = rnd::uniform(0.5, 7.0);
        float f = mtof(rnd::uniform(21, 60));
        string.set(f, d);
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
