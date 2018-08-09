#include "Util.h"
#include "SamplePlayer.h"

SAMPLE_PLAYER_EFFECT::SAMPLE_PLAYER_EFFECT() :
  AudioStream( 1, m_input_queue_array ),
  m_input_queue_array(),
  m_sample_data(nullptr),
  m_sample_length(0),
  m_speed(1.0f),
  m_read_head(0.0f)
{
}

int16_t SAMPLE_PLAYER_EFFECT::read_sample_linear() const
{
  // linearly interpolate between the current sample and its neighbour
  // (previous neighbour if frac is less than 0.5, otherwise next)
  const int int_part      = trunc_to_int( m_read_head );
  const float frac_part   = m_read_head - int_part;
  
  const int16_t curr_samp   = m_sample_data[ int_part ];
  
  if( frac_part < 0.5f )
  {
    int prev        = int_part - 1;
    if( prev < 0 )
    {
      // at the beginning of the buffer, assume next sample was the same and use that (e.g. no interpolation)
      return curr_samp;
    }
    
    const float t     = frac_part / 0.5f;
    
    const int16_t prev_samp = m_sample_data[ prev ];
    
    return lerp( prev_samp, curr_samp, t );
  }
  else
  {
    int next        = int_part + 1;
    if( next >= m_sample_length )
    {
      // at the end of the buffer, assume next sample was the same and use that (e.g. no interpolation)
      return curr_samp;
    }
    
    const float t     = ( frac_part - 0.5f ) / 0.5f;
    
    const int16_t next_samp = m_sample_data[ next ];
    
    return lerp( curr_samp, next_samp, t );
  }
}

int16_t SAMPLE_PLAYER_EFFECT::read_sample_cubic() const
{
  const int int_part      = trunc_to_int( m_read_head );
  const float frac_part   = m_read_head - int_part;
  
  float p0;
  if( int_part >= 2 )
  {
    p0            = m_sample_data[ int_part - 2 ];
  }
  else
  {
    // at the beginning of the buffer, assume previous sample was the same
    p0            = m_sample_data[ 0 ];
  }
  
  float p1;
  if( int_part <= 2 )
  {
    // reuse p0
    p1            = p0;
  }
  else
  {
    p1            = m_sample_data[ int_part - 1 ];
  }
  
  float p2        = m_sample_data[ int_part ];
  
  float p3;
  if( int_part < m_sample_length - 1)
  {
    p3            = m_sample_data[ int_part + 1 ];
  }
  else
  {
    p3            = p2;
  }
  
  const float t   = lerp( 0.33333f, 0.66666f, frac_part );
  
  float sampf     = cubic_interpolation( p0, p1, p2, p3, t );
  return round_to_int<int16_t>( sampf );
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
        if( head_int < m_sample_length )
        {
          //block->data[i] = m_sample_data[head_int];
          //block->data[i] = read_sample_cubic();
          block->data[i] = read_sample_linear();
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

void SAMPLE_PLAYER_EFFECT::play( const uint16_t* sample_data, int sample_length, float speed )
{
  m_sample_data   = sample_data;
  m_sample_length = sample_length;
  m_speed         = speed;
  m_read_head     = 0.0f;
}

void SAMPLE_PLAYER_EFFECT::stop()
{
  m_sample_data   = nullptr;
  m_sample_length = 0;
  m_read_head     = 0.0f;
}

