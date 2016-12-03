#include "TeensyCANBase.h"
#include "../FlexCAN/FlexCAN.h"

FlexCAN * CANbus = NULL;
LinkedListNode<AbstractTeensyCAN> * firstNode;

class TeensyCANFunction : public AbstractTeensyCAN{
public:
	TeensyCANFunction(uint32_t id, int (*callback)(byte* msg, byte* resp));

	int call(byte* msg, byte* resp);
protected:
	int (*callback)(byte* msg, byte* resp);

};

void CAN_begin(){
	CANbus = new FlexCAN(1000000);
	CANbus->begin();
}

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

	delay(10);
}

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

TeensyCANFunction::TeensyCANFunction(uint32_t id, int (*callback)(byte* msg, byte* resp))
	: canID(id), callback(callback){}

int TeensyCANFunction::call(byte * msg, byte * resp){
	return callback(msg, resp);
}

AbstractTeensyCAN::AbstractTeensyCAN(uint32_t id) : canID(id){}

uint32_t AbstractTeensyCAN::getId(){
	return canID;
}
