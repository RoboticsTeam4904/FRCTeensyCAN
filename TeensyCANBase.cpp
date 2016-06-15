#include <TeensyCANBase.h>

TeensyCANBase::TeensyCANBase(uint32_t id, int max_queue) {
	canID = id;
	max_queue_len = max_queue;
	if(CANbus == NULL){
		copies = 0;
		CANbus = new FlexCAN(1000000);
	}
	else{
		copies++;
	}
}

TeensyCANBase::~TeensyCANBase(){
	copies--;
	if(copies <= 0){
		delete CANbus;
	}
}

void TeensyCANBase::begin() {
	if(copies == 0){
		CANbus->begin();
	}
}

void TeensyCANBase::end() {
	if(copies == 0){
		CANbus->end();
	}
}
int TeensyCANBase::available() {
	return CANbus->available();
}

int TeensyCANBase::read(byte* &msg) {
	CAN_message_t rxmsg;

	for(int i = 0; i < message_queue.size(); i++){
		if(message_queue[i].id == canID){
			memcpy(msg, message_queue[i].buf, 8);
			message_queue.erase(i);
			return 0;
		}
	}

	while(available()){
		if (CANbus->read(rxmsg)) {
			if (rxmsg.id == canID){
				memcpy(msg, rxmsg.buf, 8);
				return 0;
			}
			else{
				message_queue.push_back(rxmsg);
				if(message_queue.size() > max_queue_len){
					// TODO: make less inefficient
					message_queue.erase(0);
				}
			}
		}
	}
	
	return 1;
}
int TeensyCANBase::write(byte* &msg) {
	CAN_message_t txmsg;

	txmsg.id = canID;
	txmsg.len = 8;

	memcpy(txmsg.buf, msg, 8);
	CANbus->write(txmsg);

	return 0;
}
