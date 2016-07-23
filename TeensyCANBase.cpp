#include "TeensyCANBase.h"
#include "../FlexCAN/FlexCAN.h"

TeensyCANBase * TeensyCANBase::firstTeensyCANBase = NULL;
FlexCAN * TeensyCANBase::CANbus = NULL;

TeensyCANBase::TeensyCANBase(uint32_t id, int (*callback)(byte* msg, byte* resp)){
	if(TeensyCANBase::firstTeensyCANBase == NULL){
		TeensyCANBase::firstTeensyCANBase = this;
	}
	else{
		TeensyCANBase * next = TeensyCANBase::firstTeensyCANBase;
		while(next != NULL){
			next = next->getNext();
		}
		next = this;
	}
}


void TeensyCANBase::begin() {
	CANbus = new FlexCAN(1000000);
	CANbus->begin();
}

void TeensyCANBase::end() {
	CANbus->end();
	delete CANbus;
}

void TeensyCANBase::update(){
	while(CANbus->available()){
		TeensyCANBase * teensyCANBase = TeensyCANBase::firstTeensyCANBase;
		bool read = false;

		CAN_message_t rxmsg;
		
		if (CANbus->read(rxmsg)) {
			while(!read && teensyCANBase != NULL){
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
					delete msg; // Cleanup, cleanup
					delete resp;
					read = true;
				}
				teensyCANBase = teensyCANBase->getNext();
			}
		}
	}
}

int TeensyCANBase::call(byte * msg, byte * resp){
	return 0;
}

TeensyCANBase * TeensyCANBase::getNext(){
	return nextTeensyCANBase;
}

uint32_t TeensyCANBase::getId(){
	return canID;
}
