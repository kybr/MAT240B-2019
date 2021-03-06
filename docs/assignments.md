---
title: Assignments
---

<section>

For each reading assignment, write one page describing your experience[^your_experience] with the reading. Submit a reading response by creating a [Google Document] and sharing it with me. Title your document with the name of the class, your name, and the assignment number. For instance, Frank Zappa might use _MAT201B Frank Zappa Assignment 4_ as the title of his response to reading assignment 4.

[^your_experience]: Answer this question: What is most important thing for _you_ to say about this reading? Was the reading useless?

For each programming assignment, submit your code by pushing it to your [Github] repository for this class. Unless the assignment specifies otherwise, submit your programming assignment solution as a single _.cpp_ placed in a folder named _assignment_.

</section>

## Assignment 5 | Due 2019-02-20 by 1700

No reading.

Submit a 2-page writen proposal, describing your final project _as if it already exists_. Include screenshots, diagrams, hand-drawn sketches, links to projects, videos and other media that insipires and/or informs your project. This is a writing assignment; Submit it as you would a reading response.

Develop a preliminary sketch (that means running c++ code) that demonstrates some aspect of your final project. Push this to code to your repo as you would for a programming homework. Be prepared to demonstrate this sketch in class on the 21st.


## Assignment 4 | Due 2019-02-13 by 1700

Read/Skim chapters 5-7 of _Designing Audio Effect Plug-Ins in C++ With Digital Audio Signal Processing Theory_ (Pirkle 2013). Pay particular attention to 1) the z-transform and simple filter designs that use it and 2) delays and circular buffers.

Read _[Karplus-Strong string synthesis]_ and _[Karplus Strong Algorithm]_ and implement Karplus-Strong:

1. Implement the most-basic, barely-functional Karplus-Strong string you can. It does not have to change pitch; It does not even have to support tuning to arbitrary pitch. Make the implementation in the offline, UNIX way, like the other algorithms in the `unix/` folder. Name this solution _string-p1.cpp_.
2. Implement Karplus-Strong using allolib and the offerings in our _synths.h_ header. Make sure this version supports triggering "plucks" and setting the fundamental frequency of the synth. Call this solution _string-p2.cpp_.
3. Do your own thing. Name this code _string-p3.cpp_. Add or change some element of Karplus-Strong. For instance, use a different kind of filter, allow for dynamically changing the "listening position", or introduce some sort of dynamic range compression to generate a "guitar feedback" sound.

To provide context for part 2, imagine this starter code:

``` {#lst:starter_code_karplus_string}

#include "Gamma/SoundFile.h"
using namespace gam;

#include "al/core.hpp"
using namespace al;

#include "synths.h"
using namespace diy;

struct KarplusStrong {
  // frequency
  //
  void pluck() {
    // what to do here?
    //
    // when is this?
    // what time context is this called in?
  }

  void frequency(float hertz) {
    // do something
  }

  void decay(float seconds) {
    // do something
  }

  //
  float operator()() {
    return 0;  // this is wrong
    //
  }
};

struct OurApp : App {
  Edge edge;
  KarplusStrong string;
  void onCreate() {
    edge.period(1);
    string.frequency(220);
  }
  void onSound(AudioIOData& io) override {
    while (io()) {
      if (edge()) {
        string.pluck();
        //
      }

      float f = string();
      io.out(0) = f;
      io.out(1) = f;
    }
  }
};
```

[Karplus-Strong string synthesis]: https://en.wikipedia.org/wiki/Karplus%E2%80%93Strong_string_synthesis
[Karplus Strong Algorithm]: https://ccrma.stanford.edu/~jos/pasp/Karplus_Strong_Algorithm.html


## Assignment 3 | Due 2019-02-04 by 1700

Read chapter 9 of _Numerical Sound Synthesis_ (Bilbao 2009) on acoustic tubes and chapter 4 of _Real Sound Synthesis for Interactive Applications_ (Cook 2002) on modal synthesis.

Write a C++ program that executes a bandlimited impulse sweep. Copy the code below to a file named _impulse-sweep.cpp_ and put it in the `unix/` folder of the course repository.

``` {#lst::impulse_sweep .cpp}
#include "everything.h"
using namespace diy;

int main(int argc, char* argv[]) {
  // calculate and output a bandlimited impulse whose
  // frequency starts near Nyquist and ends near midi 0.

  // make all calculations per-sample. determine the
  // current frequency, the number of harmonics to use,
  // and the current amplitude. increment phase.

  // output a finite number of floats, one float per line
  say(0.0); // use this function

  // use other functions and constants from everything.h
}
```

Build and run this code on the terminal, execute the commands below:

    cd path/to/MAT240B-2019
    cd unix
    make
    ./impulse-sweep.exe | ./write
    open out.wav

Investigate the waveform and spectrum of the output file. Is it what you expect? If not, why not?

Next, modify the code to make a) a sawtooth (`sawtooth-sweep.cpp`) and b) a square wave (`square-sweep.cpp`).



## Assignment 2 | Due 2019-01-28 by 1700

Read chapters 5 and 6 of _Real Sound Synthesis for Interactive Applications_ (Cook 2002), _Software for Spectral Analysis, Editing, and Synthesis_ (Klingbeil 2005), and _...a comprehensive list of window functions..._ (Heinzel 2002). The concepts and practical information in these reading are critical to our next subject, analysis/resynthesis, so make sure to do these readings.


Write a C++ program named _extract\_links.cpp_ that uses `std::regex` to identify and extract all the links from HTML given on the standard input. Each link has a _name_ and a _target_. Print these as "name --> target" on the standard output in the order they are found. For instance, if we run our program on the SPEAR website:

    c++ -std=c++11 extract_links.cpp -o extract_links
    curl http://www.klingbeil.com/spear/ | ./extract_links

