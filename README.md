# TeensyPiano
Piano based sample player

This is an alternate firmware for the Radio Music/Chord Organ by Music Thing Modular. It plays a single piano sample at a pitch controlled by input CV. It has added reverb which you can control the mix of.

##Controls:

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

Here's a video of it in use

For more info on how I wrote this, see http://www.cutlasses.co.uk/tech/piano-music/

https://youtu.be/umf8ldmmhBE



