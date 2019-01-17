// compile with: c++ wc.cpp -o wc
// CONTROL-D to insert EOF (end of file) character
#include <iostream>  // cin (standard input), cout (standard output), cerr (standard error output)
#include <string>  // std::string type is better than char*
using namespace std;
int main() {
  int lineCount = 0, wordCount = 0, charCount = 0;
  while (true) {
    // printf("Type a sentence (then hit return): ");
    string line;  // line == ""
    getline(cin, line);
    if (!cin.good()) break;

    lineCount++;

    // 1. what is the length of the string

    int length = 0;
    while (true) {
      if (line[length] == '\0') break;
      length = 1 + length;
    }
    charCount += length;
    // length means something
    // printf("the length is %d characters\n", length);

    // 2. calculate the number of words

    int words = 0;
    char last = ' ';
    for (int i = 0; i < length; ++i) {
      //???
      if (line[i] != ' ' && last == ' ') words++;
      last = line[i];
    }
    wordCount += words;
    // printf("it has %d words\n", words);
  }

  printf("\t%d\t%d\t%d\n", lineCount, wordCount, charCount);
}
