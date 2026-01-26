/**
 * Circular_q
 *
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define SIZE 5


typedef struct
{
	int data[SIZE];
	int front;
	int rear;
}CircularQueue;

void initQueue(CircularQueue *q)
{
	q->front = 0;
	q->rear = 0;
}

bool isEmpty(CircularQueue *q)
{
	return q->front == q->rear;
}

bool isFull(CircularQueue *q)
{
	return (q->rear + 1) % SIZE == q->front;
}

bool enqueue(CircularQueue *q, int value)
{
	if(isFull(q))
	{
		printf("Queue is full\n");
		return false;
	}

	q->rear = (q->rear + 1) % SIZE;
	q->data[q->rear] = value;
	return true;
}

bool dequeue(CircularQueue *q, int *value)
{
    if (isEmpty(q))
    {
        printf("Queue is empty!\n"); // 큐가 비어있으면 제거 실패
        return false;
    }
    q->front = (q->front + 1) % SIZE;
    *value = q->data[q->front];			// front 위치의 값 반환 후 front 증가
    return true;
}

void printQueue(CircularQueue *q)
{

    // front부터 rear 바로 전까지 출력

	int i = (q->front + 1) % SIZE;
	printf("Queue: ");
	while (i != (q->rear + 1) % SIZE) {
		printf("%d " , q->data[i]);
		i = (i + 1) % SIZE;
	}
	printf("\n");
}



int main()
{
	CircularQueue q;
	initQueue(&q);

	enqueue(&q, 10);
	enqueue(&q, 20);
	enqueue(&q, 30);

	int val;
	dequeue(&q, &val);
	printf("Dequeue : %d \n", val);
	printQueue(&q);

	enqueue(&q, 40);
	enqueue(&q, 50);
	printQueue(&q);

	enqueue(&q, 60);


	return 0;
}



