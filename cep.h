#ifndef Fifo_h
#define Fifo_h

#include <Arduino.h>

#define FIFO_SIZE 12
#define EVENTMANAGER_LISTENER_LIST_SIZE 8

class ComplexEventManager
{

  public:
    typedef void ( *EventListener )( int eventCode, int eventParam );

    enum EventType
    {
      kEventNone = 200,
      kEventSensor1,
      kEventSensor2
    };

    ComplexEventManager();

    // Add a listener
    // Returns true if the listener is successfully installed, false otherwise (e.g. the dispatch table is full)
    boolean addListener( int eventCode, EventListener listener );

    // Remove (event, listener) pair (all occurrences)
    // Other listeners with the same function or event code will not be affected
    boolean removeListener( int eventCode, EventListener listener );

    // Remove all occurrances of a listener
    // Removes this listener regardless of the event code; returns number removed
    // Useful when one listener handles many different events
    int removeListener( EventListener listener );

    // Enable or disable a listener
    // Return true if the listener was successfully enabled or disabled, false if the listener was not found
    boolean enableListener( int eventCode, EventListener listener, boolean enable );

    // Returns the current enabled/disabled state of the (eventCode, listener) combo
    boolean isListenerEnabled( int eventCode, EventListener listener );

    // The default listener is a callback function that is called when an event with no listener is processed
    // These functions set, clear, and enable/disable the default listener
    boolean setDefaultListener( EventListener listener );
    void removeDefaultListener();
    void enableDefaultListener( boolean enable );

    // Is the ListenerList empty?
    boolean isListenerListEmpty();

    // Is the ListenerList full?
    boolean isListenerListFull();

    int numListeners();

    // Returns true if no events are in the queue
    boolean isEventQueueEmpty();

    // Returns true if no more events can be inserted into the queue
    boolean isEventQueueFull();

    // Actual number of events in queue
    int getNumEventsInQueue();


    // tries to insert an event into the queue;
    // returns true if successful, false if the
    // queue if full and the event cannot be inserted
    boolean queueEvent( int eventCode, int eventParam);

    // this must be called regularly (usually by calling it inside the loop() function)
    int processEvent();

    // this function can be called to process ALL events in the queue
    // WARNING:  if interrupts are adding events as fast as they are being processed
    // this function might never return.  YOU HAVE BEEN WARNED.
    int processAllEvents();

    // A more general approach should be used here, something like a
    // function pointer to call and determine if a value must be kept
    // or discarded
    void filterGreater(int threshold);

    // compute the average _param_ value in the fifo
    int avg();

  private:
    /*
     * All arithmetic operations on windows are entirely computed every time,
     * no temporary values are stored, this is an impl choice, no benchmarks
     * have been done.
     *
     */
    class Fifo
    {
      public:
        Fifo();

        int length();
        int available();
        boolean isEmpty();
        boolean isFull();

        int avg();
        ComplexEventManager::Fifo filterGreater(int threshold);

        boolean queueEvent(int eventCode, int eventParam);
        boolean popEvent(int* eventCode, int* eventParam);

      private:
        struct EventElement
        {
          int code;
          int param;
        };

        EventElement fifo[FIFO_SIZE];
        int fifo_head = 0;
        int fifo_tail = 0;
    };

    // ListenerList class used internally by EventManager
    class ListenerList
    {

      public:

        // Create an event manager
        ListenerList();

        // Add a listener
        // Returns true if the listener is successfully installed, false otherwise (e.g. the dispatch table is full)
        boolean addListener( int eventCode, EventListener listener );

        // Remove event listener pair (all occurrences)
        // Other listeners with the same function or eventCode will not be affected
        boolean removeListener( int eventCode, EventListener listener );

        // Remove all occurrances of a listener
        // Removes this listener regardless of the eventCode; returns number removed
        int removeListener( EventListener listener );

        // Enable or disable a listener
        // Return true if the listener was successfully enabled or disabled, false if the listener was not found
        boolean enableListener( int eventCode, EventListener listener, boolean enable );

