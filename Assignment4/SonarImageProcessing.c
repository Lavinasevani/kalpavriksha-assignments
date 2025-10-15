#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// MACROS for configuration
#define MIN_MATRIX_SIZE 2
#define MAX_MATRIX_SIZE 10
#define MIN_PIXEL_OF_IMAGE 0
#define MAX_PIXEL_OF_IMAGE 255

// function to fill the matrix with random pixel intensity values (0–255)
void generateSonarImage(int matrixSize, int (*image)[matrixSize]) {
    for(int i = 0; i < matrixSize; i++) { //row
        for(int j = 0; j < matrixSize; j++) { //column
           *(*(image + i) + j) = rand() % (MAX_PIXEL_OF_IMAGE - MIN_PIXEL_OF_IMAGE + 1) + MIN_PIXEL_OF_IMAGE; 
        }
    }
}

// function to swap two integer values using pointers
void swap(int *num1, int *num2) {
    int temp = *num1;
    *num1 = *num2;
    *num2 = temp;
}

void rotateImage(int matrixSize, int (*image)[matrixSize]) {
    // Step 1. Transpose of orginal matrix
    for (int i = 0; i < matrixSize; i++) {
        for (int j = i+1; j < matrixSize; j++) {
            swap(&*(*(image+i)+j), &*(*(image+j)+i));
        }
    }

    // Step 2. Reverse rows
    for (int i =0 ; i < matrixSize; i++){
        for(int j = 0, k = matrixSize - 1; j < k; j++, k--){
            swap(&*(*(image+i)+j), &*(*(image+i)+k));
        }
    }
}

void smoothingFilter(int matrixSize, int (*image)[matrixSize]) {

    int smoothedRow[matrixSize];  // temporary storage for a row

    for (int i = 0; i < matrixSize; i++) { //row
        for (int j = 0; j < matrixSize; j++) { //column

            int sumOfElements = 0, numberOfElements = 0; // for 3 x 3 window of neighbor

            // Traverse all neighbors in 3x3 window centered at (i, j)
            for (int rowOffset = -1; rowOffset <= 1; rowOffset++) { 
                for (int colOffset = -1; colOffset <= 1; colOffset++) {
                    int neighborRow = i + rowOffset;
                    int neighborCol = j + colOffset;
                    // Check bounds and include valid neighboring cells only
                    if (neighborRow >= 0 && neighborRow < matrixSize && neighborCol >= 0 && neighborCol < matrixSize) {
                        sumOfElements += *(*(image + neighborRow) + neighborCol);
                        numberOfElements++;
                    }
                }
            }
            smoothedRow[j] = sumOfElements / numberOfElements;  // store average pixel
        }

        // Copy temporary smoothedRow back to image row
        for (int j = 0; j < matrixSize; j++) {
            *(*(image + i) + j) = smoothedRow[j];
        }
    }
}

void displayImage(int matrixSize, int (*image)[matrixSize]) {
    for(int i = 0; i < matrixSize; i++) {
        for(int j = 0; j < matrixSize; j++) {
           printf("%3d  ", (*(*(image + i) + j))); // "%3d " for 3-digit spacing alignment
        }
        printf("\n");
    }
}

int main() {

    int matrixSize;

    printf("Enter matrix size (%d - %d): ", MIN_MATRIX_SIZE, MAX_MATRIX_SIZE);

    // validate user input
    if (scanf("%d", &matrixSize) != 1) {
        printf("Invalid Input.");
        return 1;
    }
    else if (matrixSize < MIN_MATRIX_SIZE || matrixSize > MAX_MATRIX_SIZE) {
        printf("Invalid Matrix Size.");
        return 1;
    }

    int originalImage[matrixSize][matrixSize]; // as the size of image is small so storing into stack memory
    srand(time(NULL)); // Seed random generator to genrate diffrent images at each run

    generateSonarImage(matrixSize, originalImage);

    printf("Original:\n");
    displayImage(matrixSize, originalImage);

    printf("Rotated:\n");
    rotateImage(matrixSize, originalImage);
    displayImage(matrixSize, originalImage);

    printf("Final Output:\n");
    smoothingFilter(matrixSize, originalImage);
    displayImage(matrixSize, originalImage);

    return 0;

}