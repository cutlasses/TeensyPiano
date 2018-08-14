#include "SamplePlayer.h"
#include "AudioSamplePiano_c3_44k.h"

//#define AUDIO_BOARD

constexpr int         NUM_VOICES(4);

constexpr int         NOTE_CV_PIN(A8);    // ROOT - on panel
constexpr int         TRIG_CV_PIN(9);     // TRIG - on panel
constexpr int         ADC_BITS(13);
constexpr int         ADC_MAX_VAL(8192);

constexpr int         SEMI_TONE_RANGE( 3.3f * 12 );

SAMPLE_PLAYER_EFFECT  sample_player_1;
SAMPLE_PLAYER_EFFECT  sample_player_2;
SAMPLE_PLAYER_EFFECT  sample_player_3;
SAMPLE_PLAYER_EFFECT  sample_player_4;
AudioMixer4           sample_mixer;

#ifdef AUDIO_BOARD
AudioOutputI2S        audio_output;
AudioControlSGTL5000  sgtl5000_1;
#else // !AUDIO_BOARD
AudioOutputAnalog     audio_output;
#endif // !AUDIO_BOARD

AudioConnection       patch_cord_1( sample_player_1, 0, sample_mixer, 0 );
AudioConnection       patch_cord_2( sample_player_2, 0, sample_mixer, 1 );
AudioConnection       patch_cord_3( sample_player_3, 0, sample_mixer, 2 );
AudioConnection       patch_cord_4( sample_player_4, 0, sample_mixer, 3 );
AudioConnection       patch_cord_5( sample_mixer, 0, audio_output, 0 );


POLYPHONIC_SAMPLE_PLAYER<NUM_VOICES>  polyphonic_sample_player( reinterpret_cast<const uint16_t*>(&(AudioSamplePiano_c3_44k[0])) );

volatile boolean g_triggered = false;

void notify_trigger()
{
  g_triggered = true;
}

void setup()
{
  Serial.begin(9600);

  AudioMemory(64);

  analogReference(DEFAULT);
  analogReadRes(ADC_BITS);
  pinMode( NOTE_CV_PIN, INPUT );
  pinMode( TRIG_CV_PIN, INPUT );

  // Add an interrupt on the RESET_CV pin to catch rising edges
  attachInterrupt( digitalPinToInterrupt(TRIG_CV_PIN), notify_trigger, RISING );

#ifdef AUDIO_BOARD
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5f);

  sgtl5000_1.lineOutLevel( 8 );  // 3.16volts p-p
#endif // AUDIO_BOARD
  
  polyphonic_sample_player.add_sample_player( sample_player_1 );
  polyphonic_sample_player.add_sample_player( sample_player_2 );
  polyphonic_sample_player.add_sample_player( sample_player_3 );
  polyphonic_sample_player.add_sample_player( sample_player_4 );

  Serial.println("Setup");

  delay(100);
}

void loop()
{
  if( g_triggered )
  {
    Serial.println("Trigger");
    g_triggered = false;

    constexpr float SEMI_TONE_COEFFICIENT( SEMI_TONE_RANGE / static_cast<float>(ADC_MAX_VAL) ); // 0v -> 0 semitone, 1V - 12 semitone

    const int note_CV       = analogRead( NOTE_CV_PIN );

    const int semitone      = round( note_CV * SEMI_TONE_COEFFICIENT );

    Serial.print("note:");
    Serial.print(note_CV);
    Serial.print(" semitone:");
    Serial.println(semitone);

    polyphonic_sample_player.play_at_pitch( semitone );
  }

  /*
  const float unary = random(100) / 100.0f;
  //const float speed = lerp( 0.5, 2.0f, unary );
  const float speed = 0.5f + ( 1.5f * unary );
  polyphonic_sample_player.play( speed );

  const int processor_usage = AudioProcessorUsage();
  if( processor_usage > 50 )
  {
    Serial.print( "Performance spike: " );
    Serial.print( processor_usage );
    Serial.print( "\n" );
  }

  delay( random(1000) );
  */
}
