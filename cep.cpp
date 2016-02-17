#include "cep.h"

/*****************************************************/
// ComplexEventManager part
/*****************************************************/

ComplexEventManager::ComplexEventManager(unsigned int length, unsigned int interval) : mFifo(length, interval) // first param is actually ignored
{}

int ComplexEventManager::processEvent()
{

  int eventCode;
  int param;
  int handledCount = 0;

  if ( mFifo.popEvent( &eventCode, &param ) )
  {
    handledCount = mListeners.sendEvent( eventCode, param );
  }

  return handledCount;
}

/*****************************************************/
// ListenerList part
/*****************************************************/

ComplexEventManager::ListenerList::ListenerList() :
  mNumListeners( 0 ), mDefaultCallback( 0 )
{
}

int ComplexEventManager::ListenerList::numListeners()
{
  return mNumListeners;
};

int ComplexEventManager::numListeners()
{
  return mListeners.numListeners();
};

boolean ComplexEventManager::ListenerList::addListener( int eventCode, EventListener listener )
{
  // Argument check
  if ( !listener )
  {
    return false;
  }

  // Check for full dispatch table
  if ( isFull() )
  {
    return false;
  }

  mListeners[ mNumListeners ].callback = listener;
  mListeners[ mNumListeners ].eventCode = eventCode;
  mListeners[ mNumListeners ].enabled 	= true;
  mNumListeners++;

  return true;
}


boolean ComplexEventManager::ListenerList::removeListener( int eventCode, EventListener listener )
{
  if ( mNumListeners == 0 )
  {
    return false;
  }

  int k = searchListeners( eventCode, listener );
  if ( k < 0 )
  {
    return false;
  }

  for ( int i = k; i < mNumListeners - 1; i++ )
  {
    mListeners[ i ].callback  = mListeners[ i + 1 ].callback;
    mListeners[ i ].eventCode = mListeners[ i + 1 ].eventCode;
    mListeners[ i ].enabled   = mListeners[ i + 1 ].enabled;
  }
  mNumListeners--;

  return true;
}


int ComplexEventManager::ListenerList::removeListener( EventListener listener )
{
  if ( mNumListeners == 0 )
  {
    return 0;
  }

  int removed = 0;
  int k;
  while ((k = searchListeners( listener )) >= 0 )
  {
    for ( int i = k; i < mNumListeners - 1; i++ )
    {
      mListeners[ i ].callback  = mListeners[ i + 1 ].callback;
      mListeners[ i ].eventCode = mListeners[ i + 1 ].eventCode;
      mListeners[ i ].enabled   = mListeners[ i + 1 ].enabled;
    }
    mNumListeners--;
    removed++;
  }

  return removed;
}


boolean ComplexEventManager::ListenerList::enableListener( int eventCode, EventListener listener, boolean enable )
{
  if ( mNumListeners == 0 )
  {
    return false;
  }

  int k = searchListeners( eventCode, listener );
  if ( k < 0 )
  {
    return false;
  }

  mListeners[ k ].enabled = enable;

  return true;
}


boolean ComplexEventManager::ListenerList::isListenerEnabled( int eventCode, EventListener listener )
{
  if ( mNumListeners == 0 )
  {
    return false;
  }

  int k = searchListeners( eventCode, listener );
  if ( k < 0 )
  {
    return false;
  }

  return mListeners[ k ].enabled;
}


int ComplexEventManager::ListenerList::sendEvent( int eventCode, int param )
{
  int handlerCount = 0;
  for ( int i = 0; i < mNumListeners; i++ )
  {
    if ( ( mListeners[ i ].callback != 0 ) && ( mListeners[ i ].eventCode == eventCode ) && mListeners[ i ].enabled )
    {
      handlerCount++;
      (*mListeners[ i ].callback)( eventCode, param );
    }
  }

  if ( !handlerCount )
  {
    if ( ( mDefaultCallback != 0 ) && mDefaultCallbackEnabled )
    {
      handlerCount++;
      (*mDefaultCallback)( eventCode, param );

    }

  }

  return handlerCount;
}


