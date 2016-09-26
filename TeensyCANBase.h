#ifndef __TeensyCANBase_H__
#define __TeensyCANBase_H__

/**
   Function that initializes TeensyCANBase
   This should be called in setup()
*/
void CAN_begin();
/**
   Function to look for new CAN messages and call
   the appropriate callback
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
   \param id the message ID that this instance responds to
   \param callback the function that this instance will call
   when it recieves a message
   The parameter msg is the 8 bytes that the message contained
   The parameter resp is the 8 bytes that the function returns
   The function returns an integer status
   0 means that resp is non-empty
   1 means that resp is empty and should not be sent
*/
void CAN_add_id(uint32_t id, int (*callback)(byte* msg, byte* resp));

#endif // __TeensyCANBase_H__
