#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include "Gamma/DFT.h"
#include "Gamma/SamplePlayer.h"
#include "Gamma/SoundFile.h"
// using namespace gam;

#include <iostream>
#include <string>
#include <vector>
using namespace std;

gam::STFT stft =
    gam::STFT(2048, 2048 / 4, (1 << 18) - 2048, gam::HANN, gam::MAG_PHASE);

const int N = 34;

struct Osc : Phasor {
  float magnitude{1};
  float operator()() { return magnitude * sine(Phasor::operator()()); }
};

struct Frame {
  Osc osc[N];

  void print() {
    for (int i = 0; i < N; ++i)  //
      cout << osc[i].magnitude << ':' << osc[i].frequency() << ' ';
    cout << endl;
  }
};

float interpolate(Frame& a, Frame& b, float t) {
  float f = 0;
  for (int i = 0; i < N; ++i)  //
    f += (1 - t) * a.osc[i]() + t * b.osc[i]();
  return f;
}

struct MyApp : App {
  string fileName;
  MyApp(vector<string>& argument) {
    if (argument.size() > 1)
      fileName = argument[1];
    else
      fileName = "../sound/8.wav";
  }

  Phasor index;
  float maximum{0};
  vector<Frame> frame;

  void onCreate() override {
    gam::SoundFile soundFile;
    if (!soundFile.openRead(fileName)) exit(1);
    vector<float> data;
    data.resize(soundFile.frames());
    soundFile.readAll(&data[0]);

    // set the sample rate for Gamma objects
    // this is *critical*; the default ia 1 Hz!!!
    gam::Sync::master().spu(soundFile.frameRate());

    struct Peak {
      float magnitude, frequency;
    };

    // for each sample...
    //
    for (int i = 0; i < data.size(); ++i) {
      // calculate the FFT
      //
      if (stft(data[i])) {
        // 1. find all maxima
        //
        vector<Peak> peak;
        for (int i = 1; i < stft.numBins() - 1; ++i) {
          if ((stft.bin(i)[0] > stft.bin(i + 1)[0]) &&
              (stft.bin(i)[0] > stft.bin(i - 1)[0])) {
            // we found a maxima! save it
            //
            peak.push_back(
                {.magnitude = stft.bin(i)[0],
                 .frequency = static_cast<float>(stft.binFreq() * i)});
          }
        }

        // 2. sort by magnitude in descending order
        //
        sort(peak.begin(), peak.end(),  //
             [](const Peak& a, const Peak& b) {
               return a.magnitude > b.magnitude;
             });

        // 3. throw away all but the first N peaks
        //
        peak.resize(N);

        // 4. sort by frequency
        //
        sort(peak.begin(), peak.end(),  //
             [](const Peak& a, const Peak& b) {
               return a.frequency < b.frequency;
             });

        // initialize oscillators
        //
        frame.emplace_back();
        for (int i = 0; i < N; ++i) {
          frame.back().osc[i].magnitude = peak[i].magnitude;
          frame.back().osc[i].frequency(peak[i].frequency);
          frame.back().osc[i].phase = rnd::uniform(1.0f);  // randomize phase

          if (maximum < peak[i].magnitude)  //
            maximum = peak[i].magnitude;
        }
      }
    }

    index.period(21);
    // for (auto& f : frame) f.print();
    // exit(1);
  }

  Array recording;
  void onSound(AudioIOData& io) override {
    while (io()) {
      float t = index() * frame.size();
      if (t > frame.size()) t -= frame.size();
      int left = (int)t;
      int right = left + 1;
      if (right >= frame.size()) right -= frame.size();
      float p = t - left;
      float s = interpolate(frame[left], frame[right], p) / maximum;
      s /= 4;
      recording(s);
      io.out(0) = s;
      io.out(1) = s;
    }
  }

  void onKeyDown(const Keyboard& k) override {
    recording.save("analysis.wav");
    exit(1);
  }
};

int main(int argc, char* argv[]) {
  // example of using the "c++ way" rather than the "C way"
  vector<string> argument(argv, argv + argc);
  MyApp app(argument);
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}