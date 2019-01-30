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

int main(int argc, char* argv[]) {
  string fileName = "../sound/6.wav";
  if (argc > 1) fileName = argv[1];
  SoundFile soundFile;
  if (!soundFile.openRead(fileName)) {
    cout << "failed to open " << fileName << endl;
    exit(-1);
  }
  vector<float> data;
  data.resize(soundFile.frames());
  soundFile.readAll(&data[0]);

  STFT stft(
      2048,      // window size
      2048 / 4,  // hop size; number of samples between transforms
      8192 -
          2048,  // pad size; number of zero-valued samples appended to window
      HANN,  // window type: BARTLETT, BLACKMAN, BLACKMAN_HARRIS, HAMMING, HANN,
             // WELCH, NYQUIST, or RECTANGLE
      MAG_PHASE  // format of frequency samples: COMPLEX, MAG_PHASE, or MAG_FREQ
  );

  // XXX it is important to tell Gamma the sample rate
  // otherwise, it assumes a sample rate of 1 (1 Hz, 1 sample/second)
  gam::Sync::master().spu(SAMPLE_RATE);

  ofstream output(fileName + "-analysis.txt", ios::trunc);

  for (int i = 0; i < data.size(); ++i)
    if (stft(data[i])) {
      // say where we are in the sound clip
      output << (float)i / SAMPLE_RATE << " ";

      // compute some stats
      //
      double magnitudeTotal = 0, magnitudeMaximum = 0;
      for (int b = 0; b < stft.numBins(); ++b) {
        magnitudeTotal += stft.bin(b)[0];
        if (magnitudeMaximum < stft.bin(b)[0])
          magnitudeMaximum = stft.bin(b)[0];
      }
      output << magnitudeTotal << " ";
      float magnitudeMean = magnitudeTotal / stft.numBins();
      output << magnitudeMean << " ";
      output << magnitudeMaximum << " ";

      // find peaks, sort by magnitude
      //
      struct Peak {
        double magnitude, frequency;
      };
      vector<Peak> peakList;
      for (int b = 1; b < stft.numBins() - 1; ++b) {
        float m[3]{stft.bin(b - 1)[0], stft.bin(b)[0], stft.bin(b + 1)[0]};
        if (m[1] > m[0] && m[1] > m[2])
          peakList.push_back({m[1], stft.binFreq() * b});
      }
      sort(peakList.begin(), peakList.end(), [](const Peak& a, const Peak& b) {
        return a.magnitude > b.magnitude;
      });
      for (int i = 0; i < min(10, (int)peakList.size()); ++i)
        output << peakList[i].frequency << ":" << peakList[i].magnitude << " ";
      output << endl;
    }
}