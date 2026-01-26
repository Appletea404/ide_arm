#include <stdio.h>      // 표준 입출력 함수 사용 (printf 등)
#include <stdlib.h>     // 동적 메모리 함수 사용 (malloc, free 등)
#include <stdbool.h>    // bool 타입과 true/false 키워드 사용

// 노드(Node) 구조체 정의
// 연결 리스트의 기본 단위. 스택에 들어가는 하나의 "상자"라고 생각하면 됨.
typedef struct Node
{
    int data;           // 이 노드에 저장할 실제 값
    struct Node *next;  // 다음 노드를 가리키는 포인터 (연결 고리)
} Node;

// 스택의 가장 위(top)를 가리키는 포인터
// 아무 것도 없는 초기 상태에서는 NULL로 설정
Node *top = NULL;		//top값이 NULL이면 아무것도 안 가리키고있다는 뜻

// 스택이 비어있는지 확인하는 함수
// top이 NULL이면 스택이 비어있는 상태
bool isEmpty()
{
    return top == NULL;
}

// push 연산: 데이터를 스택에 추가 (맨 위에 새로운 노드를 추가)
void push(int value)
{
    // 새 노드 동적 할당
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        printf("메모리 할당 실패!\n");
        return;
    }

    // 새 노드에 데이터 저장
    newNode->data = value;

    // 새 노드가 현재 top을 가리키도록 설정
    // 기존 top은 그 다음 위치로 밀리게 됨
    newNode->next = top;

    // top을 새 노드로 변경 → 스택의 가장 위가 이 새 노드가 됨
    top = newNode;

    printf("PUSH: %d\n", value);
}

// pop 연산: 데이터를 스택에서 제거 (맨 위 노드를 삭제)
int pop()
{
    if (isEmpty())
    {
        printf("스택이 비어 있습니다! POP 불가\n");
        return -1;
    }

    // top 노드를 임시로 저장
    Node *temp = top;

    // 해당 노드의 데이터를 저장해둠 (반환용)
    int value = temp->data;

    // top을 다음 노드로 이동
    top = top->next;

    // 꺼낸 노드의 메모리를 해제
    free(temp);

    printf("POP: %d\n", value);
    return value;
}

// peek 연산: 가장 위(top)에 있는 데이터를 확인 (삭제 X)
int peek()
{
    if (isEmpty())
    {
        printf("스택이 비어 있습니다! 확인 불가\n");
        return -1;
    }

    // top의 값 반환
    return top->data;
}

// 현재 스택 상태를 출력하는 함수
void printStack()
{
    printf("스택 상태 [Top → Bottom]: ");

    // 현재 노드를 top부터 시작해서 아래로 내려가며 출력
    Node *current = top;
    while (current != NULL)
    {
        printf("%d ", current->data);
        current = current->next;
    }

    printf("\n");
}

// 테스트용 main 함수
int main()
{
    // 스택에 값 삽입
    push(100);
    push(200);
    push(300);

    // 현재 스택 상태 출력 (Top → Bottom)
    printStack();  // 출력: 300 200 100

    // pop 연산 실행 (맨 위 300 제거)
    pop();

    // 상태 재출력
    printStack();  // 출력: 200 100

    // peek: 현재 top 값 확인
    printf("Top element: %d\n", peek());  // 출력: 200

    return 0;
}




