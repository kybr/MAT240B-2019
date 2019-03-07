#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

struct Comb {
  float delay{0}, gain{1}, forward{0}, back{0};
  DelayLine input, output;

  Comb(float capacity = 0.5 /* a good or bad thing? */) {
    input.resize(SAMPLE_RATE * capacity);
    output.resize(SAMPLE_RATE * capacity);
  }

  void set(float d, float g, float ff, float fb) {
    delay = d;
    gain = g;
    forward = ff;
    back = fb;
  }

  float operator()(float in) {
    float out = 0;
    out = gain * in + forward * input.read(delay) + back * output.read(delay);
    input.write(in);
    output.write(out);
    return out;
  }
};

struct MyApp : App {
  SoundPlayer soundPlayer;

  Comb comb;

  void onCreate() override {
    comb.set(1 / 200.0, 0.5, 0.85, 0.85);
    soundPlayer.load("../sound/sine-sweep.wav");
    soundPlayer.sampleRate = 44100;
    soundPlayer.rate(1);
  }

  Array recording;

  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = soundPlayer();
      f = comb(f);
      recording(f);
      io.out(0) = f;
      io.out(1) = f;
    }
  }
  void onKeyDown(const Keyboard&) override { recording.save("wtf.wav"); }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}