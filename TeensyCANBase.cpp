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
	/**
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
					
					CAN_message_t txmsg;
					txmsg.id = node->data->getId();
					txmsg.len = 8;

					if(node->data->call(msg, resp) == 0){
						memcpy(txmsg.buf, resp, 8);
					}
					else{
						bzero(txmsg.buf, 8);
					}
					CANbus->write(txmsg);
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
   Function that adds an instance of a AbstractTeensyCAN class
   @param TeensyCAN the class to connect to CAN
   When the AbstractTeensyCAN's ID is detected in a message, the
   call function will be called
*/
void CAN_add_id(AbstractTeensyCAN * newAbstractTeensyCAN){
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
void CAN_add_id(uint32_t id, int (*callback)(byte* msg, byte* resp)){
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
