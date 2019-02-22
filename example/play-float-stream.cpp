#include "al/core.hpp"
using namespace al;

#include "synths.h"  // for samples rate
using namespace diy;

#include <deque>
using namespace std;

deque<float> q;  // a FIFO (First-In First-Out) of floats

int dropoutCount = 0, _ = 0;

struct MyApp : AudioApp {
  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = 0;

      // if there is a sample in the queue, pop it out
      //
      if (q.empty()) {
        dropoutCount++;
      } else {
        f = q.back();
        q.pop_back();
      }

      io.out(0) = f;
      io.out(1) = f;
    }

    if (dropoutCount > _) {
      _ = dropoutCount;
      printf("Dropout! %d\n", dropoutCount);
    }
  }
};

int main() {
  MyApp app;
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.beginAudio();  // non-blocking cal to start the audio thread

  // readin the standard input, float by float, enque each sample
  //
  float f;
  while (!cin.eof()) {
    cin >> f;
    q.push_front(f);
  }
}
