#include "everything.h"
using namespace diy;

int main(int argc, char* argv[]) {
  float frequency = 220;
  if (argc > 1) frequency = atof(argv[1]);
  float phase = 0;
  float increment = frequency / SAMPLE_RATE;
  int harmonicCount = 0;
  while (frequency * harmonicCount < SAMPLE_RATE / 2) harmonicCount++;
  float v;
  while (true) {
    v = 0;
    for (int h = 0; h < harmonicCount; ++h)
      //
      v += cos(2 * pi * h * phase);  // unipolar
    // v += sin(2 * pi * h * phase); // bipolar
    v /= harmonicCount;
    say(v);

    // increment phase
    phase += increment;
  }
}
