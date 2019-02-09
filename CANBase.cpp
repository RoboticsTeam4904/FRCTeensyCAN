#include <FlexCAN.h>
#include "LinkedList.h"

static CAN_message_t txmsg;

void CAN_begin(){
  Can0.begin(1000000)

  msg.ext = 0;
  msg.id = 0;
  msg.len = 8;
}
// All the infrastructure needed to send data.
void writeLongs(uint32_t id, int32_t value1, int32_t value2){
  msg.id = id;

  msg.buf[0] = (value1 >> 24) & 0xff;
  msg.buf[1] = (value1 >> 16) & 0xff;
  msg.buf[2] = (value1 >> 8) & 0xff;
  msg.buf[3] = (value1 & 0xff);

  msg.buf[4] = (value2 >> 24) & 0xff;
  msg.buf[5] = (value2 >> 16) & 0xff;
  msg.buf[6] = (value2 >> 8) & 0xff;
  msg.buf[7] = (value2 & 0xff);
  
  Can0.write(msg);
}

void writeData(uint32_t id, int64_t data){
  msg.id = id;

  msg.buf[0] = (data >> 56) & 0xff;
  msg.buf[1] = (data >> 48) & 0xff;
  msg.buf[2] = (data >> 40) & 0xff;
  msg.buf[3] = (data >> 32) & 0xff;
  msg.buf[4] = (data >> 24) & 0xff;
  msg.buf[5] = (data >> 16) & 0xff;
  msg.buf[6] = (data >> 8) & 0xff;
  msg.buf[7] = (data & 0xff);

  Can0.write(msg);
}

void writeBuffer(uint32_t id, int8_t buf0,  int8_t buf1, int8_t buf2, int8_t buf3, int8_t buf4, int8_t buf5, int8_t buf6, int8_t buf7){
  msg.id = id;

  msg.buf[0] = buf0 & 0xff;
  msg.buf[1] = buf1 & 0xff;
  msg.buf[2] = buf2 & 0xff;
  msg.buf[3] = buf3 & 0xff;
  msg.buf[4] = buf4 & 0xff;
  msg.buf[5] = buf5 & 0xff;
  msg.buf[6] = buf6 & 0xff;
  msg.buf[7] = buf7 & 0xff;

  Can0.write(msg);
}

// Listening infrastructure

LinkedListNode<TeensyCANFunction> * firstNode;
int lastmailbox = 0;
Listener listener0;

void listen_begin(){
  Can0.attachObj(&listener0);
}

class Listener : public CANListner 
{
public:
  void gotFrame(CAN_message_t &frame, int mailbox);
};
void Listner::gotFrame(CAN_message_t &frame, int mailbox){
  LinkedListNode<TeensyCANFunction> * nextNode = firstNode;
  for (int i = (nextmailbox - 1); i >= 0; i --){
    if (i == mailbox){
      byte * msg = new byte[8];
      memcpy(msg, rxmsg.buf, 8);

      node->data->call(msg);
      delete msg;
      break;
    }
    node = node->next;
  }
}

class TeensyCANFunction
{
public:
  uint32_t CANid;
  TeensyCANFunction(uint32_t id, void (*callback)(byte* msg));
  void call(byte* msg);
protected:
  void (*callback)(byte* msg);
}

TeensyCANFunction::TeensyCANFunction(uint32_t id, void (*callback)(byte* msg))
 : callback(callback){
  CANid = id
}

void TeensyCANFunction::call(byte * msg){
  callback(msg);
}

void add_id(uint32_t id, void (*callback)(byte* msg)){
  TeensyCANFunction newFunction(id, callback);
  if(firstNode == NULL){
    firstNode = new LinkedListNode<TeensyCANFunction>;
    firstNode->data = newFunction;
    firstNode->next = NULL;
  }
  else{
    LinkedListNode<TeensyCANFunction> * lastFirst = firstNode;
    firstNode = new LinkedListNode<TeensyCANFunction>;
    firstNode->data = newFunction;
    firstNode->next = lastFirst;
  }
  // Adding filter to listener
  CAN_filter_t newFilter;
  newFilter.id=id;
  newFilter.flags.ext=0;
  newFilter.flags.rtr=0;

  Can0.setFilter(newFilter,lastmailbox);
  listener0.attachMBHandler(lastmailbox);
  lastmailbox ++;
}
