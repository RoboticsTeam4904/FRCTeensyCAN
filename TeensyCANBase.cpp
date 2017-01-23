#include "TeensyCANBase.h"
#include "LinkedList.h"
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
	   @param id the message ID that this class will respond to (0x600-0x6FF)
	   @param callback the function that this class will call
	 */
	TeensyCANFunction(uint32_t id, int (*callback)(byte* msg));

	/**
	   Call the function with the CAN message and response
	   @param msg the 8 byte CAN message
	   @param resp the 8 byte CAN response
	   @return the return of the callback function
	   0 means send, 1 means do not send
	 */
	int call(byte* msg);
protected:
	/**
	    The callback function for this instance
	  */
	int (*callback)(byte* msg);

};

/**
   Function that initializes TeensyCANBase
   This starts the FlexCAN instance at 1 megabit
 */
void CAN_begin(){
	CANbus = new FlexCAN(1000000);
	CAN_filter_t filter;
	filter.rtr = 0;
	filter.ext = 0;
	filter.id = 0x6FF; // Change this for different IDs
	CANbus->begin(filter);
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
					byte * msg = new byte[8];
					memcpy(msg, rxmsg.buf, 8);

					node->data->call(msg);
					delete msg; // Cleanup, cleanup
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
   Function that adds an instance of a AbstractTeensyCAN class
   @param TeensyCAN the class to connect to CAN
   When the AbstractTeensyCAN's ID is detected in a message, the
   call function will be called
*/
void CAN_add(AbstractTeensyCAN * newAbstractTeensyCAN){
	if(firstNode == NULL){
		firstNode = new LinkedListNode<AbstractTeensyCAN>;
		firstNode->data = newAbstractTeensyCAN;
		firstNode->next = NULL;
	}
	else{
		LinkedListNode<AbstractTeensyCAN> * lastFirst = firstNode;
		firstNode = new LinkedListNode<AbstractTeensyCAN>;
		firstNode->data = newAbstractTeensyCAN;
		firstNode->next = lastFirst;
	}
}

/**
   Function that adds another CAN ID and callback
   @param id the message ID that this instance responds to
   @param callback the function that this instance will call
   when it recieves a message
   The parameter msg is the 8 bytes that the message contained
   The parameter resp is the 8 bytes that the function returns
   The function returns an integer status
   0 means that resp is non-empty
   1 means that resp is empty and should not be sent
*/
void CAN_add_id(uint32_t id, int (*callback)(byte* msg)){
	TeensyCANFunction * teensyCANFunction = new TeensyCANFunction(id, callback); // Cleanup occurs in remove

	if(firstNode == NULL){
		firstNode = new LinkedListNode<AbstractTeensyCAN>;
		firstNode->data = teensyCANFunction;
		firstNode->next = NULL;
	}
	else{
		LinkedListNode<AbstractTeensyCAN> * lastFirst = firstNode;
		firstNode = new LinkedListNode<AbstractTeensyCAN>;
		firstNode->data = teensyCANFunction;
		firstNode->next = lastFirst;
	}
}

void CAN_write(uint32_t id, byte * msg){
	CAN_message_t txmsg;

	txmsg.id = id;
	txmsg.len = 8;

	memcpy(txmsg.buf, msg, 8);

	CANbus->write(txmsg);
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
TeensyCANFunction::TeensyCANFunction(uint32_t id, int (*callback)(byte* msg))
	: AbstractTeensyCAN(id), callback(callback){}

/**
   Call the callback for a TeensyCANFunction
   @param msg the 8 byte CAN message
   @param resp the 8 byte CAN response
   @return the return of the callback function
   0 means send, 1 means do not send
*/
int TeensyCANFunction::call(byte * msg){
	return callback(msg);
}
