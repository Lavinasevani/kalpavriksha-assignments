#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// MACROS for configuration
#define MIN_MATRIX_SIZE 2
#define MAX_MATRIX_SIZE 10
#define MIN_PIXEL_OF_IMAGE 0
#define MAX_PIXEL_OF_IMAGE 255

// function to fill the matrix with random pixel intensity values (0–255)
void generateSonarImage(int matrixSize, int **image) {
    for(int rowIndex = 0; rowIndex < matrixSize; rowIndex++) { //row
        for(int columnIndex = 0; columnIndex < matrixSize; columnIndex++) { //column
           *(*(image + rowIndex) + columnIndex) = rand() % (MAX_PIXEL_OF_IMAGE - MIN_PIXEL_OF_IMAGE + 1) + MIN_PIXEL_OF_IMAGE; 
        }
    }
}

// function to swap two integer values using pointers
void swapPixels(int *pixel1, int *pixel2) {
    int temp = *pixel1;
    *pixel1 = *pixel2;
    *pixel2 = temp;
}

void rotateImage(int matrixSize, int **image) {
    // Step 1. Transpose of orginal matrix (swap [i][j] <-> [j][i])
    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++) {
        for (int columnIndex = rowIndex + 1; columnIndex < matrixSize; columnIndex++) { // This operation is done only for the upper triangle (i < j)
            swapPixels(&*(*(image+rowIndex)+columnIndex), &*(*(image+columnIndex)+rowIndex)); // Each pixel at (i, j) is swapped with (j, i).
        }
    }

    // Step 2. Reverse each rows: after transposing, reversing each row gives a 90° clockwise rotation
    for (int rowIndex =0 ; rowIndex < matrixSize; rowIndex++){
        for(int columnIndex = 0, endColumnIndex = matrixSize - 1; columnIndex < endColumnIndex; columnIndex++, endColumnIndex--){
            swapPixels(&*(*(image+rowIndex)+columnIndex), &*(*(image+rowIndex)+endColumnIndex)); // Swap first and last elements in the row moving inward
        }
    }
}

void applySmoothingFilter(int matrixSize, int **image) {

    // Pass 1: compute smoothed value and encode it in higher bits
    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++) {
         int *rowPointer = *(image + rowIndex); // Cache pointer to this row
        for (int columnIndex = 0; columnIndex < matrixSize; columnIndex++) {
            int sumOfElements = 0, numberOfElements = 0;

            for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                int neighborRow = rowIndex + rowOffset;
                if (neighborRow < 0 || neighborRow >= matrixSize) 
                continue;
                int *neighborRowPointer = *(image + neighborRow); // Cache neighbor row pointer
                for (int colOffset = -1; colOffset <= 1; colOffset++) {
                    int neighborCol = columnIndex + colOffset;
                    if (neighborCol < 0 || neighborCol >= matrixSize)
                    continue;
                    // '& 0xFF' ensures we only read the original (lower byte) value
                    sumOfElements += (*(neighborRowPointer + neighborCol)) & 0xFF; // faster derefrencing
                    numberOfElements++;
                }
            }

            // Compute rounded average value (sum / count, with +count/2 for rounding)
            int averagePixel = (sumOfElements + numberOfElements / 2) / numberOfElements;
            *(rowPointer + columnIndex) |= (averagePixel << 8);// Store this new pixel temporarily in higher 8 bits of same integer cell
        }
    }

    // Pass 2: move the smoothed values into the lower byte
    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++) {
        for (int columnIndex = 0; columnIndex < matrixSize; columnIndex++) {
            // shift right by 8 bits to move new value into the lower byte
            *(*(image + rowIndex) + columnIndex) >>= 8;
        }
    }
}


void displayImage(int matrixSize, int **image) {
    for(int rowIndex = 0; rowIndex < matrixSize; rowIndex++) {
        for(int columnIndex = 0; columnIndex < matrixSize; columnIndex++) {
           printf("%3d  ", (*(*(image + rowIndex) + columnIndex))); // "%3d " for 3-digit spacing alignment
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


    // Dynamic memory allocation (heap)
    int **originalImage = (int **)malloc(matrixSize * sizeof(int *));
    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++) {
        *(originalImage + rowIndex) = (int *)malloc(matrixSize * sizeof(int));
    }

    srand(time(NULL)); // Seed random generator to genrate diffrent images at each run

    generateSonarImage(matrixSize, originalImage);

    printf("Original:\n");
    displayImage(matrixSize, originalImage);

    printf("Rotated:\n");
    rotateImage(matrixSize, originalImage);
    displayImage(matrixSize, originalImage);

    printf("Final Output:\n");
    applySmoothingFilter(matrixSize, originalImage);
    displayImage(matrixSize, originalImage);

    // Free allocated heap memory
    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++) {
        free(*(originalImage + rowIndex));
    }
    free(originalImage);

    return 0;

}