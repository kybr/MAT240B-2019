#include "everything.h"
using namespace diy;

int main(int argc, char* argv[]) {
  float frequency = 220;
  float phase = 0;

  std::vector<std::string> a(argv, argv + argc);
  if (argc > 2) phase = fmod(stof(a[2]), 2.0f);
  if (argc > 1) frequency = stof(a[1]);

  float perSampleIncrement = frequency / SAMPLE_RATE;
  while (true) {
    phase += perSampleIncrement;
    if (phase > 1) phase -= 1;
    printf("%f\n", phase);
  }
}
