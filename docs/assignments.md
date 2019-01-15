---
title: Assignments
---

<section>

For each reading assignment, write one page describing your experience[^your_experience] with the reading. Submit a reading response by creating a [Google Document] and sharing it with me. Title your document with the name of the class, your name, and the assignment number. For instance, Frank Zappa might use _MAT201B Frank Zappa Assignment 4_ as the title of his response to reading assignment 4.

[^your_experience]: Answer this question: What is most important thing for _you_ to say about this reading? Was the reading useless?

For each programming assignment, submit your code by pushing it to your [Github] repository for this class. Unless the assignment specifies otherwise, submit your programming assignment solution as a single _.cpp_ placed in a folder named _assignment_.

</section>

## Assignment 1 | Due 2019-01-21 by 1700

With Chowning's 1973 paper in mind, study _Synthesis of Quasi-Bandlimited Analog Waveforms Using Frequency Modulation_ (Schoffhauzer 2005). This FM technique establishes parameters---Consider referring to [parameterization](https://en.wikipedia.org/wiki/Parametrization_(geometry)). With Stilson's 1996 paper in mind, study _Hard Sync Without Aliasing_ (Brandt 2001); How does this paper compare to Stilson's? Finally, read chapters 1 and 2 of _Numerical Sound Synthesis_ (Bilbao 2009).

Write a C++ program that calculates word frequency and produces a list of unique words, sorted by frequency (most frequent words first). Your program should read in plain-text files such as books found on [Project Guttenberg](https://www.gutenberg.org). For instance, [The Mysterious Stranger](https://www.gutenberg.org/files/50109/50109-0.txt) (for me) results in this output:

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
