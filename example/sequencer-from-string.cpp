#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include <queue>  // gets us priority_queue
#include <string>
using namespace std;

struct Event {
  double when;
  float frequency;
  float decayTime;

  // XXX there's a LOT of stuff we could put in here. every synth we've seen
  // has a different interface. most support the notion of frequency. many
  // synthesis parameters such as envelope parameters or duration may depend on
  // the "clock".
  //

  // priority_queue needs a way to compare Event objects so it can put them in
  // the right order. defining the method below is one way to accomplish this.
  //
  int operator<(const Event& other) const {
    // > or < determines ordering!
    return when > other.when;
  }
  // this means we can compare Event objects like this:
  // Event a, b;
  // if (a < b) {...}
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

  void trigger(float frequency, float decayTime) {
    pluckedString[index].set(frequency, decayTime);
    pluckedString[index].pluck();
    index++;
    if (index == N) index = 0;
  }
};

// our Thing only loops a list of midi note numbers. we can imagine something
// more interesting:
// - maintain a local notion of time
// - extract note durations
// - extract note levels
// - don't loop; call a function when done
//
struct Thing {
  vector<int> data;
  int index = 0;

  // with this function we extract information from a string to construct a
  // generator thing that outputs patterns. later this might use more
  // sophisticated tools such as regular expressions.
  //
  void load(string pattern) {
    stringstream s(pattern);
    data.clear();
    string token;
    while (getline(s, token, ' '))  //
      data.push_back(stoi(token));
    for (auto i : data) printf("%d ", i);
    printf("\n");
  }

  int operator()() {
    int returnValue = data[index];
    index++;
    if (index == data.size()) index = 0;
    return returnValue;
  }
};

struct MyApp : App {
  priority_queue<Event> eventList;

  DelayModulation delayModulation;
  StringBank stringBank;
  Edge edge;
  Thing thing;
  Echo echo;

  void onCreate() override {
    delayModulation.set(0.3, 0.005);
    echo.period(0.72);
    edge.period(0.35);
    thing.load("60 72 60 71 59 63 62");  // we'd like this to loop

    //
  }

  // this is like diy::Edge but this one has a different notion of time. it uses
  // real (wall) time rather than sample time.
  //
  struct EdgeLike {
    float period = 1;
    float timer = 0;

    bool operator()(float dt) { return fired(dt); }
    bool fired(float dt) {
      bool returnValue = false;
      if (timer > period) {
        timer -= period;
        returnValue = true;
      }
      timer += dt;
      return returnValue;
    }
  };

  EdgeLike edgeLike{.period = 0.4};

  double t = 0;
  void onAnimate(double dt) override {
    // TODO:
    // we'd like to access the GUI, reading from a text box, interpreting each
    // line as a pattern. we'd like a live text-based sequencer.
    //

    if (edgeLike(dt)) {
      float f = mtof(thing());
      eventList.push({.when = t, .frequency = f, .decayTime = 3});
    }

    t += dt;
  }

  Array a;
  void onSound(AudioIOData& io) override {
    static double now = 0;

    while (io()) {
      while (!eventList.empty()) {
        const Event& e = eventList.top();
        if (e.when < now) {
          stringBank.trigger(e.frequency, e.decayTime);
          eventList.pop();
        } else
          break;
      }

      float f = stringBank();

      f = delayModulation(f);

      f += echo(f * 0.66);

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