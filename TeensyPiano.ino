#include "SamplePlayer.h"
#include "AudioSamplePiano_c3_44k.h"

AudioInputI2S         audio_input;
SAMPLE_PLAYER_EFFECT  sample_player;
AudioOutputI2S        audio_output;

AudioConnection       patch_cord_1( audio_input, 0, sample_player, 0 );
AudioConnection       patch_cord_2( sample_player, 0, audio_output, 0 );

AudioControlSGTL5000  sgtl5000_1;

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

  delay(100);
}

void loop()
{
  const uint16_t* piano_sample = reinterpret_cast<const uint16_t*>(&(AudioSamplePiano_c3_44k[0]));

  const float unary = random(100) / 100.0f;
  //const float speed = lerp( 0.5, 2.0f, unary );
  const float speed = 0.5f + ( 1.5f * unary );
  sample_player.start( piano_sample, 58945 * 2, speed );

  Serial.println("dong");

  const int processor_usage = AudioProcessorUsage();
  if( processor_usage > 30 )
  {
    Serial.print( "Performance spike: " );
    Serial.print( processor_usage );
    Serial.print( "\n" );
  }

  delay( 4000 );
}
