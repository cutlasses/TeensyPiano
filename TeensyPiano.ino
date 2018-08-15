#include "SamplePlayer.h"
#include "AudioSamplePiano_c3_44k.h"

#define AUDIO_BOARD
#define SHOW_PERF

constexpr int         NUM_VOICES(8);

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

#ifdef AUDIO_BOARD
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5f);

  sgtl5000_1.lineOutLevel( 8 );  // 3.16volts p-p
#else // !AUDIO_BOARD
  // RADIO MUSIC setup
  analogReference(DEFAULT);
  analogReadRes(ADC_BITS);
  pinMode( NOTE_CV_PIN, INPUT );
  pinMode( TRIG_CV_PIN, INPUT );

  // Add an interrupt on the RESET_CV pin to catch rising edges
  attachInterrupt( digitalPinToInterrupt(TRIG_CV_PIN), notify_trigger, RISING );
#endif // !AUDIO_BOARD
  
  polyphonic_sample_player.add_sample_player( sample_player_1 );
  polyphonic_sample_player.add_sample_player( sample_player_2 );
  polyphonic_sample_player.add_sample_player( sample_player_3 );
  polyphonic_sample_player.add_sample_player( sample_player_4 );

  Serial.println("Setup");

  delay(100);
}

void loop()
{
#ifndef AUDIO_BOARD
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
#endif // !AUDIO_BOARD

#ifdef SHOW_PERF
  int perf_time = millis();
  static int time_stamp = 0;
  static int total_processor_usage = 0;
  static int num_samples = 0;
  if( perf_time > time_stamp )
  {
    time_stamp = perf_time + 1000.0f;
    
    Serial.print( "Average processor usage:" );
    Serial.println( static_cast<float>(total_processor_usage) / num_samples );

    total_processor_usage = 0;
    num_samples = 0;
  }
  else
  {
    total_processor_usage += AudioProcessorUsage();
    ++num_samples;
  }
#endif // SHOW_PERF

#ifdef AUDIO_BOARD
  int time = millis();
  static int next_play_time = 0;
  if( time > next_play_time )
  {
    const float unary = random(100) / 100.0f;
    const float speed = 0.5f + ( 1.5f * unary );
    polyphonic_sample_player.play( speed );

    Serial.print("Play at ");
    Serial.println( speed );
    next_play_time = time + random( 1000 );
  }
  #endif // AUDIO_BOARD
}