boolean ComplexEventManager::ListenerList::setDefaultListener( EventListener listener )
{
  if ( listener == 0 )
  {
    return false;
  }

  mDefaultCallback = listener;
  mDefaultCallbackEnabled = true;
  return true;
}


void ComplexEventManager::ListenerList::removeDefaultListener()
{
  mDefaultCallback = 0;
  mDefaultCallbackEnabled = false;
}


void ComplexEventManager::ListenerList::enableDefaultListener( boolean enable )
{
  mDefaultCallbackEnabled = enable;
}


int ComplexEventManager::ListenerList::searchListeners( int eventCode, EventListener listener )
{

  for ( int i = 0; i < mNumListeners; i++ )
  {


    if ( ( mListeners[i].eventCode == eventCode ) && ( mListeners[i].callback == listener ) )
    {
      return i;
    }
  }

  return -1;
}


int ComplexEventManager::ListenerList::searchListeners( EventListener listener )
{
  for ( int i = 0; i < mNumListeners; i++ )
  {
    if ( mListeners[i].callback == listener )
    {
      return i;
    }
  }

  return -1;
}


int ComplexEventManager::ListenerList::searchEventCode( int eventCode )
{
  for ( int i = 0; i < mNumListeners; i++ )
  {
    if ( mListeners[i].eventCode == eventCode )
    {
      return i;
    }
  }

  return -1;
}

/*****************************************************/
// FIFO part
/*****************************************************/

ComplexEventManager::Fifo::Fifo()
{}

int ComplexEventManager::Fifo::length()
{
  if (fifo_tail >= fifo_head)
    return fifo_tail - fifo_head;
  return fifo_tail + FIFO_SIZE - fifo_head;
}

boolean ComplexEventManager::Fifo::isFull()
{
  return fifo_head == ((fifo_tail + 1) % FIFO_SIZE);
}


boolean ComplexEventManager::Fifo::isEmpty()
{
  return fifo_head == fifo_tail;
}

boolean ComplexEventManager::Fifo::queueEvent(int eventCode, int eventParam)
{
  int new_tail = (fifo_tail + 1) % FIFO_SIZE;
  if (new_tail == fifo_head)
  {
    fifo_head = (fifo_head + 1) % FIFO_SIZE;
  }

  fifo[fifo_tail].code = eventCode;
  fifo[fifo_tail].param = eventParam;
  fifo_tail = new_tail;
  return 1;
}

boolean ComplexEventManager::Fifo::popEvent(int* eventCode, int* eventParam)
{
  if (isEmpty())
    return false;

  *eventCode = fifo[fifo_head].code;
  *eventParam = fifo[fifo_head].param;
  fifo_head = (fifo_head + 1)%FIFO_SIZE;
  return true;
}

int ComplexEventManager::Fifo::avg()
{
  dump();
  unsigned long sum = 0;
  int i = fifo_head;
  while (i != fifo_tail)
  {
    sum += fifo[i].param;
    i = (i +1)%FIFO_SIZE;
  }
  return sum/length();
}

void ComplexEventManager::Fifo::dump()
{
  int i = fifo_head;
  EventElement tmpEventElmt;
  String lb = "<";
  String co = ",";
  String rb = ">";

  while (i != fifo_tail)
  {
    tmpEventElmt = fifo[i];

    Serial.print(lb + tmpEventElmt.code + co + tmpEventElmt.param + rb);

    i = (i +1)%FIFO_SIZE;
  }
  Serial.println();

}

ComplexEventManager::Fifo* ComplexEventManager::Fifo::filterGreater(int threshold)
{
  String s = "filtering in progress ";
  int n = length();
  String s1 = " [";
  String s2 = ",";
  String s3 = "]";
  Serial.println(s + n + s1 + fifo_head + s2 + fifo_tail + s3);

  ComplexEventManager::Fifo new_fifo;
  int i = fifo_head;
  while (i != fifo_tail)
  {
    if (fifo[i].param >= threshold)
    {
      new_fifo.queueEvent(fifo[i].code, fifo[i].param);
    }
    i = (i +1)%FIFO_SIZE;
  }

  return &new_fifo;
}

