#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

// https://en.wikipedia.org/wiki/Harmonic_oscillator
struct System {
  // this the whole state of the simulation
  //
  float position{0};
  float velocity{0};

  float operator()() {
    // semi-implicit Euler integration with time-step 1 but the sample rate is
    // "baked into" the constants. string force and damping force are
    // accumulated into velocity. mass is 1, so it disappears.
    //
    velocity += -springConstant * position;  // resting length is 0
    velocity += -dampingCoefficient * velocity;
    position += velocity;
    return position;
  }

  // "kick" the mass-spring system such that we get a nice (-1, 1) oscillation.
  //
  void trigger() {
    // we want the "mass" to move in (-1, 1). what is the potential energy of a
    // mass-spring system at 1? PE == k * x * x / 2 == k / 2. so, we want a
    // system with k / 2 energy, but we don't want to just set the displacement
    // to 1 because that would make a click. instead, we want to set the
    // velocity. what velocity would we need to have energy k / 2? KE == m * v *
    // v / 2 == k / 2. or v * v == k. so...
    //
    velocity += sqrt(springConstant);
    // consider triggering at a level depending on frequency according to the
    // Fletcher-Munson curves.
  }

  float springConstant{0};      // N/m
  float dampingCoefficient{0};  // N·s/m
  void recalculate() {
    // sample rate is "baked into" these constants to save on per-sample
    // operations.
    dampingCoefficient = 2 / (_decayTime * SAMPLE_RATE);
    springConstant = pow(_frequency * M_2PI / SAMPLE_RATE, 2) +
                     1 / pow(_decayTime * SAMPLE_RATE, 2);
  }

  // we keep these around so that we can set each independently
  //
  float _frequency{0};  // Hertz
  float _decayTime{0};  // seconds

  void set(float hertz, float seconds) {
    _frequency = hertz;
    _decayTime = seconds;
    recalculate();
  }
  void frequency(float hertz) {
    _frequency = hertz;
    recalculate();
  }
  void decayTime(float seconds) {
    _decayTime = seconds;
    recalculate();
  }
};

struct MyApp : App {
  Edge edge;
  Line frequency;
  System system;
  void onCreate() override {
    system.set(180, 0.15);
    frequency.set(180, 180, 0.15);
    edge.period(1);
    //
  }

  Array recording;
  void onSound(AudioIOData& io) override {
    while (io()) {
      if (edge()) {
        float f = mtof(rnd::uniform(27, 50));
        float t = rnd::uniform(0.005, 1.7);
        frequency.set(mtof(rnd::uniform(17, 70)), t / 7);
        edge.period(t);
        system.set(f, t / 3);
        system.trigger();
      }
      system.frequency(frequency());
      float f = sigmoid_bipolar(system());
      recording(f);
      io.out(0) = f;
      io.out(1) = f;
    }
  }

  void onExit() override {
    //
    recording.save("recording.wav");
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}