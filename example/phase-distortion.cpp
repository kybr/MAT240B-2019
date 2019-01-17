#include "al/core.hpp"
#include "al/util/ui/al_ControlGUI.hpp"
#include "al/util/ui/al_Parameter.hpp"
#include "al/util/ui/al_Preset.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

using namespace std;

struct PhaseDistortion : Phasor, Array {
  PhaseDistortion(unsigned size = 10000) {
    const float pi2 = M_PI * 2;
    resize(size);
    for (unsigned i = 0; i < size; ++i) data[i] = sinf(i * pi2 / size);
  }

  Tri tri;
  float modulationFactor = 0;

  virtual float operator()() {
    const float index = (phase + tri() * modulationFactor) * size;
    const float v =
        get(index);  // XXX raw crashes and i think it should not: investigate
    Phasor::operator()();
    return v;
  }

  virtual void frequency(float hertz) {
    Phasor::frequency(hertz);
    tri.frequency(hertz);
  }
};

struct MyApp : App {
  ControlGUI gui;
  Parameter frequency{"/frequency", "", 60.0f, "", 0, 127};
  Parameter modulation{"/modulation", "", 0.5, "", 0, 1};

  PhaseDistortion phaseDistortion;

  void onCreate() override {
    gui.init();
    gui.add(frequency);
    gui.add(modulation);
    phaseDistortion.frequency(220);
    phaseDistortion.modulationFactor = 0.1;
  }

  void onAnimate(double dt) override {
    navControl().active(!gui.usingInput());
    //
    cout << modulation << endl;
  }

  void onDraw(Graphics& g) override {
    g.clear(0.25);
    gui.draw(g);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      phaseDistortion.frequency(mtof(frequency));
      phaseDistortion.modulationFactor = modulation;
      float f = phaseDistortion();
      io.out(0) = f;
      io.out(1) = f;
    }
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
