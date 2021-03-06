#ifndef INC_MAZEGENERATOR_H_
#define INC_MAZEGENERATOR_H_

typedef struct Stack Stack; // forward declaration

extern int directions[];

void shuffle(int* array, int n);

void carve_maze(int* maze, unsigned width, unsigned height, Stack* x_stack, Stack* y_stack);

int* getMaze(unsigned width, unsigned height);

void drawMaze(int* maze, unsigned sizeX, unsigned sizeY, unsigned startX, unsigned startY, unsigned cellSize, bool finish);

#endif /* INC_MAZEGENERATOR_H_ */
