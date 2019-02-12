#include "everything.h"
using namespace diy;

#include <vector>
using namespace std;

struct History : vector<float> {
  History(int size = 1) { resize(size); }
  unsigned index{0};  // where to put new values
  float sum{0};
  void operator()(float f) {
    if (size() < 1) exit(1);
    sum -= at(index);
    sum += f;
    at(index) = f;
    index++;
    if (index == size()) index -= size();
  }
  float mean() { return sum / size(); }
};

int main(int argc, char* argv[]) {
  int n = 1;
  if (argc >= 2) n = atoi(argv[1]);

  // this will not go well for infinite input streams;
  // we assume someone used the 'take.exe' program
  vector<float> input;
  float f;
  while (cin >> f) input.push_back(f);

  History history(n);

  for (float f : input) {
    history(f);
    say(history.mean());
  }
}
