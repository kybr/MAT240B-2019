#include "Gamma/DFT.h"
#include "Gamma/Noise.h"
#include "Gamma/Oscillator.h"
#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

#include <vector>
using namespace std;

const int WINDOW_SIZE = 2048;
const int FFT_SIZE = 1 << 18;
const int N = 35;  // number of peaks

struct Peak {
  float magnitude, frequency;
};

struct Frame {
  vector<Peak> peak;

  float timeSeconds = 0;
  float rootMeanSquare = 0;
  float amplitudeMinimum = 0;
  float amplitudeMaximum = 0;
  float zeroCrossingRate = 0;
  float magnitudeMaximum = 0;
  float magnitudeSum = 0;
  float magnitudeMean = 0;
  float spectralCentroid = 0;

  void print() {
    cout << timeSeconds << ' ';
    cout << amplitudeMinimum << ' ';
    cout << amplitudeMaximum << ' ';
    cout << rootMeanSquare << ' ';
    cout << magnitudeSum << ' ';
    cout << magnitudeMaximum << ' ';
    cout << magnitudeMean << ' ';
    cout << spectralCentroid << ' ';
    cout << zeroCrossingRate << ' ';
    for (auto& p : peak) cout << p.frequency << ':' << p.magnitude << ' ';
    cout << endl;
  }
};

struct Oscillator {
  Phasor phase;
  Line _frequency;
  Line amplitude;

  Oscillator() {
    _frequency.seconds = 0.011;
    amplitude.seconds = 0.011;
  }

  void frequency(float hertz) { _frequency.set(hertz); }
  void level(float db) { amplitude.set(dbtoa(db)); }

  float operator()() {
    phase.frequency(_frequency());
    return amplitude() * sine(phase());
  }
};

struct MyApp : App {
  string fileName;
  MyApp(int argc, char* argv[]) {
    fileName = "../sound/8.wav";
    if (argc > 1) fileName = argv[1];
  }

  float t = 0;

  vector<Oscillator> oscillator;

  vector<Frame> frame;
  void onCreate() override {
    vector<float> sample;
    float sampleRate;

    {
      SoundFile soundFile;
      if (!soundFile.openRead(fileName)) {
        cout << "failed to open " << fileName << endl;
        exit(-1);
      }
      sample.resize(soundFile.frames());
      soundFile.readAll(&sample[0]);
      sampleRate = soundFile.frameRate();
    }

    STFT stft(WINDOW_SIZE, WINDOW_SIZE / 4, FFT_SIZE - WINDOW_SIZE, HANN,
              MAG_PHASE);

    // It is important to tell Gamma the sample rate otherwise, it assumes a
    // sample rate of 1 (1 Hz, 1 sample/second)!
    gam::Sync::master().spu(sampleRate);

    for (int i = 0; i < sample.size(); ++i)
      if (stft(sample[i])) {
        // push a new frame and make an alias, f
        frame.emplace_back();
        Frame& f(frame.back());
        float* s = &sample[i - WINDOW_SIZE];

        f.timeSeconds = i / sampleRate;

        // amplitude extrema
        //
        f.amplitudeMinimum = 0;
        f.amplitudeMaximum = 0;
        for (int j = 0; j < WINDOW_SIZE; ++j) {
          if (s[j] > f.amplitudeMaximum) {
            f.amplitudeMaximum = s[j];
          }
          if (s[j] < f.amplitudeMinimum) {
            f.amplitudeMinimum = s[j];
          }
        }

        // RMS
        double t = 0;
        for (int j = 0; j < WINDOW_SIZE; ++j) {
          t += s[j] * s[j];
        }
        f.rootMeanSquare = sqrt(t / WINDOW_SIZE);

        // ZCR
        f.zeroCrossingRate = 0;
        for (int j = 1; j < WINDOW_SIZE; ++j)
          if (s[j - 1] * s[j] < 0) {
            f.zeroCrossingRate += 1.0;
          }
        f.zeroCrossingRate /= WINDOW_SIZE / sampleRate;

        // sum of all magnitudes
        f.magnitudeSum = 0;
        for (int b = 0; b < stft.numBins(); ++b) {
          f.magnitudeSum += stft.bin(b)[0];
        }

        // largest magnitude
        f.magnitudeMaximum = 0;
        for (int b = 0; b < stft.numBins(); ++b)
          if (f.magnitudeMaximum < stft.bin(b)[0])
            f.magnitudeMaximum = stft.bin(b)[0];

        // average
        f.magnitudeMean = f.magnitudeSum / stft.numBins();

        // center of gravity of the spectrum
        f.spectralCentroid = 0;
        for (int b = 0; b < stft.numBins(); ++b)
          f.spectralCentroid += stft.bin(b)[0] * stft.binFreq() * b;
        f.spectralCentroid /= f.magnitudeSum;

        // collect every maxima
        for (int b = 1; b < stft.numBins() - 1; ++b) {
          float m[3]{stft.bin(b - 1)[0], stft.bin(b)[0], stft.bin(b + 1)[0]};
          if (m[1] > m[0] && m[1] > m[2]) {
            f.peak.emplace_back();
            f.peak.back().magnitude = m[1];
            f.peak.back().frequency = stft.binFreq() * b;
          }
        }

        // sort maxima by magnitude
        sort(f.peak.begin(), f.peak.end(), [](const Peak& a, const Peak& b) {
          return a.magnitude > b.magnitude;
        });
        // throw away all but N peaks
        f.peak.resize(N);

        if (1) {
          f.magnitudeMaximum = atodb(f.magnitudeMaximum);
          f.magnitudeMean = atodb(f.magnitudeMean);
          f.magnitudeSum = atodb(f.magnitudeSum);
          for (auto& p : f.peak) p.magnitude = atodb(p.magnitude);
        }
      }
    // for (auto& f : frame) f.print();
    // exit(1);

    oscillator.resize(N);
  }

  void onAnimate(double dt) override {
    t += dt * 20;
    if (t > frame.size()) t -= frame.size();

    int left = (int)t;
    int right = left + 1;
    if (right >= frame.size()) right -= frame.size();
    float p = t - left;
    cout << left << ' ' << right << ' ' << p << endl;
    Frame &a(frame[left]), b(frame[right]);
    for (int i = 0; i < oscillator.size(); ++i) {
      oscillator[i].frequency(a.peak[i].frequency * (1 - p) +
                              b.peak[i].frequency * p);
      oscillator[i].level(a.peak[i].magnitude * (1 - p) +
                          b.peak[i].magnitude * p + 48);
    }
  }

  void onDraw(Graphics& g) override {
    //
    g.clear(0.25);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = 0;
      for (auto& o : oscillator) f += o();
      io.out(0) = f;
      io.out(1) = f;
    }
  }
};

int main(int argc, char* argv[]) {
  MyApp app(argc, argv);
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, OUTPUT_CHANNELS, INPUT_CHANNELS);
  app.start();
}
