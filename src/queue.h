#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

void pushQueue(Queue **, const char *, GMutex *);
Queue *getLastQueue(Queue **);
char *popQueue(Queue **, GMutex *);
Queue *getQueueByIndex(Queue *, gushort);
gushort countQueue(Queue *);
void freeQueue(Queue **);

#endif // QUEUE_H_INCLUDED
