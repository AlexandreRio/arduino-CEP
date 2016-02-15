#include "cep.h"

/*****************************************************/
// ComplexEventManager part
/*****************************************************/

ComplexEventManager::ComplexEventManager() : mFifo()
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
