#ifndef __TeensyCANBase_H__
#define __TeensyCANBase_H__

#include <Arduino.h>
#include "../FlexCAN/FlexCAN.h"

class TeensyCANBase {
private:
	static TeensyCANBase * firstTeensyCANBase;
	static FlexCAN * CANbus;
public:

	TeensyCANBase(uint32_t id, int (*callback)(byte* msg, byte* resp));
	static void begin();
	static void end();
	static void update();

	TeensyCANBase * getNext();
	uint32_t getId();

	int call(byte* msg, byte* resp);
protected:
	uint32_t canID;
	int (*callback)(byte* msg, byte* resp);

	TeensyCANBase * nextTeensyCANBase;
};

#endif // __TeensyCANBase_H__
