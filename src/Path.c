// path.c
// by Gabriel Nopper

#include <stdio.h>


int queueSize = 0; // Where the queue ends

void dequeueTwo(int *array, int *x, int *y);
void enqueueTwo(int *array, int x, int y);

/**
 * Sets given matrix'es shortest paths from target (X,Y) point, around all map. 
 * Width and height are matrix properties.
 * Returns -1 is left row is empty.
**/
int setShortestPaths(int array[17][13], int width, int height, int targetX, int targetY){
	
	int queue [300]; // Queue
	int currentX, currentY, currentValue;
    currentX = 0;
    currentY = 0;


	int i, j;

	// 1 - Clear all array, sets any positive value to 0.
	for(i = 0; i < width; i++)
		for(j = 0; j < height; j++){
			int gridValue = array[i][j];
			if(gridValue > 0)	// Ou seja, não é bloco intransferivel. Se algo ja for 0 esse set é desnecessário, embora não devessem existir zeros.
				array[i][j] = 0;
		}


	// 2 - Enqueue target X and Y to Queue, and it's value to 1.
	array[targetX][targetY] = 1;
	enqueueTwo(queue, targetX, targetY);

    int reached_left_corner = 0;
	// 3 - Starts Loop
	while(queueSize){
		dequeueTwo(queue, &currentX, &currentY);
		currentValue = array[currentX][currentY];

		if(currentX < width - 1){ // Sees right >
			int temp = array[currentX + 1][currentY];
			if(temp > currentValue + 1 || temp == 0){
				array[currentX + 1][currentY] = currentValue + 1;
				enqueueTwo(queue, currentX + 1, currentY);
			}
		}
		if(currentY > 0){ // Sees up /
			int temp = array[currentX][currentY - 1];
			if(temp > currentValue + 1 || temp == 0){
				array[currentX][currentY - 1] = currentValue + 1;
				enqueueTwo(queue, currentX, currentY - 1);
			}
		}
		if(currentX > 0){ // Sees left <
			int temp = array[currentX - 1][currentY];
			if(temp > currentValue + 1 || temp == 0){
				array[currentX - 1][currentY] = currentValue + 1;
				enqueueTwo(queue, currentX - 1, currentY);
			}
		}
        else{
            reached_left_corner = 1;
        }
		if(currentY < height - 1){ // Sees down /
			int temp = array[currentX][currentY + 1];
			if(temp > currentValue + 1 || temp == 0){
				array[currentX][currentY + 1] = currentValue + 1;
				enqueueTwo(queue, currentX, currentY + 1);
			}
		}
	}

	// 4 - Theorical success;
    
    if(reached_left_corner)
        return 1;
    else
        return 0;

}

void dequeueTwo(int *array, int *x, int *y){
	if(queueSize < 2){
		printf("Queue has insufficient Size. WTF have you done??\n Size: %d\n", queueSize);
		return;
	}
	int i;
	*x = array[queueSize - 2];
	*y = array[queueSize - 1];
	for(i = 0; i < queueSize - 2; i++)	// Shifts all array to the left. Prevents overflow.
		array[i] = array[i + 2];
	queueSize -= 2;
}

void enqueueTwo(int *array, int x, int y){
    
	array[queueSize] = x;
	array[queueSize + 1] = y;
	queueSize += 2;
	return;
}