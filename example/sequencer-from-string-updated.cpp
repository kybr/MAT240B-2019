#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include <queue>  // gets us priority_queue
#include <regex>
#include <string>
using namespace std;

const float TEMPO = 156;

struct MegaEvent {
  struct Basic {
    double when;
    long type;
  };
  double parameter[10];
};

struct Event {
  double when;  // in seconds since the audio callback started
  float frequency;
  float gain;
  float duration;

  int operator<(const Event& other) const {
    // > or < determines ordering!
    return when > other.when;
  }
};

struct StringBank {
  static const int N = 7;

  int index = 0;
  PluckedString pluckedString[N];

  float operator()() {
    float s = 0;
    for (auto& p : pluckedString) s += p();
    return s;
  }

  void trigger(float frequency, float decayTime, float gain = 1) {
    pluckedString[index].set(frequency, decayTime);
    pluckedString[index].pluck(gain);
    index++;
    if (index == N) index = 0;
  }
};

struct Pattern {
  struct Triple {
    float frequency;
    float duration;
    float gain;
  };
  vector<Triple> data;
  int index = 0;

  double when{0};

  void load(string pattern) {
    regex re(R"((\d+) (\d) (\d))");
    auto begin = std::sregex_iterator(pattern.begin(), pattern.end(), re);
    auto end = std::sregex_iterator();

    for (sregex_iterator i = begin; i != end; ++i) {
      smatch match = *i;

      int midi = stoi(match[1].str());
      int divisor = stoi(match[2].str());
      int level = stoi(match[3].str());

      float beatsPerMinute = TEMPO;
      float beatsPerSecond = beatsPerMinute / 60;
      float secondsPerBeat = 1 / beatsPerSecond;
      float secondsPerWhole = 4 * secondsPerBeat;
      float duration = secondsPerWhole / divisor;

      float gain = dbtoa(diy::map(level, 0, 9, -20, 0));

      data.push_back({mtof(midi), duration, gain});
    }
  }

  Event operator()() {
    Triple& triple = data[index];  //

    float time = when;

    // advance stuff; update state
    when += triple.duration;
    index++;
    if (index == data.size()) index = 0;

    // return pair
    return {time, triple.frequency, triple.gain, 0.9};
  }
};

struct MyApp : App {
  priority_queue<Event> eventList;
  StringBank stringBank;
  Pattern p;
  DelayModulation delayModulation;
  Echo echo;

  void onCreate() override {
    delayModulation.set(0.1, 0.005);
    echo.period(0.8);
    p.load("60 4 3   72 4 8   60 4 2   71 4 3   59 4 5   63 4 8   62 4 5");
  }

  double now{0};
  void onAnimate(double dt) override {
    while (p.when < now) {
      eventList.push(p());
    }
  }

  Array a;
  void onSound(AudioIOData& io) override {
    while (io()) {
      // check the scheduler
      //
      while (!eventList.empty()) {
        const Event& e = eventList.top();
        if (e.when < now) {
          stringBank.trigger(e.frequency, e.duration, e.gain);
          eventList.pop();
        } else
          break;
      }

      float f = stringBank();

      f = delayModulation(f);
      f += echo(f / 5);

      io.out(0) = f;
      io.out(1) = f;

      now += 1.0 / SAMPLE_RATE;
    }
  }

  void onKeyDown(const Keyboard& k) override {
    //
    a.save("d.wav");
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}