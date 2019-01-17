#include "al/core.hpp"
#include "al/util/ui/al_ControlGUI.hpp"
#include "al/util/ui/al_Parameter.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

using namespace std;

struct Operator : Phasor {
  float frequencyRatio = 1;
  float scaleFactor = 1;
  ADSR envelope;

  float operator()() {
    return sine(Phasor::operator()()) * envelope() * scaleFactor;
  }
  float operator()(float modulation) {
    modulate(modulation);
    return operator()();
  }

  void print() {
    printf("  frequency:%f\n", frequency());
    printf("  ratio:%f\n", frequencyRatio);
    printf("  scale:%f\n", scaleFactor);
    envelope.print();
  }
};

struct DX7 {
  union {
    struct {
      Operator a, b, c, d, e, f;
    };
    Operator op[6];
  };

  float _ = 0;  // feedback

  DX7() {
    for (auto& o : op) {
      o.frequencyRatio = 1;
      o.scaleFactor = 1;
      o.envelope.set(0, 0, 1, 0);
      o.envelope.state = 4;
    }
  }

  void randomize() {
    for (auto& o : op) {
      o.frequencyRatio = rnd::uniform(8, 1);
      // e.frequencyRatio = rnd::uniform(2.0, 1.0);
      o.scaleFactor = rnd::uniform(8.0, 1.0);
      o.envelope.set(rnd::uniform(0.7, 0.1), rnd::uniform(0.5, 0.1),
                     rnd::uniform(0.8, 0.2), rnd::uniform(0.8, 0.2));
    }
    frequency(mtof(rnd::uniform(15, 70)));
  }

  void frequency(float hertz) {
    for (auto& o : op) o.frequency(hertz * o.frequencyRatio);
  }

  void on() {
    for (auto& o : op) {
      o.envelope.on();
      o.phase = 0;
    }
  }

  void off() {
    for (auto& o : op) o.envelope.off();
  }

  float operator()(int algorithm = 32) {
    switch (algorithm) {
      default:
      case 1:
        return (a(b()) + c(d(e(_ = f(_))))) / 2;
      case 2:
        return (a(_ = b(_)) + c(d(e(f())))) / 2;
      case 3:
        return (a(b(c())) + d(e(_ = f(_)))) / 2;
      case 4:
        return (a(b(c())) + (_ = c(d(f(_))))) / 2;
      case 5:
        return (a(b()) + c(d()) + e(_ = f(_))) / 3;
      case 6:
        return (a(b()) + c(d()) + (_ = e(f(_)))) / 3;
      case 14:
        return (a(b()) + c(d(e() + (_ = f(_))))) / 2;
      case 32:
        return (a() + b() + c() + d() + e() + (_ = f(_))) / 6;
    }
  }

  void print() {
    printf("\033[2J\033[;H");
    printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    printf("XX  DX7 Settings  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    // printf("algorithm:%d\n", algorithm);
    for (auto& o : op) {
      printf("  ----------\n");
      o.print();
    }
    fflush(stdout);
  }
};

struct MyApp : App {
  ControlGUI gui;
  Parameter frequency{"/frequency", "", 40, "", 0, 127};
  ParameterInt algorithm{"/algorithm", "", 32, "", 1, 32};

  DX7 dx7;

  void onCreate() override {
    rnd::global().seed(21);
    gui.init();
    gui.add(algorithm);
    gui.add(frequency);

    dx7.randomize();
    dx7.print();
  }

  void onAnimate(double dt) override {
    //
    navControl().active(!gui.usingInput());
  }

  void onDraw(Graphics& g) override {
    g.clear(0.25);
    gui.draw(g);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = dx7(algorithm) / 3;
      io.out(0) = f;
      io.out(1) = f;
    }
  }

  void onKeyDown(Keyboard const& k) override {
    if (k.key() == ' ') dx7.on();
    if (k.key() == 'r') dx7.randomize();
  }

  void onKeyUp(Keyboard const& k) override {
    if (k.key() == ' ') dx7.off();
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
