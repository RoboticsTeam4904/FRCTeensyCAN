
#include <FlexCAN.cpp>
#include <TeensyCANBase.h>

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

	if (CANbus.read(&rxmsg)) {
		if (rxmsg.id == )
		memcpy(msg, rxmsg.);
		return 0;
	}
	return 1;
}
int TeensyCANBase::write(const CAN_message_t &msg) {
	CANBus.write(&msg);
}