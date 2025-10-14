#include <stdio.h>
#include <string.h>

// Macro definitions for program constraints
#define MAX_NAME_LENGTH 100
#define NUMBER_OF_SUBJECTS 3
#define MIN_NUMBER_OF_STUDENTS 1
#define MAX_NUMBER_OF_STUDENTS 100

enum Grade { A, B, C, D, F };   // Enum for grades

// Structure to store student details
struct Student{
    int rollNumber;
    char name[MAX_NAME_LENGTH];
    int marks[NUMBER_OF_SUBJECTS]; // Marks in 3 subjects
};

// Function to calculate total marks of 3 subjects
int getTotalMarks(int marks1, int marks2, int marks3){
    return marks1 + marks2 + marks3;
}

// Function to calculate average marks
float getAverageMarks(int totalMarks){
    return totalMarks/3.0; // divide by 3.0 to get floating point result
}

// Function to assign grade based on average marks
enum Grade getGrade(float averageMarks) {
    if (averageMarks >= 85) return A;
    else if (averageMarks >= 70) return B;
    else if (averageMarks >= 50) return C;
    else if (averageMarks >= 35) return D;
    else return F;
}

// Function to convert enum to printable grade letter
const char* gradeToString(enum Grade g) {
    switch (g) {
        case A: return "A";
        case B: return "B";
        case C: return "C";
        case D: return "D";
        case F: return "F";
        default: return "";
    }
}

// Function to print performance stars based on grade
void printPerformance(enum Grade grade) {
    switch (grade) {
        case A:
            printf("Performance : *****\n");    //5
            break;
        case B:
            printf("Performance : ****\n");     //4
            break;
        case C:
            printf("Performance : ***\n");      //3
            break;
        case D:
            printf("Performance : **\n");       //2
            break;
        default:
            break; //for grade F no stars printed
    }
}

// Recursive function to print roll numbers of all students
void printRollNumber(struct Student students[] , int index , int totalStudents){
    if(index >= totalStudents) return;
    printf("%d ", students[index].rollNumber);
    printRollNumber(students, index+1, totalStudents); // recursive call
}

// Function to take input for all students
void inputStudentDetails(struct Student students[], int numberOfStudents){
    int rollNumber, mark1, mark2, mark3;
        char name[MAX_NAME_LENGTH];

        printf("Enter the student %d detail into following format : \n" ,numberOfStudents);
        printf("RollNumber Name Marks1 Marks2 Marks3\n");

        // Loop to take input for each student
        for(int i = 0; i < numberOfStudents ; i++){
            printf("Enter student %d details : ",i+1);
                if (scanf("%d %99s %d %d %d", &rollNumber, name, &mark1, &mark2, &mark3) != 5) {    // %99s limits the input extra characters are not read
                    printf("Invalid input format! Please enter again.\n");
                    i--;    // repeat this iteration
                    while (getchar() != '\n');  // clear the input buffer
                    continue;
                }
                // Store values into the students array
                students[i].rollNumber = rollNumber;
                strncpy(students[i].name, name, MAX_NAME_LENGTH - 1); //safely copy into the structure
                students[i].name[MAX_NAME_LENGTH - 1] = '\0';  // ensures null-terminated
                students[i].marks[0] = mark1;
                students[i].marks[1] = mark2;
                students[i].marks[2] = mark3;
        }
}

// Function to display performance of all students
void displayStudentPerformance(struct Student students[], int numberOfStudents) {
    // Loop to calculate and display student performance
    for(int i = 0; i < numberOfStudents ; i++){

        printf("RollNumber : %d \n",students[i].rollNumber);
        printf("Name : %s \n",students[i].name);

        // Calculate total marks
        int totalMarks = getTotalMarks(students[i].marks[0], students[i].marks[1], students[i].marks[2]);
        printf("Total Marks : %d \n", totalMarks);

        // Calculate average marks
        float averageMarks = getAverageMarks(totalMarks);
        printf("Average Marks : %.2f \n", averageMarks);

        // Determine grade
        enum Grade grade = getGrade(averageMarks);
        printf("Grade : %s\n", gradeToString(grade));

        // Print performance stars (will print nothing if grade F)
        printPerformance(grade); 
        printf("\n");    

    }

    // Print all roll numbers recursively
    printf("List of Roll Numbers (via recursion):");
    printRollNumber(students , 0, numberOfStudents);
}


int main(){
    int numberOfStudents;
    
    printf("Enter number of Students : ");

     // Input number of students and validate
    if(scanf("%d",&numberOfStudents) != 1){
        printf("Invalid Number!!");
        return 1;
    }else if(numberOfStudents < MIN_NUMBER_OF_STUDENTS|| numberOfStudents > MAX_NUMBER_OF_STUDENTS){
        printf("Invalid number it should be from %d to %d", MIN_NUMBER_OF_STUDENTS , MAX_NUMBER_OF_STUDENTS);
        return 1;
    }    
    struct  Student students[numberOfStudents]; 
    inputStudentDetails(students, numberOfStudents);
    displayStudentPerformance(students, numberOfStudents);

    return 0;
}