        boolean isListenerEnabled( int eventCode, EventListener listener );

        // The default listener is a callback function that is called when an event with no listener is processed
        boolean setDefaultListener( EventListener listener );
        void removeDefaultListener();
        void enableDefaultListener( boolean enable );

        // Is the ListenerList empty?
        boolean isEmpty();

        // Is the ListenerList full?
        boolean isFull();

        // Send an event to the listeners; returns number of listeners that handled the event
        int sendEvent( int eventCode, int param );

        int numListeners();

      private:

        // Maximum number of event/callback entries
        // Can be changed to save memory or allow more events to be dispatched
        static const int kMaxListeners = EVENTMANAGER_LISTENER_LIST_SIZE;

        // Actual number of event listeners
        int mNumListeners;

        // Listener structure and corresponding array
        struct ListenerItem
        {
          EventListener callback;// The listener function
          int eventCode;// The event code
          boolean enabled;// Each listener can be enabled or disabled
        };
        ListenerItem mListeners[ kMaxListeners ];

        // Callback function to be called for event types which have no listener
        EventListener mDefaultCallback;

        // Once set, the default callback function can be enabled or disabled
        boolean mDefaultCallbackEnabled;

        // get the current number of entries in the dispatch table
        int getNumEntries();

        // returns the array index of the specified listener or -1 if no such event/function couple is found
        int searchListeners( int eventCode, EventListener listener);
        int searchListeners( EventListener listener );
        int searchEventCode( int eventCode );

    };

    Fifo mFifo;
    ListenerList mListeners;
};

inline int ComplexEventManager::avg()
{
  return mFifo.avg();
}

inline void ComplexEventManager::filterGreater(int threshold)
{
  mFifo = mFifo.filterGreater(threshold);
}

inline boolean ComplexEventManager::addListener( int eventCode, EventListener listener )
{
  return mListeners.addListener( eventCode, listener );
}

inline boolean ComplexEventManager::removeListener( int eventCode, EventListener listener )
{
  return mListeners.removeListener( eventCode, listener );
}

inline int ComplexEventManager::removeListener( EventListener listener )
{
  return mListeners.removeListener( listener );
}

inline boolean ComplexEventManager::enableListener( int eventCode, EventListener listener, boolean enable )
{
  return mListeners.enableListener( eventCode, listener, enable );
}

inline boolean ComplexEventManager::isListenerEnabled( int eventCode, EventListener listener )
{
  return mListeners.isListenerEnabled( eventCode, listener );
}

inline boolean ComplexEventManager::setDefaultListener( EventListener listener )
{
  return mListeners.setDefaultListener( listener );
}

inline void ComplexEventManager::removeDefaultListener()
{
  mListeners.removeDefaultListener();
}

inline void ComplexEventManager::enableDefaultListener( boolean enable )
{
  mListeners.enableDefaultListener( enable );
}

inline boolean ComplexEventManager::isListenerListEmpty()
{
  return mListeners.isEmpty();
}

inline boolean ComplexEventManager::isListenerListFull()
{
  return mListeners.isFull();
}

inline boolean ComplexEventManager::isEventQueueFull()
{
  return mFifo.isFull();
}

inline boolean ComplexEventManager::isEventQueueEmpty()
{
  return mFifo.isEmpty();
}

inline int ComplexEventManager::getNumEventsInQueue()
{
  return mFifo.length();
}

inline boolean ComplexEventManager::queueEvent(int eventCode,int eventParam)
{
  return mFifo.queueEvent(eventCode, eventParam);
}

inline boolean ComplexEventManager::ListenerList::isEmpty()
{
  return (mNumListeners == 0);
}

inline boolean ComplexEventManager::ListenerList::isFull()
{
  return (mNumListeners == kMaxListeners);
}

inline int ComplexEventManager::ListenerList::getNumEntries()
{
  return mNumListeners;
}
#endif
