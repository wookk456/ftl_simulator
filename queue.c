#include "queue.h"
#include <stdio.h>

// 큐 초기화
void initializeQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}

// 큐가 비어있는지 확인
int isQueueEmpty(Queue *q) {
    return q->front == -1;
}

// 큐가 가득 차있는지 확인
int isQueueFull(Queue *q) {
    return (q->rear + 1) % MAX_QUEUE_SIZE == q->front;
}

// 큐에 요소 추가
void enqueue(Queue *q, int value) {
    if (isQueueFull(q)) {
        printf("Queue is full. Cannot enqueue.\n");
    } else {
        if (isQueueEmpty(q)) {
            q->front = 0;  // 큐가 비어있을 때 front를 0으로 초기화
        }
        q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
        q->items[q->rear] = value;
    }
}

// 큐에서 요소 제거
int dequeue(Queue *q) {
    int value = -1;
    if (isQueueEmpty(q)) {
        printf("Queue is empty. Cannot dequeue.\n");
    } else {
        value = q->items[q->front];
        if (q->front == q->rear) {
            // 큐에 마지막 요소를 제거한 경우, 큐 초기화
            initializeQueue(q);
        } else {
            q->front = (q->front + 1) % MAX_QUEUE_SIZE;
        }
    }
    return value;
}

int getQueueSize(Queue *q) {
    if (isQueueEmpty(q)) {
        return 0;
    } else {
        return (q->rear - q->front + MAX_QUEUE_SIZE + 1) % MAX_QUEUE_SIZE;
    }
}

// 큐의 front에 있는 요소 반환
int front(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("Queue is empty. No front element.\n");
        return -1;
    } else {
        return q->items[q->front];
    }
}

// 큐의 rear에 있는 요소 반환
int rear(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("Queue is empty. No rear element.\n");
        return -1;
    } else {
        return q->items[q->rear];
    }
}