## MAT 240B: Digital Audio Programming - Synthesis and Processing

> Instructor: Karl Yerkes <karl.yerkes@gmail.com>
>
> TA: Diarmid Flatley <diarmid@umail.ucsb.edu>
>

This course surveys established techniques for audio synthesis and digital signal processing applied to sound. We explore and implement additive, subtractive, frequency modulation and granular synthesis techniques and we cover digital audio effects such as reverb, filter, phase vocoder, and dynamic range compression.

Languages, frameworks, and tools:

- C++: Gamma, SDL2, AlloSystem
- Cycling '74 Max and gen~
- Javascript: genish.js and gibber
- DAW: Adobe Audition and/or Audacity


### Readings

Students may read and report on excerpts from the following materials:

- _Computer Music: Synthesis, Composition, and Performance_ (Dodge and Jerse 1985/1997)
- _Microsound_ (Roads 2001)
- _Real Sound Synthesis for Interactive Applications_ (Cook 2002)
- _The Theory and Technique of Electronic Music_ (Pucket 2006)
- _Numerical Sound Synthesis: Finite Difference Schemes and Simulation in Musical Acoustics_ (Bilboa 2009)
- _The Musical Art of Synthesis_ (McGuire and Nathan Van der Rest 2016)
- _The Audio Programming Book_ (Boulanger and Lazzarini, eds. 2011)
- _Designing Sound_ (Farnell 2010)

- _The Synthesis of Complex Audio Spectra by Means of Frequency Modulation_ (Chowning 1973)
- _Alias-Free Digital Synthesis of Classic Analog Waveforms_ (Stilson and Smith 1996)
- _Hard sync without aliasing_ (Brandt 2001)
- _Oscillator and Filter Algorithms for Virtual Analog Synthesis_ (Välimäki and Huovilainen 2006)
- _Adaptive Phase Distortion Synthesis_ (Lazzarini and Välimäki 2009)
- _Phase and Amplitude Distortion Methods for Digital Synthesis of Classic Analogue Waveforms_ (Timoney1 2009)
- _Synthesis of Quasi-Bandlimited Analog Waveforms Using Frequency Modulation_ (Schoffhauzer 2007?)



## Final project

Each student is expected to develop a practical application or exploration of one or more of the techniques we present, showing understanding of a particular synthesis or processing technique. The final project consists of open source software and documentation.


### Topics

Time permitting, we discuss subset of the following:

- Concepts
  + Music: Pitch, Timbre, Intensity, and Duration
  + Physical: Frequency, Spectrum, Pressure, Time
  + Sine, Impulse, Sawtooth, Square, Triangle
  + A/DAC, Sampling, Aliasing, and Band-limited oscillators
  + Signal arithmetic: Mixing, Inversion, Modulation,
  + Envelopes: AD, ASR, ADSR
  + Waveshaping
  + Feedback
- Synthesis techniques
  + Subractive
  + Additive
  + Analysis/Resynthesis ([AARS](http://arss.sourceforge.net/examples.shtml), [SPEAR](http://www.klingbeil.com/spear/))
  + Frequency Modulation (DX-7)
  + Amplitude and Phase Modulation
  + Hard-Sync and "Ring" Modulation
  + Phase distortion (CZ-1)
  + One line of C
  + Delay-Lines
  + Granular
  + Physical Modeling (VL-1)
  + Spectrally-matched click
- Processing techniques
  + Filtering
    * Lowpass, highpass, bandpass, bandreject
  + Reverb/Delay
  + Pitch shifting / Time stretching
    * Phase Vocoder
    * Vocoder (Spectral Cross-modular)
  + Dynamic Range Compression
    * Side-chaining
  + Perceptual Coding (mp3)
- Fourier Series
  + [Triangle](http://mathworld.wolfram.com/FourierSeriesTriangleWave.html)
  + [Sawtooth](http://mathworld.wolfram.com/FourierSeriesSawtoothWave.html)
  + [Square](http://mathworld.wolfram.com/FourierSeriesSquareWave.html)
  + [Impulse](http://mathworld.wolfram.com/DeltaFunction.html)
- [Gibbs Phenomenon](http://mathworld.wolfram.com/GibbsPhenomenon.html)
- [Discontinuity](http://mathworld.wolfram.com/Discontinuity.html)


## Homework, Labs, and Reading

Homework, reading reports/quizzes, and lab work count for approximately 50% of course grades. We assign these roughly once per week over the first 5-8 weeks of the quarter.


## Final Projects

Final projects count for approximately 50% of your course grade. Each course requires you to submit a final project in the form of an original, open source software project as well as the proposal, documentation, and public demonstration thereof. With this project, you demonstrate your understanding of the subject.

Final project grades are spread out over several "milestones" that start early in the quarter.


### Final Project Milestones

+ Week 4:
  * Present a 45-second in class "[elevator pitch](https://en.wikipedia.org/wiki/Elevator_pitch)" promoting your project
  * Meet with me for 30 minutes to workshop your project
+ Week 5:
  * Submit a 2-page written project proposal with diagrams/sketches, a title, and links to similar (or inspirational) work
  * Meet with me for 30 minutes to review your proposal
+ Week 6:
  * Choose a [software license](https://opensource.org/licenses) for your code and a [Creative Commons license](https://creativecommons.org/share-your-work) for your work (see [Free Culture](https://en.wikipedia.org/wiki/Free_Culture_(book)))
  * Demonstrate in class a preliminary software sketch related to your project
  * Submit your updated and final project proposal based on my criticism
+ Week 7:
  * Demonstrate in class a significantly improved software sketch related to your project
+ Week 8:
  * Meet with me for 30 minutes to demonstrate and workshop your final project and review your source code
+ Week 9:
  * Submit a draft of your 3-page final project report describing the project's questions, challenges, successes, and failures
+ Week 10:
  * Submit your 3-page final project report
  * Meet with me for 30 minutes review your report
+ Week 11:
  * Demonstrate your final project to the class and the MAT community
  * Submit all final project materials for a grade; This includes:
    - Complete source code
    - Resources (i.e., data, images, sounds, etc.)
    - Prominent markings of software and Creative Commons licenses
    - Documentation explaining how to build, install, and run
    - Complete list of dependencies (software libraries, tools, apps, etc.)
    - Final version of your 3-page project report

Drafts of written deliverables must be submitted as [Google Docs](https://docs.google.com) so that I may make edits, annotations, and suggestions. Submit final versions of written deliverables as PDFs; These should meet academic standards, and faculty may read them.

Final submission of project materials must take the form of an email containing a link to a single ZIP file hosted by a cloud service. The email subject must be "MAT X Final Project" where X is the identifier of the course (e.g., 240B, 201B, etc.).

