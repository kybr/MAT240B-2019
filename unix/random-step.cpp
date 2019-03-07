#include "everything.h"
using namespace diy;

//
float rnd(float high, float low = 0) {
  return low + (high - low) * float(rand()) / RAND_MAX;
}

int main(int argc, char* argv[]) {
  // high and low determine the rate of advance of "phase"
  float low = 0.1;
  float high = 1;

  // argument 3: int that seeds the random numbers
  if (argc > 3)
    srand(atoi(argv[3]));
  else
    srand(0);

  // argument 1 and 2 are high and low, respectively
  if (argc > 2) high = atof(argv[2]);
  if (argc > 1) low = atof(argv[1]);

  while (true) {
    float r = rnd(1, -1);      // random value to output
    float s = rnd(high, low);  // seconds to hold the value
    for (int j = 0; j < int(s * SAMPLE_RATE); j++)
      //
      say(r);
  }
}
