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

struct Foo {
  int i = 0;
  int j;
  int k;
  Foo() : k(0) { j = 0; }
};

struct MyApp : App {
  string fileName;
  // STFT(
  //  unsigned winSize = 1024,
  //  unsigned hopSize = 256,
  //  unsigned padSize = 0,
  //  WindowType winType = RECTANGLE,
  //  SpectralType specType = COMPLEX,
  //  unsigned numAux = 0
  // 	)
  STFT stft = STFT(WINDOW_SIZE, WINDOW_SIZE / 4, FFT_SIZE - WINDOW_SIZE, HANN,
                   MAG_PHASE);

  MyApp(int argc, char* argv[]) {
    vector<string> argumentList(argv, argv + argc);
    if (argumentList.size() > 1)
      fileName = argumentList[1];
    else
      fileName = "../sound/8.wav";
  }

  Noise noise;

  SoundPlayer soundPlayer;
  void onCreate() {
    SoundFile soundFile;
    if (!soundFile.openRead(fileName)) {
      cerr << "has died" << endl;
      exit(1);
    }
    vector<float> v;
    v.resize(soundFile.frames());
    soundFile.readAll(&v[0]);
    soundPlayer.load(&v[0], v.size(), soundFile.frameRate());
  }

  void onSound(AudioIOData& io) {
    while (io()) {
      if (stft(soundPlayer())) {
        // mess with the spectrum
        for (int i = 10; i < stft.numBins(); ++i) {
          // stft.bin(i)[1] = 0;  // reset phase
          // stft.bin(i)[1] = noise() * M_PI;
          stft.bin(i)[0] = stft.bin(i - 10)[0];
        }
        for (int i = 0; i < 10; ++i) {
          stft.bin(i)[0] = 0;
        }
      }
      float s = stft();
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
