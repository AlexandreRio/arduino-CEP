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

    int avg();
    //Dump the content of the event queue formatted to the Serial output
    void dump();
    Fifo *filterGreater(int threshold);

    boolean queueEvent(int eventCode, int eventParam);
    boolean popEvent(int* eventCode, int* eventParam);

  private:
    struct EventElement
    {
      int code;
      int param;
    };

    EventElement* fifo[];
    unsigned int setp = 0;
    int fifo_head = 0;
    int fifo_tail = 0;
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
    TemporalEventElement fifo[FIFO_SIZE];
};

#endif
