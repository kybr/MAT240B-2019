#include "everything.h"
using namespace diy;

struct System {
  float position{0};
  float velocity{0};

  float springConstant{0};      // N/m
  float dampingCoefficient{0};  // N·s/m

  float operator()() {
    velocity += -springConstant * position;  // resting length is 0
    velocity += -dampingCoefficient * velocity;
    position += velocity;
    float ke = velocity * velocity / 2;
    float pe = position * position * springConstant / 2;
    printf("ke:%f pe:%f te:%f ", ke, pe, ke + pe);
    return position;
  }

  float _frequency{0};
  float _decayTime{0};

  void set(float hertz, float seconds) {
    _frequency = hertz;
    _decayTime = seconds;
    recalculate();
  }

  void kick() {
    // find the potential energy corresponding to displacement 1
    // find the velocity that will create that energy
    velocity += sqrt(springConstant);
  }

  void recalculate() {
    dampingCoefficient = 2 / (_decayTime * SAMPLE_RATE);
    springConstant = pow(_frequency * 2 * pi / SAMPLE_RATE, 2) +
                     1 / pow(_decayTime * SAMPLE_RATE, 2);
    // float r = _dampingRatio;
    // float w1 = 2 * pi * _frequency / SAMPLE_RATE;
    // float w0 = w1 / sqrt(1 - r * r);
    // springConstant = w0 * w0;
    // dampingCoefficient = 2 * w0 * r;
    // printf("k:%f c:%f\n", springConstant, dampingCoefficient);
  }
};

int main(int argc, char* argv[]) {
  System system;
  system.set(440, 0.15);
  system.kick();
  for (int i = 0; i < 10000; ++i) {
    say(system());
  }
}
