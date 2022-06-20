#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
#include "ugui.h"


int directions[] = { 0, 1, 2, 3 };

void shuffle(int* array, int n)
{
	if (n > 1)
	{
		for (int i = 0; i < n - 1; i++)
		{
			int j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
}

void carve_maze(int* maze, unsigned width, unsigned height, Stack* x_stack, Stack* y_stack)
{
	while (!stack_isempty(x_stack))
	{
		shuffle(directions, 4); // randomizes directions

		int x = stack_pop(x_stack);
		int y = stack_pop(y_stack);

		for (int i = 0; i < 4; i++)
		{
			switch (directions[i])
			{
				case 0: // up
				{
					if (y - 2 <= 0) continue;

					if (maze[(y - 2) * width + x] != 0)
					{
						maze[(y - 2) * width + x] = 0;
						maze[(y - 1) * width + x] = 0;
						stack_push(x_stack, x);
						stack_push(y_stack, y - 2);
					}
					break;
				}
				case 1: // right
				{
					if (x + 2 >= width - 1) continue;

					if (maze[y * width + x + 2] != 0)
					{
						maze[y * width + x + 2] = 0;
						maze[y * width + x + 1] = 0;
						stack_push(x_stack, x + 2);
						stack_push(y_stack, y);
					}
					break;
				}
				case 2: // down
				{
					if (y + 2 >= height - 1) continue;

					if (maze[(y + 2) * width + x] != 0)
					{
						maze[(y + 2) * width + x] = 0;
						maze[(y + 1) * width + x] = 0;
						stack_push(x_stack, x);
						stack_push(y_stack, y + 2);
					}
					break;
				}
				case 3: // left
				{
					if (x - 2 <= 0) continue;

					if (maze[y * width + x - 2] != 0)
					{
						maze[y * width + x - 2] = 0;
						maze[y * width + x - 1] = 0;
						stack_push(x_stack, x - 2);
						stack_push(y_stack, y);
					}
					break;
				}
			}
		}
	}
}


int* getMaze(int w, int h){
	int* maze = (int*) malloc(sizeof(int)*w*h); // allocate space for a maze
	for(int i = 0; i<h*w;i++)  maze[i] = 1; // fill a maze with walls
	
	Stack* x_stack = stack_create(64);
	Stack* y_stack = stack_create(64);

	// start in top left
	int x = 1;
	int y = 1;

  stack_push(x_stack, x);
  stack_push(y_stack, y);

	maze[y * w + x] = 0;

	carve_maze(maze, w, h, x_stack, y_stack);

	free(x_stack);
	free(y_stack);

	return maze;
}

void drawMaze(int* maze, int sizeX, int sizeY, int startX, int startY, int cellSize, bool finish){
	int x, y;
	for(int i = 0; i<sizeY-1; i+=2){
		for(int j = 0; j<sizeX-1; j+=2){
			if(maze[i*sizeX+j]==1){
				x = startX+(int)(j/2)*cellSize;
				y = startY+(int)(i/2)*cellSize;
				if(maze[(i+1)*sizeX+j]==1){
					UG_DrawLine(x,y,x,y+cellSize,C_WHITE);
				}
				if(maze[i*sizeX+j+1]==1){
					 UG_DrawLine(x,y,x+cellSize,y,C_WHITE);
				}
			}
		}
	}
	y = startY+(int)(sizeY/2)*cellSize;
	int maxy = (sizeY-1)*sizeX;
	for(int i = 0; i<sizeX-1; i+=2){
		if(maze[maxy+i]==1 && maze[maxy+i+1]==1){
			x = startX+(int)(i/2)*cellSize;
			UG_DrawLine(x,y,x+cellSize,y,C_WHITE);
		}
	}
	x = startX+(int)(sizeX/2)*cellSize;
	for(int i = 0; i<sizeY-1; i+=2){
		if(maze[i*sizeX+sizeX-1]==1 && maze[(i+1)*sizeX+sizeX-1]==1){
			y = startY+(int)(i/2)*cellSize;
			UG_DrawLine(x,y,x,y+cellSize,C_WHITE);
		}
	}

	if(finish) UG_DrawFrame(x-cellSize+1, startY+1, x-1, startY+cellSize-1, C_GREEN);
}
