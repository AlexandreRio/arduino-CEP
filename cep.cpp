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

    union param_t {
      int content;
      byte buffer[2];
    } param_un;
    param_un.content = param;
    m1_fifo[m1_fifo_tail] = param_un.buffer[1] & 0xFF;
    m1_fifo[m1_fifo_tail + 1] = param_un.buffer[0] & 0xFF;

    union stamp_t {
      unsigned long time;
      byte time_buffer[TIMESTAMP_DATASIZE];
    } stamp;
    stamp.time = millis();
    m1_fifo[m1_fifo_tail + 2] = stamp.time_buffer[3] & 0xFF;
    m1_fifo[m1_fifo_tail + 3] = stamp.time_buffer[2] & 0xFF;
    m1_fifo[m1_fifo_tail + 4] = stamp.time_buffer[1] & 0xFF;
    m1_fifo[m1_fifo_tail + 5] = stamp.time_buffer[0] & 0xFF;

    m1_fifo_tail = (m1_fifo_tail + M1_ELEMENT_SIZE) % M1_FIFO_SIZE;
  }

  checkTrigger();
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
    m2_fifo[m2_fifo_tail + 1] = stamp.time_buffer[2] & 0xFF;
    m2_fifo[m2_fifo_tail + 2] = stamp.time_buffer[1] & 0xFF;
    m2_fifo[m2_fifo_tail + 3] = stamp.time_buffer[0] & 0xFF;

    m2_fifo_tail = (m2_fifo_tail + M2_ELEMENT_SIZE) % M2_FIFO_SIZE;
  }

  checkTrigger();
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
//    //Serial.print(lb + tmpEventElmt.code + co + c[0] + rb);
//
//    i = (i +1)%FIFO_SIZE;
//  }
//  Serial.println();
//
}
