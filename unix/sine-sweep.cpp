#include "everything.h"
using namespace diy;

int main(int argc, char* argv[]) {
  float phase = 0;
  for (float note = 127; note > 0; note -= 0.001) {
    float frequency = mtof(note);
    say(sin(phase) * 0.707f);
    phase += 2 * pi * frequency / SAMPLE_RATE;
    if (phase > 2 * pi) phase -= 2 * pi;
  }
}
