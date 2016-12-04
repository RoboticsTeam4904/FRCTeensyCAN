#include "AbstractTeensyCAN.h"

AbstractTeensyCAN::AbstractTeensyCAN(uint32_t id) : canID(id){}

uint32_t AbstractTeensyCAN::getId(){
	return canID;
}
