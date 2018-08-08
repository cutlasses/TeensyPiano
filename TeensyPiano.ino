#include "SamplePlayer.h"
#include "AudioSamplePiano_c3_44k.h"

AudioInputI2S         audio_input;
SAMPLE_PLAYER_EFFECT  sample_player;
AudioOutputI2S        audio_output;

AudioConnection       patch_cord_1( audio_input, 0, sample_player, 0 );
AudioConnection       patch_cord_2( sample_player, 0, audio_output, 0 );

AudioControlSGTL5000  sgtl5000_1;

void setup()
{
  Serial.begin(9600);

  AudioMemory(64);
  
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.75f);

  //sgtl5000_1.lineInLevel( 10 );  // 0.56volts p-p
  sgtl5000_1.lineOutLevel( 8 );  // 3.16volts p-p

  delay(100);
}

void loop()
{
  const uint16_t* piano_sample = reinterpret_cast<const uint16_t*>(&(AudioSamplePiano_c3_44k[0]));
  sample_player.start( piano_sample, 58945 * 2, 1.0f );

  Serial.println("dong");

  delay( 4000 );
}
