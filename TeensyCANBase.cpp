#include "TeensyCANBase.h"
#include "../FlexCAN/FlexCAN.h"

/**
   Pointer to the FlexCAN object
   Only one of these should exist
 */
FlexCAN * CANbus = NULL;
/**
   The first node in the linked list of AbstractTeensyCAN 
 */
LinkedListNode<AbstractTeensyCAN> * firstNode;

/**
   The linked list consists entirely of the classes, so
   this class simply calls a function when a message is
   recieved. It should not be exposed generally, it will
   be more difficult to extend it than to extend
   AbstractTeensyCAN.
 */
class TeensyCANFunction : public AbstractTeensyCAN{
public:
	/**
	   Constructor
	   @param id the message ID that this class will respond to
	   @param callback the function that this class will call
	 */
	TeensyCANFunction(uint32_t id, int (*callback)(byte* msg, byte* resp));

	/**
	   Call the function with the CAN message and response
	   @param msg the 8 byte CAN message
	   @param resp the 8 byte CAN response
	   @return the return of the callback function
	   0 means send, 1 means do not send
	 */
	int call(byte* msg, byte* resp);
protected:
	*/**
	    The callback function for this instance
	  */
	int (*callback)(byte* msg, byte* resp);

};

/**
   Function that initializes TeensyCANBase
   This starts the FlexCAN instance at 1 megabit
 */
void CAN_begin(){
	CANbus = new FlexCAN(1000000);
	CANbus->begin();
}

/**
   Function to look for new CAN messages and call
   the appropriate class
 */
void CAN_update(){
	while(CANbus->available()){
		LinkedListNode<AbstractTeensyCAN> * node = firstNode;

		CAN_message_t rxmsg;
		
		if (CANbus->read(rxmsg)) {
			while(node != NULL){
				if(rxmsg.id == node->data->getId()){
					byte * msg = (uint8_t *) malloc(8);
					memcpy(msg, rxmsg.buf, 8);
					byte * resp = (uint8_t *) malloc(8);
					if(node->data->call(msg, resp) == 0){
						CAN_message_t txmsg;

						txmsg.id = node->data->getId();
						txmsg.len = 8;

						memcpy(txmsg.buf, resp, 8);
						CANbus->write(txmsg);
					}
					free(msg); // Cleanup, cleanup
					free(resp);
					break;
				}
				node = node->next;
			}
		}
	}
}

/**
   Function that cleans up FlexCAN
   and deletes the LinkedList
 */
void CAN_end(){
	LinkedListNode<AbstractTeensyCAN> * node = firstNode;

	while(node != NULL){
		LinkedListNode<AbstractTeensyCAN> * next = node->next;
		delete node->data;
		delete node;
		node = next;
	}

	firstNode = NULL;
	
	CANbus->end();
	delete CANbus;
}

/**
   Function that adds a new TeensyCANFunction class
   to the linked list given an id and a callback
   For usage, see documentation in the header file
 */
void CAN_add_id(uint32_t id, int (*callback)(byte* msg, byte* resp)){
	TeensyCANFunction * teensyCANFunction = new TeensyCANFunction(id, callback); // Cleanup occurs in remove

	if(firstNode == NULL){
		firstNode = new LinkedListNode<AbstractTeensyCAN>;
		firstNode->data = teensyCANFunction;
		firstNode->next = NULL;
	}
	else{
		LinkedListNode<AbstractTeensyCAN> * node = firstNode;
		while(node->next != NULL){
			node = node->next;
		}
		next->next = new LinkedListNode<AbstractTeensyCAN>;
		next->next->data = teensyCANFunction;
		next->next->next = NULL;
	}
}

/**
   Delete a class from the linked list
   with a given CAN ID
 */
void CAN_remove_id(uint32_t id){
	if(firstNode == NULL){
		return;
	}
	if(firstNode->data->getId() == id){
		delete firstNode->data;
		LinkedListNode<AbstractTeensyCAN> * next = firstNode->next;
		delete firstNode;
		firstNode = next;
	}
	else{
		LinkedListNode<AbstractTeensyCAN> * node = firstNode;
		while(node != NULL){
			LinkedListNode<AbstractTeensyCAN> * next = node->next;
			if(next->data->getId() == id){
				node->next = next->next;
				delete next->data;
				delete next;
				break;
			}
			node = next;
		}
	}
}

/**
   Constructor for a TeensyCANFunction
   @param id the message ID that this class will respond to
   @param callback the function that this class will call
 */
TeensyCANFunction::TeensyCANFunction(uint32_t id, int (*callback)(byte* msg, byte* resp))
	: AbstractTeensyCAN(id), callback(callback){}

/**
   Call the callback for a TeensyCANFunction
   @param msg the 8 byte CAN message
   @param resp the 8 byte CAN response
   @return the return of the callback function
   0 means send, 1 means do not send
*/
int TeensyCANFunction::call(byte * msg, byte * resp){
	return callback(msg, resp);
}
