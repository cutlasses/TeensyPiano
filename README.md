# TeensyPiano
Piano based sample player

This is an alternate firmware for the Radio Music/Chord Organ by Music Thing Modular. It plays a single piano sample at a pitch controlled by input CV. It has added reverb which you can control the mix of.

## Controls:

### Chord Organ
Root jack - controls the pitch of the sound
Trig jack - will trigger a new note
Root pot - controls the reverb mix
Out jack - the output sound

### Radio Music
Start jack - controls the pitch of the sound
Reset jack - will trigger a new note
Start pot - controls the reverb mix
Out jack - the output sound

The code can quantise the input, currently only to C Major. You can enable and disable this with the QUANTISE_INPUT define in TeenyPiano.ino

For more info on how I wrote this, see http://www.cutlasses.co.uk/tech/piano-music/


Here's a video of it in use
https://youtu.be/umf8ldmmhBE

<a href="http://www.youtube.com/watch?feature=player_embedded&vumf8ldmmhBE
" target="_blank"><img src="http://img.youtube.com/vi/umf8ldmmhBE/0.jpg" 
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="10" /></a>

## Binaries
I've created some binary hex files which can be uploaded. These are currently UNTESTED. I've uploaded versions which don't overclock the Teensy, but this may cause audio drop outs. Please log an issue if you have problems.
