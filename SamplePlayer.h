#include <Audio.h>

/////////////////////////////////////////////////////////

class SAMPLE_PLAYER_EFFECT : public AudioStream
{
  audio_block_t*        m_input_queue_array[1];

  const uint16_t*       m_sample_data;
  int                   m_sample_length;

  float                 m_speed;
  float                 m_read_head;

  int16_t               read_sample_linear() const;
  int16_t               read_sample_cubic() const;

  public:

  SAMPLE_PLAYER_EFFECT();
  virtual void          update() override;

  void                  play( const uint16_t* sample_data, int sample_length, float speed );
  void                  stop();

  inline bool           playing() const                 { return m_sample_data != nullptr; }
};

/////////////////////////////////////////////////////////

template< int MAX_NUM_VOICES >
class POLYPHONIC_SAMPLE_PLAYER
{
  SAMPLE_PLAYER_EFFECT* m_sample_players[ MAX_NUM_VOICES ];

  int                   m_num_voices;
  int                   m_next_voice;

  const uint16_t*       m_sample_data;
  int                   m_sample_length;

 public:

  POLYPHONIC_SAMPLE_PLAYER(const uint16_t* sample_data) :
    m_num_voices(0),
    m_next_voice(0),
    m_sample_data( sample_data + 2 ),
    m_sample_length(0)
  {
    const uint32_t header = (reinterpret_cast<const uint32_t*>(sample_data))[0];
    m_sample_length       = header & 0xFFFFFF;

    const int rate_code   = header >> 24;
    
    Serial.print( "sample length:");
    Serial.print(m_sample_length);
    Serial.print( " rate code:0x");
    Serial.println(rate_code, HEX);
  }

  void                  add_sample_player( SAMPLE_PLAYER_EFFECT& sample_player )
  {
    if( m_num_voices == MAX_NUM_VOICES )
    {
      Serial.println("Too many voices");
      return;
    }
    m_sample_players[ m_num_voices++ ] = &sample_player;
  }

  void                  play( float speed )
  {
    SAMPLE_PLAYER_EFFECT& sample_player = *m_sample_players[ m_next_voice ];
    sample_player.stop();
    sample_player.play( m_sample_data, m_sample_length, speed );

    if( ++m_next_voice == m_num_voices )
    {
      m_next_voice = 0;
    }
  }
  
};

/////////////////////////////////////////////////////////
