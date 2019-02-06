#include "everything.h"
using namespace diy;

int main(int argc, char* argv[]) {
  float phase = 0;

  for (float note = 127; note > 0; note -= 0.001) {
    float frequency = mtof(note);
    float harmonicCount = (SAMPLE_RATE / 2) / frequency;
    float v = 0;

    // fade in the highest harmonic
    //
    int h = harmonicCount;
    float t = harmonicCount - h;
    v += t * sin(h * phase);

    // add each lower harmonic
    //
    for (int h = 1; h < harmonicCount - 1; ++h)
      //
      v += sin(h * phase);

    // weight each harmonic equally
    //
    v /= harmonicCount;

    // output this sample
    //
    say(v * 0.707);

    // increment phase
    //
    phase += 2 * pi * frequency / SAMPLE_RATE;
    if (phase > 2 * pi) phase -= 2 * pi;
  }
}
