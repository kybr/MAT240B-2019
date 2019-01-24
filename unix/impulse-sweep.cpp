#include "everything.h"
using namespace diy;

int main(int argc, char* argv[]) {
  float phase = 0;

  for (float note = 127; note > 0; note -= 0.001) {
    float frequency = mtof(note);

    int harmonicCount = 0;
    while (frequency * harmonicCount < 0.8 * SAMPLE_RATE / 2) harmonicCount++;

    float v = 0;
    for (int h = 0; h < harmonicCount; ++h)
      // v += sin(2 * pi * h * phase);  // "float" noise
      v += sin(2.0 * pi * h * phase);  // double precision
    v /= harmonicCount;
    say(v);

    // increment phase
    phase += frequency / SAMPLE_RATE;
  }
}
