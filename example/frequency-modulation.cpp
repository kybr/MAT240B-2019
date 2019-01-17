#include "al/core.hpp"
#include "al/util/ui/al_ControlGUI.hpp"
#include "al/util/ui/al_Parameter.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

struct DX7FM {
  void frequency(float hertz) {
    for (auto& e : o) e.frequency(e.frequency() * e.frequencyRatio);
  }
  void on() {
    for (auto& e : o) e.envelope.on();
    for (auto& e : o) e.phase = 0;
  }
  void off() {
    for (auto& e : o) e.envelope.off();
  }

  struct Operator : Sine {
    float frequencyRatio{1};
    float scaleFactor{1};
    bool feedback{false};
    ADSR envelope;
    float operator()() {
      float v = Sine::operator()() * envelope() * scaleFactor;
      if (feedback) frequencyAdd(v);
      return v;
    }
    float operator()(float modulation) {
      frequencyAdd(modulation);
      return operator()();
    }
  };
  Operator o[6];
};

struct DX7FM1 : DX7FM {
  DX7FM1() { o[5].feedback = true; }
  float operator()() { return o[2](o[1](o[0]())) + o[5](o[4](o[3]())); }
};

void randomize(DX7FM& synth) {
  for (auto& e : synth.o) {
    e.frequencyRatio = rnd::uniform(4.0, 1.0);
    e.scaleFactor = rnd::uniform(10.0, 1.0);
    e.envelope.set(rnd::uniform(0.7, 0.1), rnd::uniform(0.5, 0.1),
                   rnd::uniform(0.8, 0.2), rnd::uniform(0.8, 0.2));
  }
  synth.o[0].frequencyRatio = 1;
  synth.o[3].frequencyRatio = 1;
}

struct MyApp : App {
  ControlGUI gui;
  Parameter mod{"/modulation", "", 0.0, "", 0.0, 1.0};
  Parameter res{"/frequency", "", 0.0, "", 0.0, 127.0};

  Mesh mesh{Mesh::LINE_STRIP};
  DX7FM1 fm;

  void onCreate() override {
    gui.init();
    gui.add(mod).add(res);

    rnd::global().seed(21);
    fm.frequency(220);
    randomize(fm);

    mesh.vertex(0, 0);
    mesh.vertex(width(), height());
  }

  void onAnimate(double dt) override {
    //
    navControl().active(!gui.usingInput());
  }

  void onDraw(Graphics& g) override {
    g.clear(0.25);
    g.camera(Viewpoint::ORTHO_FOR_2D);
    g.draw(mesh);
    gui.draw(g);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = fm() / 10;
      io.out(0) = f;
      io.out(1) = f;
    }
  }

  bool keyPressed{false};
  int key{0};
  void onKeyDown(Keyboard const& k) override {
    keyPressed = true;
    key = k.key();
    if (key == ' ') fm.on();
    if (key == 'r') randomize(fm);
  }
  void onKeyUp(Keyboard const& k) override {
    keyPressed = false;
    if (key == ' ') fm.off();
  }

  float mouseX, mouseY;
  void onMouseMove(Mouse const& m) override {
    mouseX = m.x();
    mouseY = height() - m.y();
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
