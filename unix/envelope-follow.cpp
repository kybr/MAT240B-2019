#include "everything.h"
using namespace diy;

struct ShortTermRootMeanSquared {
  float squaredSum{0};
  float alpha{1};  // (0, 1) determines how much history to take into account

  void set(float a) { alpha = a; }

  float operator()(float f) {
    // https://en.wikipedia.org/wiki/Moving_average#Exponential%20moving%20average
    //
    // alpha
    //   1   don't take into account any history; useless
    //   x   balance history with now; XXX be more specific here
    //   0   only take history into account; nonsensical
    squaredSum = alpha * (f * f) + (1 - alpha) * squaredSum;
    return sqrt(squaredSum);
  }
};

struct EnvelopeFilterAsymmetric {
  float attack{1};
  float release{1};
  float value{0};
  void set(float a, float r) {
    // a and r in seconds
    // XXX not sure why the log(0.01)
    attack = exp(log(0.01) / (SAMPLE_RATE * a));
    release = exp(log(0.01) / (SAMPLE_RATE * a));
  }
  float operator()(float f) {
    return value = f + (value - f) * ((f > value) ? attack : release);
  }
};

int main() {
  float f;
  ShortTermRootMeanSquared m;
  m.set(0.07);

  EnvelopeFilterAsymmetric e;
  e.set(0.1, 0.2);

  while (cin >> f) {
    say(f, e(m(f)));
  }
}
