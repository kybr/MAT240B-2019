#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include "Gamma/DFT.h"
#include "Gamma/SamplePlayer.h"
#include "Gamma/SoundFile.h"
using namespace gam;

#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int WINDOW_SIZE = 2048;
const int FFT_SIZE = 1 << 13;

STFT stft =
    STFT(WINDOW_SIZE, WINDOW_SIZE / 4, FFT_SIZE - WINDOW_SIZE, HANN, MAG_PHASE);

const int N = 3;

struct Peak {
  float magnitude, frequency;
};

struct Frame {
  vector<Peak> peakList;
  void print() {
    for (Peak& p : peakList)  //
      cout << p.magnitude << ':' << p.frequency << ' ';
    cout << endl;
  }
};

struct MyApp : App {
  string fileName;
  MyApp(int argc, char* argv[]) {
    // example of using the "c++ way" rather than the "C way"
    vector<string> argumentList(argv, argv + argc);
    if (argumentList.size() > 1)
      fileName = argumentList[1];
    else
      fileName = "../sound/8.wav";
  }

  vector<Frame> frameList;
  void onCreate() {
    gam::SoundFile soundFile;
    if (!soundFile.openRead(fileName)) {
      cerr << "has died" << endl;
      exit(1);
    }
    vector<float> data;
    data.resize(soundFile.frames());
    soundFile.readAll(&data[0]);
    gam::Sync::master().spu(
        soundFile.frameRate());  // XXX otherwise sample rate is 1Hz

    // for each sample...
    for (int i = 0; i < data.size(); ++i) {
      if (stft(data[i])) {
        // 0. add an empty/default frame to the list of frames
        //
        frameList.emplace_back();
        Frame& frame(frameList.back());  // make an alias

        // 1. find all maxima
        //
        for (int i = 1; i < stft.numBins() - 1; ++i) {
          if ((stft.bin(i)[0] > stft.bin(i + 1)[0]) &&
              (stft.bin(i)[0] > stft.bin(i - 1)[0])) {
            // we found a maxima!
            frame.peakList.push_back(
                {.magnitude = stft.bin(i)[0],
                 .frequency = static_cast<float>(stft.binFreq() * i)});
          }
        }

        // 2. sort by magnitude in descending order
        //
        sort(frame.peakList.begin(), frame.peakList.end(),
             [](const Peak& a, const Peak& b) {
               return a.magnitude > b.magnitude;
             });

        // 3. throw away all but the first N peaks
        //
        frame.peakList.resize(N);

        // to test, run this on a sine sweep and look at the output
        frame.print();
      }
    }
  }

  void onSound(AudioIOData& io) {
    while (io()) {
      float s = 0;
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main(int argc, char* argv[]) {
  MyApp app(argc, argv);
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
