#ifndef __TeensyCANBase_H__
#define __TeensyCANBase_H__

#include <Arduino.h>
#include "AbstractTeensyCAN.h"

/**
   Function that initializes TeensyCANBase
   This should be called in setup()
*/
void CAN_begin();
/**
   Function to look for new CAN messages and call
   the appropriate class
   This should be called at some point in loop()
*/
void CAN_update();
/**
   Function that cleans up FlexCAN
   This is probably not too useful
*/
void CAN_end();
/**
   Function that adds another CAN ID and callback
   @param id the message ID that this instance responds to (0x600-0x6FF)
   @param callback the function that this instance will call
   when it recieves a message
   The parameter msg is the 8 bytes that the message contained
   The parameter resp is the 8 bytes that the function returns
   The function returns an integer status
   0 means that resp is non-empty
   1 means that resp is empty and should not be sent
*/
void CAN_add_id(uint32_t id, void (*callback)(byte* msg));
/**
   Function that adds an instance of a AbstractTeensyCAN class
   @param TeensyCAN the class to connect to CAN
   When the AbstractTeensyCAN's ID is detected in a message, the
   call function will be called
*/
void CAN_add(AbstractTeensyCAN * TeensyCAN);

void CAN_write(uint32_t id, byte * msg);

/**
   Removes a CAN ID
*/
void CAN_remove_id(uint32_t id);

#endif // __TeensyCANBase_H__
