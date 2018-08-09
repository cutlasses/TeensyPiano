#include "SamplePlayer.h"
#include "AudioSamplePiano_c3_44k.h"

constexpr int         NUM_VOICES(2);

AudioInputI2S         audio_input;
SAMPLE_PLAYER_EFFECT  sample_player_1;
SAMPLE_PLAYER_EFFECT  sample_player_2;
SAMPLE_PLAYER_EFFECT  sample_player_3;
SAMPLE_PLAYER_EFFECT  sample_player_4;
AudioMixer4           sample_mixer;
AudioOutputI2S        audio_output;

//AudioConnection       patch_cord_1( audio_input, 0, sample_player_1, 0 );
AudioConnection       patch_cord_1( sample_player_1, 0, sample_mixer, 0 );
AudioConnection       patch_cord_2( sample_player_2, 0, sample_mixer, 1 );
AudioConnection       patch_cord_3( sample_player_3, 0, sample_mixer, 2 );
AudioConnection       patch_cord_4( sample_player_4, 0, sample_mixer, 3 );
AudioConnection       patch_cord_5( sample_mixer, 0, audio_output, 0 );


AudioControlSGTL5000  sgtl5000_1;

POLYPHONIC_SAMPLE_PLAYER<NUM_VOICES>  polyphonic_sample_player( reinterpret_cast<const uint16_t*>(&(AudioSamplePiano_c3_44k[0])) );

/*
template <typename T>
T lerp( const T& v1, const T& v2, float t )
{
  return v1 + ( (v2 - v1) * t );
}
*/

void setup()
{
  Serial.begin(9600);

  AudioMemory(64);
  
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5f);

  //sgtl5000_1.lineInLevel( 10 );  // 0.56volts p-p
  sgtl5000_1.lineOutLevel( 8 );  // 3.16volts p-p

  polyphonic_sample_player.add_sample_player( sample_player_1 );
  polyphonic_sample_player.add_sample_player( sample_player_2 );
  polyphonic_sample_player.add_sample_player( sample_player_3 );
  polyphonic_sample_player.add_sample_player( sample_player_4 );

  delay(100);
}

void loop()
{ 
  const float unary = random(100) / 100.0f;
  //const float speed = lerp( 0.5, 2.0f, unary );
  const float speed = 0.5f + ( 1.5f * unary );
  polyphonic_sample_player.play( speed );

  Serial.println("dong");

  const int processor_usage = AudioProcessorUsage();
  if( processor_usage > 30 )
  {
    Serial.print( "Performance spike: " );
    Serial.print( processor_usage );
    Serial.print( "\n" );
  }

  delay( random(1000) );
}
