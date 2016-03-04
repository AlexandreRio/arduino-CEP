#include "cep.h"

/*****************************************************/
// FIFO part
/*****************************************************/

streamFilteredJoin1_Fifo::streamFilteredJoin1_Fifo()
{}

streamFilteredJoin1_Fifo::~streamFilteredJoin1_Fifo()
{}

void streamFilteredJoin1_Fifo::checkTrigger()
{
  if (!m1_isEmpty() && !m2_isEmpty())
  {
    // pop all the events
    m1_popEvent();
    m2_popEvent();

    // May need to surround this with another guard on all
    // the events. So pop should reconstruct the events.

    // create the output event
    // See the rest of the generated ThingML code
  }
}

/*****************************************************/
// m1 handling part
/*****************************************************/

int streamFilteredJoin1_Fifo::m1_length()
{
  if (m1_fifo_tail >= m1_fifo_head)
    return m1_fifo_tail - m1_fifo_head;
  return m1_fifo_tail + M1_FIFO_SIZE - m1_fifo_head;
}

int streamFilteredJoin1_Fifo::m1_available()
{
  return M1_FIFO_SIZE - 1 - m1_length();
}

inline boolean streamFilteredJoin1_Fifo::m1_isFull()
{
  return m1_fifo_head == ((m1_fifo_tail + 1) % M1_FIFO_SIZE);
}


inline boolean streamFilteredJoin1_Fifo::m1_isEmpty()
{
  return m1_fifo_head == m1_fifo_tail;
}

void streamFilteredJoin1_Fifo::m1_queueEvent(int param)
{

  if (m1_available() >= M1_ELEMENT_SIZE)
  {

    union stamp_t {
      unsigned long time;
      byte time_buffer[TIMESTAMP_DATASIZE];
    } stamp;
    stamp.time = millis();
    m1_fifo[m1_fifo_tail    ] = stamp.time_buffer[3] & 0xFF;
    m1_fifo[m1_fifo_tail + 1] = stamp.time_buffer[2] & 0xFF;
    m1_fifo[m1_fifo_tail + 2] = stamp.time_buffer[1] & 0xFF;
    m1_fifo[m1_fifo_tail + 3] = stamp.time_buffer[0] & 0xFF;

    union param_t {
      int content;
      byte buffer[2];
    } param_un;
    param_un.content = param;
    m1_fifo[m1_fifo_tail + 4] = param_un.buffer[1] & 0xFF;
    m1_fifo[m1_fifo_tail + 5] = param_un.buffer[0] & 0xFF;

    m1_fifo_tail = (m1_fifo_tail + M1_ELEMENT_SIZE) % M1_FIFO_SIZE;
  }

  checkTrigger();
}

void streamFilteredJoin1_Fifo::m1_popEvent()
{
  // For optimize purposes the guard should be removed
  // because we manage when the method is called
  if (!m1_isEmpty())
  {

    union stamp_t {
      unsigned long time;
      byte time_buffer[TIMESTAMP_DATASIZE];
    } stamp;

    stamp.time_buffer[3] = m1_fifo[m1_fifo_head];
    stamp.time_buffer[2] = m1_fifo[(m1_fifo_head + 1) % M1_FIFO_SIZE];
    stamp.time_buffer[1] = m1_fifo[(m1_fifo_head + 2) % M1_FIFO_SIZE];
    stamp.time_buffer[0] = m1_fifo[(m1_fifo_head + 3) % M1_FIFO_SIZE];

    union param_t {
      int content;
      byte buffer[2];
    } param;
    param.buffer[1] = m1_fifo[(m1_fifo_head + 4) % M1_FIFO_SIZE];
    param.buffer[0] = m1_fifo[(m1_fifo_head + 5) % M1_FIFO_SIZE];

    m1_fifo_head = (m1_fifo_head + M1_ELEMENT_SIZE) % M1_FIFO_SIZE;
  }

}

/*****************************************************/
// m2 handling part
/*****************************************************/

int streamFilteredJoin1_Fifo::m2_length()
{
  if (m2_fifo_tail >= m2_fifo_head)
    return m2_fifo_tail - m2_fifo_head;
  return m2_fifo_tail + M2_FIFO_SIZE - m2_fifo_head;
}

int streamFilteredJoin1_Fifo::m2_available()
{
  return M2_FIFO_SIZE - 1 - m2_length();
}

inline boolean streamFilteredJoin1_Fifo::m2_isFull()
{
  return m2_fifo_head == ((m2_fifo_tail + 1) % M2_FIFO_SIZE);
}


inline boolean streamFilteredJoin1_Fifo::m2_isEmpty()
{
  return m2_fifo_head == m2_fifo_tail;
}

void streamFilteredJoin1_Fifo::m2_queueEvent()
{

  if (m2_available() >= M2_ELEMENT_SIZE)
  {

    union stamp_t {
      unsigned long time;
      byte time_buffer[TIMESTAMP_DATASIZE];
    } stamp;
    stamp.time = millis();
    m2_fifo[m2_fifo_tail    ] = stamp.time_buffer[3] & 0xFF;
    m2_fifo[(m2_fifo_tail + 1) % M2_FIFO_SIZE] = stamp.time_buffer[2] & 0xFF;
    m2_fifo[(m2_fifo_tail + 2) % M2_FIFO_SIZE] = stamp.time_buffer[1] & 0xFF;
    m2_fifo[(m2_fifo_tail + 3) % M2_FIFO_SIZE] = stamp.time_buffer[0] & 0xFF;

    m2_fifo_tail = (m2_fifo_tail + M2_ELEMENT_SIZE) % M2_FIFO_SIZE;
  }

  checkTrigger();
}

void streamFilteredJoin1_Fifo::m2_popEvent()
{
  if (!m2_isEmpty())
  {

    union stamp_t {
      unsigned long time;
      byte time_buffer[TIMESTAMP_DATASIZE];
    } stamp;

    stamp.time_buffer[3] = m2_fifo[m2_fifo_head];
    stamp.time_buffer[2] = m2_fifo[(m2_fifo_head + 1) % M2_FIFO_SIZE];
    stamp.time_buffer[1] = m2_fifo[(m2_fifo_head + 2) % M2_FIFO_SIZE];
    stamp.time_buffer[0] = m2_fifo[(m2_fifo_head + 3) % M2_FIFO_SIZE];

    m2_fifo_head = (m2_fifo_head + M2_ELEMENT_SIZE) % M2_FIFO_SIZE;
  }
}

void streamFilteredJoin1_Fifo::dump()
{
//  int i = m1_fifo_head;
//  EventElement tmpEventElmt;
//  String lb = "<";
//  String co = ",";
//  String rb = ">";
//
//  while (i != m1_fifo_tail)
//  {
//    tmpEventElmt = fifo[i];
//
//    //if (tmpEventElmt.code == 0) {
//    //  byte c[] = (*tmpEventElmt.content);
//    //}
//    ////Serial.print(lb + tmpEventElmt.code + co + c[0] + rb);
//
//    i = (i +1)%FIFO_SIZE;
//  }
//  //Serial.println();
//
}
