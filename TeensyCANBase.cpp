#include "TeensyCANBase.h"
#include "../FlexCAN/FlexCAN.h"

/**
   TeensyCANBase
   This is the core class for sending/receiving messages to the RoboRIO
   via CAN from a Teensy.
   As there does not appear to be a queue of messages available from
   FlexCAN (reading a message deletes it), the receive function is a
   static function that checks each instance of TeensyCANBase for if
   it responds to messages from each new message's ID.
   The class is also a linked list in order to more easily iterate over
   them to find the correct instance of TeensyCANBase. This simplifies
   adding new TeensyCANBase instances a little. It might be slightly
   better to use something like STL's vector class.
 */
class TeensyCANBase {
private:
	/**
	   The first TeensyCANBase in the linked list
	   Used to begin the iterating
	*/
	static TeensyCANBase * firstTeensyCANBase;
	/**
	   The single instance of FlexCAN for all
	   instances of TeensyCANBase
	*/
	static FlexCAN * CANbus;
public:
	/**
	   Constructor for a TeensyCANBase
	   @param id the message ID that this instance responds to
	   @param callback the function that this instance will call
	   when it recieves a message
	   The parameter msg is the 8 bytes that the message contained
	   The parameter resp is the 8 bytes that the function returns
	   The function returns an integer status
	   	0 means that resp is non-empty
		1 means that resp is empty and should not be sent
	*/
	TeensyCANBase(uint32_t id, int (*callback)(byte* msg, byte* resp));
	/**
	   Static function that initializes FlexCAN
	   This should be called in setup()
	 */
	static void begin();
	/**
	   Static function that cleans up FlexCAN
	   This is probably not too useful
	*/
	static void end();
	/**
	   Static function to look for new CAN messages and call
	   the appropriate callback
	   This should be called at some point in loop()
	*/
	static void update();
	/**
	   Static function to remove a CAN id from the list
	*/
	static void remove_id(uint32_t id);

	/**
	   The next TeensyCANBase in the linked list
	 */
	TeensyCANBase * nextTeensyCANBase;

	/**
	   Function that returns the ID that this instance responds to
	   \return the ID that this instance responds to
	*/
	uint32_t getId();

	/**
	   Call the callback of this instance
	   @param msg the 8 bytes of input
	   @param resp the 8 bytes of output
	 */
	int call(byte* msg, byte* resp);
protected:
	/**
	   The message ID that this TeensyCANBase instance responds to
	*/
	uint32_t canID;
	/**
	   The callback that this TeensyCANBase instance calls when it
	   recieves a message
	   The parameter msg is the 8 bytes that the message contained
	   The parameter resp is the 8 bytes that the function returns
	   The function returns an integer status
	   	0 means that resp is non-empty
		1 means that resp is empty and should not be sent
	*/
	int (*callback)(byte* msg, byte* resp);

};

void CAN_begin(){
	TeensyCANBase::begin();
}

void CAN_update(){
	TeensyCANBase::update();
}

void CAN_end(){
	TeensyCANBase::end();
}

void CAN_add_id(uint32_t id, int (*callback)(byte* msg, byte* resp)){
	TeensyCANBase * newInstance = new TeensyCANBase(id, callback); // Cleanup occurs in remove
}

void CAN_remove_id(uint32_t id){
	TeensyCANBase::remove_id(id);
}

TeensyCANBase * TeensyCANBase::firstTeensyCANBase = NULL;
FlexCAN * TeensyCANBase::CANbus = NULL;

TeensyCANBase::TeensyCANBase(uint32_t id, int (*callback)(byte* msg, byte* resp)) : canID(id), callback(callback){
	if(TeensyCANBase::firstTeensyCANBase == NULL){
		TeensyCANBase::firstTeensyCANBase = this;
	}
	else{
		TeensyCANBase * next = TeensyCANBase::firstTeensyCANBase;
		while(next->nextTeensyCANBase != NULL){
			next = next->nextTeensyCANBase;
		}
		next->nextTeensyCANBase = this;
	}
	nextTeensyCANBase = NULL;
}


void TeensyCANBase::begin() {
	CANbus = new FlexCAN(1000000);
	CANbus->begin();
}

void TeensyCANBase::remove_id(uint32_t id){
	TeensyCANBase * next = TeensyCANBase::firstTeensyCANBase;
	while(next->nextTeensyCANBase->getId() != id){
		next = next->nextTeensyCANBase;
	}
	delete next;
}

void TeensyCANBase::end() {
	CANbus->end();
	delete CANbus;
}

void TeensyCANBase::update(){
	while(CANbus->available()){
		TeensyCANBase * teensyCANBase = TeensyCANBase::firstTeensyCANBase;

		CAN_message_t rxmsg;
		
		if (CANbus->read(rxmsg)) {
			while(teensyCANBase != NULL){
				if(rxmsg.id == teensyCANBase->getId()){
					byte * msg = (uint8_t *) malloc(8);
					memcpy(msg, rxmsg.buf, 8);
					byte * resp = (uint8_t *) malloc(8);
					if(teensyCANBase->call(msg, resp) == 0){
						CAN_message_t txmsg;

						txmsg.id = teensyCANBase->getId();
						txmsg.len = 8;

						memcpy(txmsg.buf, resp, 8);
						CANbus->write(txmsg);
					}
					free(msg); // Cleanup, cleanup
					free(resp);
					break;
				}
				teensyCANBase = teensyCANBase->nextTeensyCANBase;
			}
		}
	}

	delay(10);
}

int TeensyCANBase::call(byte * msg, byte * resp){
	return callback(msg, resp);
}

uint32_t TeensyCANBase::getId(){
	return canID;
}
