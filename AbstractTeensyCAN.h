#ifndef __AbstractTeensyCAN_H__
#define __AbstractTeensyCAN_H__

#include <Arduino.h>

/**
   Base class for future classes that can
   be associated with a CAN ID in order
   to provide access to a sensor or other
   Teensy connected item.
*/
class AbstractTeensyCAN {
public:
	/**
	   Constructor.
	   For the abstract base class, only
	   the CAN ID is needed.
	   @param id: the CAN ID
	*/
	AbstractTeensyCAN(uint32_t id);

	/**
	   Returns the classes's ID
	   @return: the current CAN ID
	*/
	uint32_t getId();

	/**
	   Function to be called when
	   CAN message arrives.
	   @param msg: the CAN message (8 bytes long)
	   @param resp: the CAN response (8 bytes long)
	   @return: 0 to send response, 1 to hold
	 */
	virtual int call(byte* msg) = 0;
protected:
	/**
	   The ID of CAN messages to read/write
	 */
	uint32_t canID;
};

#endif // __AbstractTeensyCAN_H__
