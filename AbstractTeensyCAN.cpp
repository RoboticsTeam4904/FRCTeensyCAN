#include "AbstractTeensyCAN.h"

/**
   Constructor.
   For the abstract base class, only
   the CAN ID is needed.
   @param id: the CAN ID
*/
AbstractTeensyCAN::AbstractTeensyCAN(uint32_t id){
	canID = 0; // This ensures that all memory not set is 0
	canID += id;
}

/**
   Returns the classes's ID
   @return: the current CAN ID
*/
uint32_t AbstractTeensyCAN::getId(){
	return canID;
}
