#include "cep.h"

/*****************************************************/
// FIFO part
/*****************************************************/

Fifo::Fifo(unsigned int size, unsigned int step) : mSize(size + 1), mStep(step)
{
  fifo = new EventElement[size + 1];
}

Fifo::~Fifo()
{
  free (fifo);
}

int Fifo::length()
{
  if (fifo_tail >= fifo_head)
    return fifo_tail - fifo_head;
  return fifo_tail + mSize - fifo_head;
}

inline boolean Fifo::isFull()
{
  return fifo_head == ((fifo_tail + 1) % mSize);
}


inline boolean Fifo::isEmpty()
{
  return fifo_head == fifo_tail;
}

boolean Fifo::queueEvent(unsigned int eventCode, void* eventContent)
{
  //TODO should consider the step attribute if the buffer is full
  int new_tail = (fifo_tail + 1) % mSize;
  if (new_tail == fifo_head)
  {
    free (fifo[fifo_head].content);
    fifo_head = (fifo_head + 1) % mSize;
  }

  fifo[fifo_tail].code    = eventCode;
  fifo[fifo_tail].content = eventContent; //TESTME
  fifo[fifo_tail].stamp   = millis(); //FIXME use a relative timestamp

  fifo_tail = new_tail;

  return 1;
}

boolean Fifo::popEvent(int* eventCode, void* eventParam)
{
  if (isEmpty())
    return false;

  *eventCode = fifo[fifo_head].code;
  //*eventParam = fifo[fifo_head].content;
  fifo_head = (fifo_head + 1)%FIFO_SIZE;
  return true;
}

Fifo::EventElement Fifo::operator[](int idx)
{
  //no check on the index value, thug life
  return fifo[idx];
}

//int Fifo::avg()
//{
//  dump();
//  unsigned long sum = 0;
//  int i = fifo_head;
//  while (i != fifo_tail)
//  {
//    sum += fifo[i].param;
//    i = (i +1)%FIFO_SIZE;
//  }
//  return sum/length();
//}

void Fifo::dump()
{
  int i = fifo_head;
  EventElement tmpEventElmt;
  String lb = "<";
  String co = ",";
  String rb = ">";

  while (i != fifo_tail)
  {
    tmpEventElmt = fifo[i];

    //if (tmpEventElmt.code == 0) {
    //  byte c[] = (*tmpEventElmt.content);
    //}
    //Serial.print(lb + tmpEventElmt.code + co + c[0] + rb);

    i = (i +1)%FIFO_SIZE;
  }
  Serial.println();

}

//Fifo* Fifo::filterGreater(int threshold)
//{
//  String s = "filtering in progress ";
//  int n = length();
//  String s1 = " [";
//  String s2 = ",";
//  String s3 = "]";
//  Serial.println(s + n + s1 + fifo_head + s2 + fifo_tail + s3);
//
//  Fifo new_fifo;
//  int i = fifo_head;
//  while (i != fifo_tail)
//  {
//    if (fifo[i].param >= threshold)
//    {
//      new_fifo.queueEvent(fifo[i].code, fifo[i].param);
//    }
//    i = (i +1)%FIFO_SIZE;
//  }
//
//  return &new_fifo;
//}

/*****************************************************/
// TemporalFifo part
/*****************************************************/
TemporalFifo::TemporalFifo(unsigned int length, unsigned int interval) : mLength(length), mInterval(interval)
{
  fifo = new TemporalEventElement[length];
}

TemporalFifo::~TemporalFifo()
{
  free (fifo);
}

int TemporalFifo::avg()
{
  return sum/length();
}



boolean TemporalFifo::queueEvent(int eventCode, int eventParam, boolean timeCheck, unsigned long time)
{
  if (timeCheck && (millis() - lastAdd) < mInterval)
    return 0; // litteraly too soon

  int new_tail = (fifo_tail + 1) % FIFO_SIZE;
  if (new_tail == fifo_head) //TODO should refactor moving the head
  {
    sum -= fifo[fifo_head].param;
    fifo_head = (fifo_head + 1) % FIFO_SIZE;
  }

  fifo[fifo_tail].code = eventCode;
  fifo[fifo_tail].param = eventParam;
  fifo[fifo_tail].stamp = time;
  sum += eventParam;
  fifo_tail = new_tail;
  lastAdd = millis();
  return 1;
}

