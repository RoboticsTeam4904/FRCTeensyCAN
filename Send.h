#ifndef __SEND_H__
#define __SEND_H__
#include <FlexCAN.h>



void sendMsg(byte* inMsg, uint32_t id);

void receiveMsg(uint32_t id, byte * resp);

#endif
