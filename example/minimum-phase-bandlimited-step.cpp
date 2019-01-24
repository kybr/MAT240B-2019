#include "al/core.hpp"
#include "al/util/ui/al_ControlGUI.hpp"
#include "al/util/ui/al_Parameter.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include "Gamma/SoundFile.h"

struct SawMinBLEP : Phasor {
  Array step;
  float rate;

  SawMinBLEP() {
    gam::SoundFile soundFile;
    if (!soundFile.openRead("../MinBLEP-down.wav")) exit(-1);
    step.resize(soundFile.frames());
    soundFile.readAll(&step.data[0]);
  }

  virtual float operator()() {
    // rate -> down as...
    // frequency -> up
    // SAMPLE_RATE * increment
    // step.size()
    float v = phase + (1 - phase) * step.phasor(phase * rate);
    Phasor::operator()();
    return (2 * v - 1) / 1.309664;
  }
};

struct MyApp : App {
  ControlGUI gui;
  Parameter note{"/note", "", 40, "", 0, 127};
  Parameter rate{"/rate", "", 0.1, "", 0, 1};

  SawMinBLEP saw;

  void onCreate() override {
    gui.init();
    gui.add(note);
    gui.add(rate);
  }

  void onAnimate(double dt) override {
    //
    navControl().active(!gui.usingInput());
  }

  void onDraw(Graphics& g) override {
    g.clear(0.25);
    //
    gui.draw(g);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = mtof(note);
      saw.rate = rate / sqrt(f);
      saw.frequency(f);
      float s = saw();
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}