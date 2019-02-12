#include "everything.h"
using namespace diy;

#include <vector>
using namespace std;

int main(int argc, char* argv[]) {
  // this will not go well for infinite input streams;
  // we assume someone used the 'take.exe' program
  vector<float> input;
  float f;
  while (cin >> f) input.push_back(f);

  float last_f = 0;
  for (float f : input) {
    say((f + last_f) / 2);
    last_f = f;
  }
}