void TemporalFifo::dump()
{
  int i = fifo_head;
  TemporalEventElement tmpEventElmt;
  String lb = "<";
  String co = ",";
  String rb = ">";

  while (i != fifo_tail)
  {
    tmpEventElmt = fifo[i];

    Serial.print(lb + tmpEventElmt.stamp + co + tmpEventElmt.code + co + tmpEventElmt.param + rb);

    i = (i +1)%FIFO_SIZE;
  }
  String m = " mean is: ";
  Serial.print(m + avg());
  Serial.println();
}

void TemporalFifo::trigger()
{
  // look for oldest data
  if ((length() > 0 )&& (millis() - fifo[fifo_head].stamp) > TTL)
  {
    sum -= fifo[fifo_head].param;
    fifo_head = (fifo_head + 1) % FIFO_SIZE;
  }
}

int TemporalFifo::length()
{
  if (fifo_tail >= fifo_head)
    return fifo_tail - fifo_head;
  return fifo_tail + FIFO_SIZE - fifo_head;
}

boolean TemporalFifo::isFull()
{
  return fifo_head == ((fifo_tail + 1) % FIFO_SIZE);
}

TemporalFifo* TemporalFifo::filterGreater(int threshold)
{
  String s = "filtering in progress ";
  int n = length();
  String s1 = " [";
  String s2 = ",";
  String s3 = "]";
  Serial.println(s + n + s1 + fifo_head + s2 + fifo_tail + s3);

  TemporalFifo new_fifo(0, mInterval);
  int i = fifo_head;
  while (i != fifo_tail)
  {
    if (fifo[i].param >= threshold)
    {
      new_fifo.queueEvent(fifo[i].code, fifo[i].param, false, fifo[i].stamp);
    }
    i = (i +1)%FIFO_SIZE;
  }

  return &new_fifo;

}

TemporalFifo::TemporalEventElement TemporalFifo::operator[](int idx)
{
  return fifo[idx];
}

/*
ComplexEventManager* join(ComplexEventManager* cm1, ComplexEventManager* cm2, unsigned int width)
{
  int i  = cm1->mFifo.fifo_head;
  int ii = cm1->mFifo.fifo_tail;
  int j  = cm2->mFifo.fifo_head;
  int jj = cm2->mFifo.fifo_tail;

  ComplexEventManager cmRet(0, cm1->mFifo.mInterval);

  TemporalFifo::TemporalEventElement t1 = cm1->mFifo[i];
  TemporalFifo::TemporalEventElement t2 = cm2->mFifo[j];
  while (i != ii && j != jj && !cmRet.isEventQueueFull())
  {
    unsigned int diff = (t1.stamp > t2.stamp) ? t1.stamp - t2.stamp : t2.stamp - t1.stamp;

    if (diff <= width)
    {
      cmRet.queueEvent(kEventNone, 42, false, t1.stamp);
    }

    i = (i+1) % FIFO_SIZE;
    t1 = cm1->mFifo[i];
    j = (j+1) % FIFO_SIZE;
    t2 = cm2->mFifo[j];
  }

  return &cmRet;
}

ComplexEventManager* merge(ComplexEventManager* cm1, ComplexEventManager* cm2)
{
  int i  = cm1->mFifo.fifo_head;
  int ii = cm1->mFifo.fifo_tail;
  int j  = cm2->mFifo.fifo_head;
  int jj = cm2->mFifo.fifo_tail;

  ComplexEventManager cmRet(0, cm1->mFifo.mInterval);

  Serial.println("Merging window");

  TemporalFifo::TemporalEventElement t1 = cm1->mFifo[i];
  TemporalFifo::TemporalEventElement t2 = cm2->mFifo[j];
  while ((i != ii || j != jj) && !cmRet.isEventQueueFull())
  {
    if (t1.stamp <= t2.stamp || j == jj)
    {
      cmRet.queueEvent(t1.code, t1.param, false, t1.stamp);
      i = (i+1) % FIFO_SIZE;
      t1 = cm1->mFifo[i];
    }
    else if (t1.stamp > t2.stamp || i == ii)
    {
      cmRet.queueEvent(t2.code, t2.param, false, t2.stamp);
      j = (j+1) % FIFO_SIZE;
      t2 = cm2->mFifo[j];
    }
  }

  Serial.print("done merging, new size is ");
  Serial.println(cmRet.getNumEventsInQueue());
  return &cmRet;
}

*/
