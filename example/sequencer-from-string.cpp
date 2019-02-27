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
  // possible synthesis parameters that may depend on the "clock"
  // envelope parameters
  // duration
  //

  // Event a, b;
  // if (a < b ) {...}
  int operator<(const Event& other) const {
    // > or < determines ordering!
    return when > other.when;
  }
};

const int N = 7;

struct StringBank {
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

struct Thing {
  vector<int> data;
  int index = 0;

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
    // access the GUI...
    // read from a text box
    // interpret each line as a pattern

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
      // inspect the queue of events
      // if is is empty...
      // for each event that is even a little bit in the past..
      // deque the event and dispatch an instrument
      //
      // do nothing
      while (!eventList.empty()) {
        const Event& e = eventList.top();
        if (e.when < now) {
          //
          // use properties of the event; this specific to the instrument
          stringBank.trigger(e.frequency / 2, e.decayTime);

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