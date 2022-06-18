/*
 * stack.h
 *
 *	source:  https://github.com/Figglewatts/mazegen
 *
 *	Created on: Apr 16, 2018
 *		Author: Figglewatts
 *
 */

#ifndef INC_STACK_H_
#define INC_STACK_H_

#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

typedef struct Stack{
	int top;
	unsigned capacity;
	int* array;
} Stack;

Stack* stack_create(unsigned capacity) {
	Stack* stack = (Stack*) malloc(sizeof(Stack));
	stack->capacity = capacity;
	stack->top = -1;
	stack->array = (int*) malloc(stack->capacity * sizeof(int));
	return stack;
}

bool stack_isfull(Stack* stack) {
	return stack->top == stack->capacity - 1;
}

bool stack_isempty(Stack* stack) {
	return stack->top == -1;
}

void stack_grow(Stack* stack, unsigned size) {
	if (stack->capacity >= size)
		return;
	stack->array = realloc(stack->array, size * sizeof(int));
	stack->capacity = size;
}

void stack_push(Stack* stack, int item) {
	if (stack_isfull(stack))
		stack_grow(stack, stack->capacity * 2);
	stack->array[++stack->top] = item;
}

int stack_pop(Stack* stack) {
	if (stack_isempty(stack))
		return INT_MIN;
	return stack->array[stack->top--];
}

#endif /* INC_STACK_H_ */
