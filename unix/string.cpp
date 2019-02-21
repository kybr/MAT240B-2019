#include "everything.h"
using namespace diy;

struct MeanFilter {
  float _{0};
  float operator()(float f) {
    float v = f + _;
    _ = f;
    return v / 2;
  }
};

int main(int argc, char* argv[]) {
  float frequency = 220;
  if (argc > 1) frequency = atof(argv[1]);

  int N = SAMPLE_RATE / frequency;
  //  printf("you wanted %f Hz\n", frequency);
  //  printf("N = %d\n", N);
  //  printf("we are throwing away %f\n", SAMPLE_RATE / frequency - N);
  //  printf("you are getting %f Hz\n", (float)SAMPLE_RATE / N);

  float delay[N];
  int index = 0;
  for (int i = 0; i < N; ++i)  //
    delay[i] = uniform(-1, 1);
  MeanFilter filter;
  while (true) {
    //
    // read from the delay and...
    float v = delay[index];

    //
    // filter the result and...
    v = filter(v);

    // spectra-independent attenuation
    v *= 0.95;

    //
    // write back to the delay
    delay[index] = v;

    //
    // output the filtered result
    say(v);

    //
    // increment delay index
    index++;
    if (index == N)  //
      index = 0;
  }
}
