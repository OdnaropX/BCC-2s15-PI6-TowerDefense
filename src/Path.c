<<<<<<< HEAD:Path.c
// path.c
// by Gabriel Nopper


int queueSize = 0; // Where the queue ends

void dequeueTwo(int *array, int x, int y);
void enqueueTwo(int *array, int x, int y);

/**
 * Sets given matrix'es shortest paths from target (X,Y) point, around all map. 
 * Width and height are matrix properties.
**/
int setShortestPaths(int **array, int width, int height, int targetX, int targetY){
	
	int queue [300]; // Queue
	int currentX, currentY, currentValue;


	int i, j;

	// 1 - Clear all array, sets any positive value to 0.
	for(i = 0; i < width, i++)
		for(j = 0; j < height; j++){
			int gridValue = array[i][j];
			if(gridValue > 0)	// Ou seja, não é bloco intransferivel. Se algo ja for 0 esse set é desnecessário, embora não devessem existir zeros.
				array[i][j] = 0;
		}


	// 2 - Enqueue target X and Y to Queue, and it's value to 1.
	array[targetX][targetY] = 1;
	enqueueTwo(queue, targetX, targetY);

	// 3 - Starts Loop
	while(queueSize){
		dequeueTwo(queue, currentX, currentY);
		currentValue = array[currentX][currentY];

		if(currentX > 0){ // Sees right >
			int temp = array[currentX + 1][currentY];
			if(temp < currentValue && temp >= 0){
				array[currentX + 1][currentY] = currentValue + 1;
				enqueueTwo(currentX + 1, currentY);
			}
		}
		if(currentY > 0){ // Sees up
			int temp = array[currentX][currentY + 1];
			if(temp < currentValue && temp >= 0){
				array[currentX][currentY + 1] = currentValue + 1;
				enqueueTwo(currentX, currentY + 1);
			}
		}
		if(currentX < width - 1){ // Sees left <
			int temp = array[currentX - 1][currentY];
			if(temp < currentValue && temp >= 0){
				array[currentX - 1][currentY] = currentValue + 1;
				enqueueTwo(currentX - 1, currentY);
			}
		}
		if(currentY < height - 1){ // Sees down 
			int temp = array[currentX][currentY - 1];
			if(temp < currentValue && temp >= 0){
				array[currentX][currentY - 1] = currentValue + 1;
				enqueueTwo(currentX, currentY - 1);
			}
		}
	}

	// 4 - Theorical success;
	printf("Path is set.\n");
	return 1;

}

void dequeueTwo(int *array, int x, int y){
	if(queueSize < 2){
		printf("Queue has insufficient Size. WTF have you done??\n Size: %d\n", queueSize);
		return;
	}
	int i;
	x = array[0];
	y = array[1];
	for(i = 0; i < queueSize - 2; i++)	// Shifts all array to the left. Prevents overflow.
		array[i] = array[i + 2];
	queueSize -= 2;
}

void enqueueTwo(int *array, int x, int y){
	array[queueSize] = x;
	array[queueSize + 1] = y;
	queueSize += 2;
	return;
=======
// path.c
// by Gabriel Nopper


int queueSize = 0; // Where the queue ends

void dequeueTwo(int *array, int x, int y);
void enqueueTwo(int *array, int x, int y);

/**
 * Sets given matrix'es shortest paths from target (X,Y) point, around all map. 
 * Width and height are matrix properties.
**/
int setShortestPaths(int **array, int width, int height, int targetX, int targetY){
	
	int queue [300]; // Queue
	int currentX, currentY, currentValue;


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

	// 3 - Starts Loop
	while(queueSize){
		dequeueTwo(queue, currentX, currentY);
		currentValue = array[currentX][currentY];

		if(currentX > 0){ // Sees right >
			int temp = array[currentX + 1][currentY];
			if(temp < currentValue && temp >= 0){
				array[currentX + 1][currentY] = currentValue + 1;
				enqueueTwo(queue, currentX + 1, currentY);
			}
		}
		if(currentY > 0){ // Sees up /
			int temp = array[currentX][currentY + 1];
			if(temp < currentValue && temp >= 0){
				array[currentX][currentY + 1] = currentValue + 1;
				enqueueTwo(queue, currentX, currentY + 1);
			}
		}
		if(currentX < width - 1){ // Sees left <
			int temp = array[currentX - 1][currentY];
			if(temp < currentValue && temp >= 0){
				array[currentX - 1][currentY] = currentValue + 1;
				enqueueTwo(queue, currentX - 1, currentY);
			}
		}
		if(currentY < height - 1){ // Sees down /
			int temp = array[currentX][currentY - 1];
			if(temp < currentValue && temp >= 0){
				array[currentX][currentY - 1] = currentValue + 1;
				enqueueTwo(queue, currentX, currentY - 1);
			}
		}
	}

	// 4 - Theorical success;
	printf("Path is set.\n");
	return 1;

}

void dequeueTwo(int *array, int x, int y){
	if(queueSize < 2){
		printf("Queue has insufficient Size. WTF have you done??\n Size: %d\n", queueSize);
		return;
	}
	int i;
	x = array[0];
	y = array[1];
	for(i = 0; i < queueSize - 2; i++)	// Shifts all array to the left. Prevents overflow.
		array[i] = array[i + 2];
	queueSize -= 2;
}

void enqueueTwo(int *array, int x, int y){
	array[queueSize] = x;
	array[queueSize + 1] = y;
	queueSize += 2;
	return;
>>>>>>> 234353775098e9c0439ec0380f831407fea2a882:src/Path.c
}