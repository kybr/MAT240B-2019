#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include <vector>
using namespace std;

struct CombBank {
  float gain{1};
  DelayLine delayLine;

  struct Thing {
    float feedforward;
    float feedback;
    float delayTime;
    float output;
    DelayLine delayLine;
  };

  vector<Thing> thing;

  void add(float gain, float delayTime) {
    thing.push_back({gain, gain, delayTime, 0});
  }

  float operator()(float in) {
    for (int i = 0; i < thing.size(); ++i) {
      thing[i].output =
          in * gain +
          thing[i].feedforward * delayLine.read(thing[i].delayTime) +
          thing[i].feedback * thing[i].delayLine.read(thing[i].delayTime);
    }

    delayLine.write(in);
    for (int i = 0; i < thing.size(); ++i) {
      thing[i].delayLine.write(thing[i].output);
    }

    float sum = 0;
    for (int i = 0; i < thing.size(); ++i) {
      sum += thing[i].output;
    }
    return sum / thing.size();
  }
};

struct MyApp : App {
  SoundPlayer soundPlayer;

  CombBank combBank;

  void onCreate() override {
    // configuration
    combBank.gain = 0.13;
    for (int i = 37; i < 120; i += 12) {
      for (int o : {0, 3, 7}) {
        combBank.add(0.93, 1 / mtof(i + o));
      }
    }

    soundPlayer.load("../sound/9.wav");
    soundPlayer.sampleRate = 44100;
    soundPlayer.rate(1);
  }

  Array recording;

  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = soundPlayer();
      f = combBank(f) / 3;
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