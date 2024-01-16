#define MAX_QUEUE_SIZE 2048

typedef struct free_block_queue{
    int front, rear;
    int items[MAX_QUEUE_SIZE];
} Queue;

void initializeQueue(Queue *q);
int isQueueEmpty(Queue *q);
int isQueueFull(Queue *q);
void enqueue(Queue *q, int value);
int dequeue(Queue *q);
int getQueueSize(Queue *q);
int front(Queue *q);
int rear(Queue *q);