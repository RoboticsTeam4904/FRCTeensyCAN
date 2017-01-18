#ifndef __CANENCODER_H__
#define __CANENCODER_H__

#include <AbstractTeensyCAN.h>
#include <Encoder.h>

class CANEncoder : public AbstractTeensyCAN, public Encoder {
  public:
    CANEncoder(uint8_t Pin1, uint8_t Pin2, uint32_t id);
    int call(byte* msg, byte* resp);
  private:
    long lastPos;
    long lastTime;
};

#endif
