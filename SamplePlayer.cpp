#include "SamplePlayer.h"

SAMPLE_PLAYER_EFFECT::SAMPLE_PLAYER_EFFECT() :
  AudioStream( 1, m_input_queue_array ),
  m_input_queue_array(),
  m_sample_data(nullptr),
  m_sample_size(0),
  m_speed(1.0f),
  m_read_head(0.0f)
{
}
  
void SAMPLE_PLAYER_EFFECT::update()
{
  if( playing() )
  {
    audio_block_t* block = allocate();

    if( block != nullptr )
    {
      for( int i = 0; i < AUDIO_BLOCK_SAMPLES; ++i )
      {
        const int head_int = static_cast<int>(m_read_head);
        if( head_int < m_sample_size )
        {
          block->data[i] = m_sample_data[head_int];
          m_read_head += m_speed;
        }
        else
        {
          // reached the end of the sample
          memset( block->data + i, 0, (AUDIO_BLOCK_SAMPLES - i) * sizeof(int16_t) );
          break;
        }
      }

      transmit( block, 0 );

      release( block );
    }
  }
}

void SAMPLE_PLAYER_EFFECT::start( const uint16_t* sample_data, int sample_size, float speed )
{
  m_sample_data = sample_data + 2; // skip the 4 byte header (contains num samples and sample rate) https://github.com/PaulStoffregen/Audio/blob/master/play_memory.cpp
  m_sample_size = sample_size;
  m_speed       = speed;
  m_read_head   = 0.0f;
}

void SAMPLE_PLAYER_EFFECT::stop()
{
  m_sample_data  = nullptr;
  m_sample_size  = 0;
  m_read_head    = 0.0f;
}

