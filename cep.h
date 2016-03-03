#ifndef Fifo_h
#define Fifo_h

#include <Arduino.h>

#define TTL 4000
#define TIMESTAMP_DATASIZE 4
#define DEFAULT_NUMBER_MSG 10

// computed size: buffer_size * (stamp size + sum(data_size))
#define M1_NUMBER_MSG     DEFAULT_NUMBER_MSG
#define M1_ELEMENT_SIZE   (TIMESTAMP_DATASIZE + 2)
#define M1_FIFO_SIZE      ((M1_NUMBER_MSG * M1_ELEMENT_SIZE) + 1)

#define M2_NUMBER_MSG     DEFAULT_NUMBER_MSG
#define M2_ELEMENT_SIZE   (TIMESTAMP_DATASIZE)
#define M2_FIFO_SIZE      ((M2_NUMBER_MSG * M2_ELEMENT_SIZE) + 1)

class streamFilteredJoin1_Fifo
{
  public:
    streamFilteredJoin1_Fifo();
    ~streamFilteredJoin1_Fifo();

    int m1_length();
    int m2_length();

    int m1_available();
    int m2_available();

    boolean m1_isEmpty();
    boolean m2_isEmpty();

    boolean m1_isFull();
    boolean m2_isFull();

    void dump();

    void m1_queueEvent(int param);
    void m2_queueEvent();

    void checkTrigger();

    //boolean popEvent(int* eventCode, void* eventParam);

  private:
    byte m1_fifo[M1_FIFO_SIZE];
    byte m2_fifo[M2_FIFO_SIZE];

    unsigned int m1_fifo_head = 0;
    unsigned int m1_fifo_tail = 0;

    unsigned int m2_fifo_head = 0;
    unsigned int m2_fifo_tail = 0;

};

#endif
