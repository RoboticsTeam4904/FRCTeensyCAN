#ifndef __AbstractTeensyCAN_H__
#define __AbstractTeensyCAN_H__

class AbstractTeensyCAN {
public:
	AbstractTeensyCAN(uint32_t id);

	uint32_t getId();

	virtual int call(byte* msg, byte* resp) = 0;
protected:
	uint32_t canID;
};

#endif // __AbstractTeensyCAN_H__
