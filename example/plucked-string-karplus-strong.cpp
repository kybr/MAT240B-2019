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

struct PluckedString {
  Delay delay;
  Filter filter;

  void frequency(float hertz) {
    //
    delay.frequency(hertz);
  }

  float gain{1};
  void decay(float seconds) {
    // depends on frequency!
    gain = 0.8;
  }

  float v{0};
  float operator()() {
    v = filter(delay(v)) * gain;
    return v;
  }

  void pluck() {
    Noise noise;
    for (int i = 0; i < 2000; ++i) {
      delay(noise());
    }

    /*
        Saw saw;
        saw.frequency(55);
        for (int i = 0; i < 2000; ++i) {
          delay(saw());
        }
        */
  }
};

struct MyApp : App {
  Edge edge;
  PluckedString string;

  void onCreate() override {
    edge.period(0.9);
    string.gain = 0.8;
    string.pluck();
    string.frequency(mtof(rnd::uniform(21, 60)));
    //
  }

  Array recording;
  bool shouldClear = false;
  void onSound(AudioIOData& io) override {
    while (io()) {
      if (edge()) {
        string.pluck();
        string.frequency(mtof(rnd::uniform(21, 60)));
        //
      }
      float f = string();
      if (shouldClear) {
        shouldClear = false;
        recording.clear();
      }
      recording(f);
      io.out(0) = f;
      io.out(1) = f;
    }
  }

  void onKeyDown(const Keyboard& k) override {
    recording.save("karplus-strong-recording.wav");
    shouldClear = true;
    //
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
