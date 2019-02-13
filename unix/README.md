# UNIX-y programs for learning C++ and audio programming

- <http://wiki.c2.com/?UnixWay>
- <https://en.wikipedia.org/wiki/Unix_philosophy>

Generally, these programs generate or operate on streams of floating point numbers in human-readable (ASCII) form.

    phasor.cpp   Generate a phasor
    sine.cpp     Generate a sinuoid
    uniform.cpp  Generate uniformly random numbers
    take.cpp     Only pass on so many inputs
    sort.cpp     Reorganize into ascending or descending order
    info.cpp     Show statistics: count, mean, minimum, maximum
    reverse.cpp  Reverse the order of numbers
    fade.cpp     Fade in/out a stream

Build a program like this:

    make

## PATH

While it is considered naughty and insecure, it might help to add this folder to you path:

    export PATH=$PATH:`pwd`

## Stream types

- infinite streams and `take.exe`
- stream format:
  + one line per sample "frame"
  + whitespace separated floating-point numbers as strings
  + one float per channel: left, right
