#include "SamplePlayer.h"
#include "AudioSamplePiano_c3_44k.h"

SAMPLE_PLAYER_EFFECT  sample_player;

void setup() {
  // put your setup code here, to run once:

  // TEMP
  const uint16_t* piano_sample = reinterpret_cast<const uint16_t*>(&(AudioSamplePiano_c3_44k[0]));
  sample_player.start( piano_sample, 1.0f );
}

void loop() {
  // put your main code here, to run repeatedly:

}
