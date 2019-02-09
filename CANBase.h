#ifdef __CANBase_H__
#define __CANBase_H__

#include <Arduino.h>

void CAN_begin();

void writeLongs(uint32_t id, int32_t value1, int32_t value2);

void writeData(uint32_t id, int64_t data);

void writeBuffer(uint32_t id, int8_t buf0,  int8_t buf1, int8_t buf2, int8_t buf3, int8_t buf4, int8_t buf5, int8_t buf6, int8_t buf7);

void listen_begin();

void add_id(uint32_t id, void (*callback)(byte* msg));


#endif
