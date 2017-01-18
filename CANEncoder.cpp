#include "CANEncoder.h"

CANEncoder::CANEncoder(uint8_t Pin1, uint8_t Pin2, uint32_t id) : AbstractTeensyCAN(id), Encoder(Pin1, Pin2), lastTime(micros()), lastPos(read()) {}

int CANEncoder::call(byte * msg, byte * resp) {
  long pos = read();
  long rate = ((double) 1000000.0 * (pos - lastPos)) / ((double) (micros() - lastTime));
  lastPos = pos;
  lastTime = micros();
  if (msg[0] == 0) {
    resp[0] = pos & 0xff;
    resp[1] = (pos >> 8) & 0xff;
    resp[2] = (pos >> 16) & 0xff;
    resp[3] = (pos >> 24) & 0xff;

    resp[4] = 0; // Mode

    for (int i = 5; i < 8; i++) {
      resp[i] = 0;
    }

    return 0;
  }
  else if (msg[0] == 1) {
    resp[0] = rate & 0xff;
    resp[1] = (rate >> 8) & 0xff;
    resp[2] = (rate >> 16) & 0xff;
    resp[3] = (rate >> 24) & 0xff;

    resp[4] = 1; // Mode

    for (int i = 5; i < 8; i++) {
      resp[i] = 0;
    }

    return 0;
    }
  else if (msg[0] == 0x72 && msg[1] == 0x65 && msg[2] == 0x73 && msg[3] == 0x65 && msg[4] == 0x74 && msg[5] == 0x65 && msg[6] == 0x6e && msg[7] == 0x63) {
    write(0);
    pos = 0;
    lastPos = 0;
    lastTime = micros();
    rate = 0;
    return 1;
  }
  return 1;
}

