#include "SamplePlayer.h"

SAMPLE_PLAYER_EFFECT::SAMPLE_PLAYER_EFFECT() :
  AudioStream( 1, m_input_queue_array ),
  m_input_queue_array(),
  m_sample_data(nullptr),
  m_speed(1.0f),
  m_read_head(0.0f)
{
}
  
void SAMPLE_PLAYER_EFFECT::update()
{
  audio_block_t* block = allocate();

  if( block != nullptr )
  {
    if( playing() )
    {
      for( int i = 0; i < AUDIO_BLOCK_SAMPLES; ++i )
      {
        
      }
    }

    transmit( block, 0 );

    release( block );
  }
}

void SAMPLE_PLAYER_EFFECT::start( const uint16_t* sample_data, float speed )
{
  m_sample_data = sample_data;
  m_speed       = speed;
  m_read_head   = 0.0f;
}

void SAMPLE_PLAYER_EFFECT::stop()
{
  m_sample_data  = nullptr;
  m_read_head    = 0.0f;
}