We might get this output on the terminal:

    SPEAR home --> /spear/
    help --> /spear/analysis.html
    downloads --> /spear/downloads/
    main --> /
    vitae --> /cv.html
    music --> /music.html
    photos --> /photos/
    links --> /connect.html
    Click here to download --> downloads
    Click here to download --> downloads
    downloads --> downloads
    downloads --> downloads
    downloads --> downloads
    Help page --> analysis.html
    ICMC paper “Software for Spectral Analysis, Editing, and Synthesis.” --> /papers/spearfinal05.pdf
    <i>Spectral Analysis, Editing, and Resynthesis: Methods and Applications</i> --> /data/Klingbeil_Dissertation_web.pdf
    download page --> downloads/
    SPEAR home --> /spear/
    help --> /spear/analysis.html
    downloads --> /spear/downloads/
    main --> /
    vitae --> /cv.html
    music --> /music.html
    photos --> /photos/
    links --> /connect.html

Use this code to start:

``` {#lst:histogram .cpp}
#include <regex> // regex
#include <iostream>
using namespace std;
int main() {
  while (true) {
    string line;
    getline(cin, line);

    // do things
  }
}
```

Read these: [regex](https://en.cppreference.com/w/cpp/regex), ["raw" string literal](https://en.cppreference.com/w/cpp/language/string_literal), [match_results](https://en.cppreference.com/w/cpp/regex/match_results), [Regular Expressions](https://en.wikipedia.org/wiki/Regular_expression).


## Assignment 1 | Due 2019-01-21 by 1700

With Chowning's 1973 paper in mind, study _Synthesis of Quasi-Bandlimited Analog Waveforms Using Frequency Modulation_ (Schoffhauzer 2005). This FM technique establishes parameters---Consider referring to [parameterization](https://en.wikipedia.org/wiki/Parametrization_(geometry)). With Stilson's 1996 paper in mind[^blit], study _Hard Sync Without Aliasing_ (Brandt 2001); How does this paper compare to Stilson's? Finally, read chapters 1 and 2 of _Numerical Sound Synthesis_ (Bilbao 2009).

[^blit]: This text on [BLIT integration] might help with understanding Stilson 1996.

Write a C++ program named _word\_histogram.cpp_ that calculates word frequency and produces a list of unique words, sorted by frequency (most frequent words first). Your program should read in plain-text files such as books found on [Project Guttenberg](https://www.gutenberg.org). For instance, [The Mysterious Stranger](https://www.gutenberg.org/files/50109/50109-0.txt) (for me) results in this output:

    1649:the
    926:to
    854:of
    802:a
    714:was
    548:in
    548:he
    513:it
    429:that
    425:I
    372:not
    344:for
    324:we
    303:his
    298:is
    294:with
    289:had
    285:but
    277:you
    237:were
    195:as
    192:him
    182:her
    178:be
    177:have
    172:they
    169:He
    169:would
    ...

To compile and execute the code and get this result printed to the terminal, we use a few commands on the terminal as well as [redirection]:

    curl http://www.gutenberg.org/files/50109/50109-0.txt > The-Mysterious-Stranger.txt
    c++ word_histogram.cpp -o word_histogram
    ./word_histogram < The-Mysterious-Stranger.txt

You will almost certainly need to refer to documentation these: [pair](http://www.cplusplus.com/reference/utility/pair), [unorded_map](http://www.cplusplus.com/reference/unordered_map/unordered_map), and [sort](http://www.cplusplus.com/reference/algorithm/sort). Start with the code below:

``` {#lst:histogram .cpp}
#include <algorithm>      // sort
#include <iostream>       // cin, cout, printf
#include <unordered_map>  // unordered_map
#include <vector>         // vector
using namespace std;
int main() {
  unordered_map<string, unsigned> dictionary;

  string word;
  while (cin >> word) {

    // put your counting code here
    //
    // use dictionary.find, dictionary.end, and dictionary[]
  }

  vector<pair<string, unsigned>> wordList;
  for (auto& t : dictionary) wordList.push_back(t);

  // put your sorting code here
  //
  // use the sort function

  for (auto& t : wordList) printf("%u:%s\n", t.second, t.first.c_str());
}
```

## Assignment 0 | Due 2019-01-14 by 1700

Read and study _The Synthesis of Complex Audio Spectra by Means of Frequency Modulation (Chowning 1973)_ and _Alias-Free Digital Synthesis of Classic Analog Waveforms (Stilson 1996)_

Write a C++ program that accepts sentences typed on the terminal, printing each sentence with its words reversed[^words_reversed]. You may start with the code below.

[^words_reversed]: Reverse each word of the sentence individually. For instance: "this is the truth" becomes "siht si eht hturt".

``` {#lst:starter_code .cpp}
#include <iostream>
#include <string>
using namespace std;
int main() {
  while (true) {
    printf("Type a sentence (then hit return): ");
    string line;
    getline(cin, line);
    if (!cin.good()) {
      printf("Done\n");
      return 0;
    }

    // put your code here
    //
  }
}
```

To submit this assignment, place your solution in a file named _reverse_words.cpp_ and put the file in a folder named _assignment_ in your github repository. See [how to git] for more information.

[Google Document]: https://drive.google.com
[Github]: https://github.com
[how to git]: how-to-git.html
[BLIT integration]: http://musicdsp.org/files/waveforms.txt
[redirection]: https://www.tutorialspoint.com/unix/unix-io-redirections.htm
