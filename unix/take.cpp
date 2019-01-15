#include "everything.h"
using namespace diy;

// accept and pass on only so many inputs

int main(int argc, char* argv[]) {
  std::vector<std::string> a(argv, argv + argc);
  if (argc < 2) {
    printf("take <n> # take n values from the input stream");
    return 1;
  }

  unsigned many = stoi(a[1]);

  char buffer[256];
  while (many > 0) {
    many--;
    scanf("%s", buffer);
    printf("%s\n", buffer);
  }
}
