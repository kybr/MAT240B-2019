---
title: Assignments
---

<section>

For each reading assignment, write one page describing your experience[^your_experience] with the reading. Submit a reading response by creating a [Google Document] and sharing it with me. Title your document with the name of the class, your name, and the assignment number. For instance, Frank Zappa might use _MAT201B Frank Zappa Assignment 4_ as the title of his response to reading assignment 4.

[^your_experience]: Answer this question: What is most important thing for _you_ to say about this reading? Was the reading useless?

For each programming assignment, submit your code by pushing it to your [Github] repository for this class. Unless the assignment specifies otherwise, submit your programming assignment solution as a single _.cpp_ placed in a folder named _assignment_.

</section>

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
