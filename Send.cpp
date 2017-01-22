#include "Send.h"

void sendMsg(byte* inMsg, uint32_t id) {
  CAN_message_t msg;
  msg.id = id;
  msg.len = 8;
  memcpy(msg.buf, inMsg, 8);
  Can0.write(msg);  
}
//Call this in the void loop
void receiveMsg(uint32_t id, byte * resp) {
  CAN_message_t resp1;
  byte * outMsg = (uint8_t *) malloc(8);
  while (Can0.read(resp1)) {
    if (resp1.id == id) {
      memcpy(outMsg, resp1.buf, 8);
    }else{
      bzero(outMsg, 8);
    }
  }
  resp = outMsg;
}
