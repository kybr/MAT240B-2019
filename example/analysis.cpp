#include "Gamma/DFT.h"
#include "Gamma/Noise.h"
#include "Gamma/Oscillator.h"
#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include <fstream>
using namespace std;

const int WINDOW_SIZE = 2048;
const int FFT_SIZE = 1 << 18;

struct Peak {
  float magnitude, frequency;
  // float frequency, magnitude;
};

struct Frame {
  vector<Peak> peak;
  float frameTimeSeconds;
  float rootMeanSquare;
  float amplitudeMinimum;
  float amplitudeMaximum;
  float zeroCrossingRate;
  float magnitudeMaximum;
  float magnitudeSum;
  float magnitudeMean;
  float spectralCentroid;
  void print() {
    cout << frameTimeSeconds << ' ';
    cout << rootMeanSquare << ' ';
    cout << amplitudeMinimum << ' ';
    cout << amplitudeMaximum << ' ';
    cout << zeroCrossingRate << ' ';
    cout << magnitudeMaximum << ' ';
    cout << magnitudeSum << ' ';
    cout << magnitudeMean << ' ';
    cout << spectralCentroid << ' ';
    for (auto& p : peak)
      cout << p.frequency << ':' << atodb(p.magnitude) << ' ';
    cout << endl;
  }
};

void load(vector<Frame>& frame, string fileName) {
  vector<float> data;
  float sampleRate;

  {
    SoundFile soundFile;
    if (!soundFile.openRead(fileName)) {
      cout << "failed to open " << fileName << endl;
      exit(-1);
    }
    data.resize(soundFile.frames());
    soundFile.readAll(&data[0]);
    sampleRate = soundFile.frameRate();
  }

  STFT stft(
      WINDOW_SIZE,
      WINDOW_SIZE / 4,         // hop size
      FFT_SIZE - WINDOW_SIZE,  // pad size; appended to window
      HANN,                    // window type:
      MAG_PHASE  // format of frequency samples: COMPLEX, MAG_PHASE, or MAG_FREQ
  );

  // It is important to tell Gamma the sample rate otherwise, it assumes a
  // sample rate of 1 (1 Hz, 1 sample/second)!
  gam::Sync::master().spu(sampleRate);

  for (int i = 0; i < data.size(); ++i)
    if (stft(data[i])) {
      // push a new frame and make an alias, f
      frame.push_back(Frame());
      Frame& f(frame.back());

      // calculate time-domain statistics
      //
      f.frameTimeSeconds = i / sampleRate;
      f.amplitudeMinimum = 0;
      f.amplitudeMaximum = 0;
      for (int j = 0; j < WINDOW_SIZE; ++j) {
        if (data[i - j] > f.amplitudeMaximum) f.amplitudeMaximum = data[i - j];
        if (data[i - j] < f.amplitudeMinimum) f.amplitudeMinimum = data[i - j];
      }
      double t = 0;
      for (int j = 0; j < WINDOW_SIZE; ++j) t += data[i - j] * data[i - j];
      f.rootMeanSquare = sqrt(t / WINDOW_SIZE);
      f.zeroCrossingRate = 0;
      for (int j = 1; j < WINDOW_SIZE; ++j)
        if (data[i - j - 1] * data[i - j] < 0) f.zeroCrossingRate += 1.0;
      f.zeroCrossingRate /= (float)WINDOW_SIZE / SAMPLE_RATE;

      // calculate frequency-domain statistics
      //
      f.magnitudeSum = 0;
      for (int b = 0; b < stft.numBins(); ++b) f.magnitudeSum += stft.bin(b)[0];
      f.magnitudeMaximum = 0;
      for (int b = 0; b < stft.numBins(); ++b)
        if (f.magnitudeMaximum < stft.bin(b)[0])
          f.magnitudeMaximum = stft.bin(b)[0];
      f.magnitudeMean = f.magnitudeSum / stft.numBins();
      f.spectralCentroid = 0;
      for (int b = 0; b < stft.numBins(); ++b)
        f.spectralCentroid += stft.bin(b)[0] * stft.binFreq() * b;
      f.spectralCentroid /= f.magnitudeSum;

      // collect every maxima
      for (int b = 1; b < stft.numBins() - 1; ++b) {
        float m[3]{stft.bin(b - 1)[0], stft.bin(b)[0], stft.bin(b + 1)[0]};
        if (m[1] > m[0] && m[1] > m[2])
          f.peak.push_back({m[1], static_cast<float>(stft.binFreq() * b)});
      }
      // sort maxima by magnitude
      sort(f.peak.begin(), f.peak.end(), [](const Peak& a, const Peak& b) {
        return a.magnitude < b.magnitude;
      });
      // throw away all but N peaks
      f.peak.resize(100);
    }
}

int main(int argc, char* argv[]) {
  string fileName = "../sound/6.wav";
  if (argc > 1) fileName = argv[1];
  vector<Frame> frame;
  load(frame, fileName);
  for (auto& f : frame) f.print();
}