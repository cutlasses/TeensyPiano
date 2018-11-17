#include "Interface.h"
#include "SamplePlayer.h"
#include "AudioSamplePiano_c3_44k.h"

//#define AUDIO_BOARD
//#define SHOW_PERF
//#define TEST_BOARD

//#define QUANTISE_INPUT

constexpr int         NUM_VOICES(4);

constexpr int         NOTE_CV_PIN(A8);    // ROOT - on panel
constexpr int         TRIG_CV_PIN(9);     // TRIG - on panel
constexpr int         TRIG_BUTTON_PIN(8);
constexpr int         RESET_LED_PIN(11);
constexpr int         CHORD_POT_PIN(A9);
constexpr int         ROOT_POT_PIN(A7);
constexpr int         ADC_BITS(13);
constexpr int         ADC_MAX_VAL(8192);

constexpr int         TRIG_FLASH_TIME_MS(100);

constexpr float       SAMPLE_MIX( 1.0f / NUM_VOICES );

LED                   trig_led(RESET_LED_PIN, false);
BUTTON                trig_button(TRIG_BUTTON_PIN, false);
DIAL                  root_dial(ROOT_POT_PIN);
DIAL                  chord_dial(CHORD_POT_PIN);

SAMPLE_PLAYER_EFFECT      sample_player_1;
SAMPLE_PLAYER_EFFECT      sample_player_2;
SAMPLE_PLAYER_EFFECT      sample_player_3;
SAMPLE_PLAYER_EFFECT      sample_player_4;
AudioMixer4               sample_mixer;
AudioEffectFreeverb       freeverb;
AudioMixer4               reverb_mixer;
//AudioFilterStateVariable  filter;

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
//AudioConnection       patch_cord_7( sample_mixer, 0, audio_output, 0 );
AudioConnection       patch_cord_5( sample_mixer, 0, reverb_mixer, 0 );
AudioConnection       patch_cord_6( sample_mixer, 0, freeverb, 0 );
AudioConnection       patch_cord_7( freeverb, 0, reverb_mixer, 1 );
AudioConnection       patch_cord_8( reverb_mixer, 0, audio_output, 0 );

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
  sgtl5000_1.volume(0.85f);

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

  for( int i = 0; i < 4; ++i )
  {
    sample_mixer.gain( i, SAMPLE_MIX );
  }

  //Serial.println("Setup");

  freeverb.roomsize( 0.95f );
  freeverb.damping( 0.5f );

  reverb_mixer.gain( 0, 1.0f );
  reverb_mixer.gain( 1, 0.0f );

  trig_led.setup();
  trig_button.setup();

  delay(100);
}

void loop()
{
  const int time = millis();
  
  trig_led.update( time );
  trig_button.update( time );
  
  if( root_dial.update() )
  {
    const float v = root_dial.value( static_cast<float>(ADC_MAX_VAL) );
    //freeverb.roomsize( v );
    reverb_mixer.gain( 1, v );

    //Serial.print("Root:");
    //Serial.println(v);
  }
  if( chord_dial.update() )
  {
    const float v = chord_dial.value( static_cast<float>(ADC_MAX_VAL) );
    freeverb.damping( v );

    //Serial.print("Chord:");
    //Serial.println(v);
  }

  if( trig_button.single_click() )
  {
    g_triggered = true;
  }
  
#ifndef AUDIO_BOARD
  if( g_triggered )
  {
    g_triggered = false;

    constexpr float SEMI_TONE_COEFFICIENT( SEMI_TONE_RANGE / static_cast<float>(ADC_MAX_VAL) ); // 0v -> 0 semitone, 1V - 12 semitone

    const int note_CV       = analogRead( NOTE_CV_PIN );

    const int semitone      = round( note_CV * SEMI_TONE_COEFFICIENT );

    trig_led.flash_on( time, TRIG_FLASH_TIME_MS );

    /*
    Serial.print("note:");
    Serial.print(note_CV);
    Serial.print(" semitone:");
    Serial.println(semitone);
    */

#ifdef QUANTISE_INPUT
    polyphonic_sample_player.play_at_quantised_pitch( semitone );
#else
    polyphonic_sample_player.play_at_pitch( semitone );
#endif
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

#ifdef TEST_BOARD
  static int next_play_time = 0;
  if( time > next_play_time )
  {
    static int semitone = 0;
    polyphonic_sample_player.play_at_quantised_pitch( semitone );

    semitone = ( semitone + 1 ) % SEMI_TONE_RANGE;
    next_play_time = time + 2000;

    trig_led.flash_on( time, TRIG_FLASH_TIME_MS );
    /*
    const int semitone = random_ranged( 0, SEMI_TONE_RANGE );
    polyphonic_sample_player.play_at_quantised_pitch( semitone );

    next_play_time = time + random( 3000 );
    */
  }
  #endif // TEST_BOARD
}
