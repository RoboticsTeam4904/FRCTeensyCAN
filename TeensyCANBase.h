#ifndef __TeensyCANBase_H__
#define __TeensyCANBase_H__

#include <Arduino.h>
#include <FlexCAN.h>

class TeensyCANBase {
private:
	FlexCAN CANbus;
public:
	uint32_t canID;
	TeensyCANBase(uint32_t id = 0x222);
	void begin();
	void end();
	int available();
	int read(byte* &msg);
	int write(byte* &msg);

};

#endif // __TeensyCANBase_H__
