#Arduino-CEP

Partially based on this: https://github.com/igormiktor/arduino-EventManager

Current approach is to use a queue buffer to store event and perform operations,
such as filtering and arithmetic. Goal is also to perform operations on multiple
queues, like merging.