/*****************************************************/
// TemporalFifo part
/*****************************************************/
ComplexEventManager::TemporalFifo::TemporalFifo(unsigned int length, unsigned int interval) : mLength(length), mInterval(interval)
{}

int ComplexEventManager::TemporalFifo::avg()
{
  return sum/length();
}



boolean ComplexEventManager::TemporalFifo::queueEvent(int eventCode, int eventParam, boolean timeCheck, unsigned long time)
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

void ComplexEventManager::TemporalFifo::dump()
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

void ComplexEventManager::TemporalFifo::trigger()
{
  // look for oldest data
  if ((length() > 0 )&& (millis() - fifo[fifo_head].stamp) > TTL)
  {
    sum -= fifo[fifo_head].param;
    fifo_head = (fifo_head + 1) % FIFO_SIZE;
  }
}

int ComplexEventManager::TemporalFifo::length()
{
  if (fifo_tail >= fifo_head)
    return fifo_tail - fifo_head;
  return fifo_tail + FIFO_SIZE - fifo_head;
}

boolean ComplexEventManager::TemporalFifo::isFull()
{
  return fifo_head == ((fifo_tail + 1) % FIFO_SIZE);
}

ComplexEventManager::TemporalFifo* ComplexEventManager::TemporalFifo::filterGreater(int threshold)
{
  String s = "filtering in progress ";
  int n = length();
  String s1 = " [";
  String s2 = ",";
  String s3 = "]";
  Serial.println(s + n + s1 + fifo_head + s2 + fifo_tail + s3);

  ComplexEventManager::TemporalFifo new_fifo(0, mInterval);
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

ComplexEventManager::TemporalFifo::TemporalEventElement ComplexEventManager::TemporalFifo::operator[](int idx)
{
  return fifo[idx];
}

ComplexEventManager* ComplexEventManager::join(ComplexEventManager* cm1, ComplexEventManager* cm2, unsigned int width)
{
  int i  = cm1->mFifo.fifo_head;
  int ii = cm1->mFifo.fifo_tail;
  int j  = cm2->mFifo.fifo_head;
  int jj = cm2->mFifo.fifo_tail;

  ComplexEventManager cmRet(0, cm1->mFifo.mInterval);

  ComplexEventManager::TemporalFifo::TemporalEventElement t1 = cm1->mFifo[i];
  ComplexEventManager::TemporalFifo::TemporalEventElement t2 = cm2->mFifo[j];
  while (i != ii && j != jj && !cmRet.isEventQueueFull())
  {
    unsigned int diff = (t1.stamp > t2.stamp) ? t1.stamp - t2.stamp : t2.stamp - t1.stamp;

    if (diff <= width)
    {
      cmRet.queueEvent(ComplexEventManager::kEventNone, 42, false, t1.stamp);
    }

    i = (i+1) % FIFO_SIZE;
    t1 = cm1->mFifo[i];
    j = (j+1) % FIFO_SIZE;
    t2 = cm2->mFifo[j];
  }

  return &cmRet;
}

ComplexEventManager* ComplexEventManager::merge(ComplexEventManager* cm1, ComplexEventManager* cm2)
{
  int i  = cm1->mFifo.fifo_head;
  int ii = cm1->mFifo.fifo_tail;
  int j  = cm2->mFifo.fifo_head;
  int jj = cm2->mFifo.fifo_tail;

  ComplexEventManager cmRet(0, cm1->mFifo.mInterval);

  Serial.println("Merging window");

  ComplexEventManager::TemporalFifo::TemporalEventElement t1 = cm1->mFifo[i];
  ComplexEventManager::TemporalFifo::TemporalEventElement t2 = cm2->mFifo[j];
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

