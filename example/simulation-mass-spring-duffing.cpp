#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
#include "al/util/ui/al_ControlGUI.hpp"
#include "al/util/ui/al_Parameter.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include <vector>
using namespace std;

struct Duffing {
  float stiffness = 1;   // alpha
  float duffing = 5;     // beta
  float damping = 0.02;  // delta
  float drive = 8;       // gamma
  Sine sin;              // omega

  // Duffing() { sin.frequency(500); }

  float position{0};
  float velocity{0};

  float operator()() {
    float acceleration = drive * sin();
    acceleration += -stiffness * position;
    acceleration += -damping * velocity;
    acceleration += -duffing * position * position * position;

    acceleration /= SAMPLE_RATE;

    // (semi-implicit) Euler integration
    velocity += acceleration;
    position += velocity;
    return position;
  }
};

struct MyApp : App {
  Parameter stiffness{"stiffness", "", 1, "", 0, 50};
  Parameter duffing{"duffing", "", 5, "", 1, 20};
  Parameter damping{"damping", "", 0.02, "", 0.003, 0.997};
  Parameter drive{"drive", "", 8, "", 0, 20};
  Parameter frequency{"frequency", "", 48, "", 20, 100};
  ControlGUI gui;

  Duffing oscillator;
  ShortTermRootMeanSquared strms;
  float gain{0.1};

  void onCreate() override {
    strms.set(0.07);
    //
    gui.init();
    gui.add(stiffness);
    gui.add(duffing);
    gui.add(damping);
    gui.add(drive);
    gui.add(frequency);
  }

  void onAnimate(double dt) override {
    navControl().active(!gui.usingInput());
    //
  }

  void onDraw(Graphics& g) override {
    g.clear(0.37);
    gui.draw(g);
  }

  StereoArray a;
  void onSound(AudioIOData& io) override {
    while (io()) {
      oscillator.sin.frequency(mtof(frequency));
      oscillator.stiffness = stiffness;
      oscillator.duffing = duffing;
      oscillator.damping = damping;
      oscillator.drive = drive;

      float f = oscillator() * gain;
      a(f, gain);
      float level = strms(f);
      if (level > 0.6) {
        float e = level - 0.6;
        gain -= 0.0003f * e;
      } else if (level < 0.2) {
        float e = level - 0.2;
        gain -= 0.0003f * e;
      }

      io.out(0) = f;
      io.out(1) = f;
    }
  }
  void onKeyDown(const Keyboard&) override { a.save("duffing-oscillator.wav"); }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}