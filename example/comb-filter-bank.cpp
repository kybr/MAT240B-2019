#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include <vector>
using namespace std;

struct CombBank {
  float gain{1}, feedforward{0}, feedback{0};
  DelayLine input;

  vector<float> delay;
  vector<DelayLine> output;
  vector<float> out;

  CombBank(int delayCount = 6, float capacity = 0.5) {
    input.resize(SAMPLE_RATE * capacity);
    delay.resize(delayCount);
    out.resize(delayCount);
    output.resize(delayCount);
    for (auto& dl : output) {
      dl.resize(SAMPLE_RATE * capacity);
    }
  }

  void set(float g, float ff, float fb) {
    gain = g;
    feedforward = ff;
    feedback = fb;
  }

  float operator()(float in) {
    for (int i = 0; i < delay.size(); ++i) {
      out[i] = gain * in + feedforward * input.read(delay[i]) +
               feedback * output[i].read(delay[i]);
    }

    input.write(in);
    for (int i = 0; i < delay.size(); ++i) {
      output[i].write(out[i]);
    }

    float sum = 0;
    for (int i = 0; i < delay.size(); ++i) {
      sum += out[i];
    }
    return sum;
  }
};

struct MyApp : App {
  SoundPlayer soundPlayer;

  CombBank combBank;

  void onCreate() override {
    combBank.set(0.5, 0.85, 0.85);
    for (int i = 0; i < combBank.delay.size(); ++i) {
      combBank.delay[i] = 1 / mtof(23 + i * 12);
    }
    soundPlayer.load("../sound/sine-sweep.wav");
    soundPlayer.sampleRate = 44100;
    soundPlayer.rate(1);
  }

  Array recording;

  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = soundPlayer();
      f = combBank(f) / (3 * combBank.delay.size());
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