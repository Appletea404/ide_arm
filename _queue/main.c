
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 큐 구조체 정의
typedef struct
{
    int *data;      // 동적 배열 포인터 (큐 데이터를 저장하는 배열)
    int front;      // 데이터를 꺼내는 위치 (첫 번째 원소)
    int rear;       // 데이터를 넣는 위치 (다음 삽입 위치)
    int capacity;   // 큐의 최대 크기
} Queue;

// 큐 초기화 함수
void initQueue(Queue *q, int size)
{
    q->data = (int*)malloc(sizeof(int) * size); // 지정된 크기만큼 메모리 동적 할당
    q->front = 0;       // front와 rear를 0으로 초기화
    q->rear = 0;
    q->capacity = size; // 큐의 최대 크기 설정
}

// 큐가 비었는지 확인
bool isEmpty(Queue *q)
{
    // front와 rear가 같으면 데이터가 없음
    return q->front == q->rear;
}

// 큐가 가득 찼는지 확인
bool isFull(Queue *q)
{
    // rear가 capacity에 도달하면 더 이상 삽입 불가
    return q->rear == q->capacity;
}

// 큐에 데이터 삽입
bool enqueue(Queue *q, int value)
{
    if (isFull(q))
    {
        printf("Queue is full!\n");  // 큐가 가득 차면 삽입 실패
        return false;
    }
    q->data[q->rear++] = value; // rear 위치에 값 삽입 후 rear 증가
    return true;
}

// 큐에서 데이터 제거
bool dequeue(Queue *q, int *value)
{
    if (isEmpty(q))
    {
        printf("Queue is empty!\n"); // 큐가 비어있으면 제거 실패
        return false;
    }
    *value = q->data[q->front++]; // front 위치의 값 반환 후 front 증가
    return true;
}

// 큐 내용 출력 (디버깅용)
void printQueue(Queue *q)
{
    printf("Queue: ");
    // front부터 rear 바로 전까지 출력
    for (int i = q->front; i < q->rear; i++)
    {
        printf("%d ", q->data[i]);
    }
    printf("\n");
}

// 큐 메모리 해제
void freeQueue(Queue *q)
{
    free(q->data); // 동적 메모리 해제
}

int main()
{
    Queue q;
    initQueue(&q, 5);  // 크기 5짜리 큐 생성

    // 큐에 데이터 삽입
    enqueue(&q, 10);
    enqueue(&q, 20);
    enqueue(&q, 30);
    printQueue(&q);  // 출력: 10 20 30

    int value;
    dequeue(&q, &value);   // 10 제거
    printf("Dequeued: %d\n", value);
    printQueue(&q);        // 출력: 20 30

    // 추가 삽입
    enqueue(&q, 40);
    enqueue(&q, 50);
    enqueue(&q, 60);       // 이 시점에서 rear == capacity → 삽입 실패
    printQueue(&q);        // 출력: 20 30 40 50

    freeQueue(&q);         // 사용한 메모리 정리
    return 0;
}
