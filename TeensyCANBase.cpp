#Include <TeensyCANBase.h>

TeensyCANBase::TeensyCANBase(uint32_t id) {
	canID = id;
	CANbus = FlexCAN(1000000);
}
void TeensyCANBase::begin() {
	CANbus.begin();
}

void TeensyCANBase::end() {
	CANbus.end();
}
int TeensyCANBase::available() {
	return CANbus.available();
}

int TeensyCANBase::read(byte* &msg) {
	CAN_message_t rxmsg;

	if (CANbus.read(rxmsg)) {
		if (rxmsg.id == canID){
			memcpy(msg, rxmsg.buf, 8);
		}
		return 0;
	}
	return 1;
}
int TeensyCANBase::write(byte* &msg) {
	CAN_message_t txmsg;

	txmsg.id = canID;
	txmsg.len = 8;

	memcpy(txmsg.buf, msg, 8);
	CANbus.write(txmsg);

	return 0;
}
