#ifndef Fifo_h
#define Fifo_h

#include <Arduino.h>

#define TTL 4000
#define FIFO_SIZE 50
#define EVENTMANAGER_LISTENER_LIST_SIZE 8

/*
 * All arithmetic operations on windows are entirely computed every time,
 * no temporary values are stored, this is an impl choice, no benchmarks
 * have been done.
 *
 * The tail of the fifo indicates the next free element, thus a fifo of size
 * n can hold n-1 element, that's why the constructor should increment the
 * given parameter size to fit what provided size.
 */
class Fifo
{
  public:
    Fifo(unsigned int size, unsigned int step = 0);
    ~Fifo();

    int length();
    int available();
    boolean isEmpty();
    boolean isFull();

    //int avg();
    //Dump the content of the event queue formatted to the Serial output
    void dump();
    //Fifo *filterGreater(int threshold);

    boolean queueEventM1(int param);
    boolear queueEventM2();

    boolean popEvent(int* eventCode, void* eventParam);

    struct EventElement
    {
      unsigned int code;
      void* content;
      unsigned long stamp;
    };

    EventElement operator[](int idx);
    EventElement* fifo;
    int fifo_head = 0;
    int fifo_tail = 0;
    unsigned int mStep;
    unsigned int mSize;
  private:
};

class TemporalFifo
{
  public:
    TemporalFifo(unsigned int length, unsigned int interval);
    ~TemporalFifo();

    int length();
    int available();
    boolean isEmpty();
    boolean isFull();

    int avg();
    void dump();
    void trigger();

    TemporalFifo* filterGreater(int threshold);

    boolean queueEvent(int eventCode, int eventParam, boolean timeCheck = true, unsigned long time = millis());
    boolean popEvent(int* eventCode, int* eventParam);

    int fifo_head = 0;
    int fifo_tail = 0;
    unsigned int mInterval;

    struct TemporalEventElement
    {
      //TODO: use a smaller variable size by storing only the time spent modulo
      //the time window: millis() % mLength 
      unsigned long stamp;
      int code;
      int param;
    };

    TemporalEventElement operator[](int idx);
  private:

    unsigned int sum = 0;
    unsigned int mLength = 0;
    unsigned long lastAdd = 0;
    TemporalEventElement* fifo;
};

#endif
