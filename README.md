# WinAPI MIDI Playback
I wrote this file for those also interested in learning about reading/sending MIDI input/output and MIDI sound manipulation. Please take a look at the .c file for a more descriptive explanation on the various steps to be able to hear back audio coming from a MIDI input device. In my experiment I used a Williams Allegro electronic piano to read in MIDI messages, which were then played back via Microsfot's GS Wavetable Synth. I have read online that this is not the best synth out there, but I was more interested in the procedure to get from reading MIDI messages to making them audible. I hope this serves as a good starting point to others just like it served me. 

Compiled using MinGW as: gcc <filename>.c -lwinmm -o midiPlayback 

