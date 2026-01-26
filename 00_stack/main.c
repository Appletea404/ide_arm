#include "stdio.h"
#include "stdbool.h"

// 스택의 크기를 5로 정의
#define STACK_SIZE 5

int stack[STACK_SIZE];		//배열 선언 (크기는 5)

//스택의 가장 위를 가리키는 인덱스 변수
//-1는 "스택이 비어있다" 라는 것을 의미
int top = -1;
/**
 * Stack 구조
 * LIFO
 * 마지막에 넣으면 먼저나옴
 */
bool isFull()
{
	return top == STACK_SIZE - 1;
}

bool isEmpty()
{
	//top이 -1이면 아무런 원소가 없음
	return top == -1;
}

/**
 * 스택에 값을 넣는 연산(push)
 */

void push(int value)
{
	if(isFull())
	{
		printf("스택이 가득 찼습니다\n");//더이상 삽입이 불가
		return;
	}

	//top 값을 하나 올리고 해당 위치에 값을 저장
	top = top + 1;
	stack[top] = value;
	printf("PUSH : %d\n",value);
}

/**
 * 스택에서 값을 꺼내는 연산 (POP)
 */

int pop()
{
	if(isEmpty())
	{
		printf("스택이 비어있습니다\n");
		return -1;
	}

	//top 위치의 값을 저장한 후, top 감수
	int value = stack[top];
	top = top - 1;
	printf("POP : %d\n", value);
	return value;
}

/**
 * 스택의 가장 위에 있는 값을 확인하는 연산(PEEK)
 */

int peek()
{
	if(isEmpty())
	{
		printf("스택이 비어있습니다 \n");  //꺼낼것이 없다
		return -1;
	}
	return stack[top];				//top위치 값을 반환
}

/**
 * 현재 스택의 모든 요소를 출력 하는 함수
 */

void printStack()
{
	printf("스택상태 [Bottom -> Top] : ");
	//아래에서부터 top까지 순서대로 출력
	for(int i = 0; i<= top; i++)
	{
		printf("%d ", stack[i]);
	}
	printf("\n");
}

int main()
{
	//스택에 값을 3개 넣기
	push(10);
	push(20);
	push(30);

	//현재상태 출력
	printStack();			//10,20,30

	pop();

	printStack();

	printf("Top Element : %d\n",peek());

	return 0;
}
