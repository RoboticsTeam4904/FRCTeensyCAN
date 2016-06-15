#ifndef __TeensyCANBase_H__
#define __TeensyCANBase_H__

#include <Arduino.h>
#include <FlexCAN.h>
#include <vector>

class TeensyCANBase {
private:
	static FlexCAN * CANbus;
	static int copies;
	// There does not seem to be a message queue within FlexCAN, so we need one
	static vector<CAN_message_t> message_queue;
	static int max_queue_len;
public:
	uint32_t canID;
	TeensyCANBase(uint32_t id = 0x222, int max_queue = 256);
	void begin();
	void end();
	int available();
	int read(byte* &msg);
	int write(byte* &msg);

};

#endif // __TeensyCANBase_H__
