#include <Audio.h>

class SAMPLE_PLAYER_EFFECT : public AudioStream
{
  audio_block_t*        m_input_queue_array[1];

  const uint16_t*       m_sample_data;
  int                   m_sample_size;

  float                 m_speed;
  float                 m_read_head;

  int16_t               read_sample_linear() const;
  int16_t               read_sample_cubic() const;

  public:

  SAMPLE_PLAYER_EFFECT();
  virtual void          update() override;

  void                  start( const uint16_t* sample_data, int sample_size, float speed );
  void                  stop();

  inline bool           playing() const                 { return m_sample_data != nullptr; }
};

