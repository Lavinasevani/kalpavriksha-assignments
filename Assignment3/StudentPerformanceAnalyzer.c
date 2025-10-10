#include <stdio.h>
#include <string.h>

// Macro definitions for program constraints
#define MAX_NAME_LENGTH 100
#define NUMBER_OF_SUBJECTS 3
#define MIN_NumberOfStudents 1
#define MAX_NumberOfStudents 100

// Structure to store student details
struct student{
    int RollNumber;
    char Name[MAX_NAME_LENGTH];
    int Marks[NUMBER_OF_SUBJECTS]; // Marks in 3 subjects
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
char getGrade(float averageMarks){
    if (averageMarks >= 85) return 'A';
    else if (averageMarks >= 70) return 'B';
    else if (averageMarks >= 50) return 'C';
    else if (averageMarks >= 35) return 'D';
    else return 'F'; //below 35 get F grade
}
 // Function to print performance stars based on grade
void  printPerformance(char grade){
    switch (grade)
    {
    case 'A':
        printf("Performance : *****\n"); //5
        break;
    case 'B':
        printf("Performance : ****\n"); //4
        break;
    case 'C':
        printf("Performance : ***\n\n"); //3
        break;
    case 'D':
        printf("Performance : **\n"); //2
        break;
    default:
        break;  //for grade F no stars printed
    }
}

// Recursive function to print roll numbers of all students
void printRollNumberRecursively(struct student students[] , int index , int totalStudents){
    if(index >= totalStudents) return;
    printf("%d ", students[index].RollNumber);
    printRollNumberRecursively(students, index+1, totalStudents); // recursive call
}

int main(){
    int numberOfStudents;
    
    printf("Enter number of Students : ");

     // Input number of students and validate
    if(scanf("%d",&numberOfStudents) != 1){
        printf("Invaild Number!!");
    }else if(numberOfStudents < MIN_NumberOfStudents || numberOfStudents > MAX_NumberOfStudents){
        printf("Invaild number it should be from %d to %d", MIN_NumberOfStudents , MAX_NumberOfStudents);
    }    
    else{

        struct  student students[numberOfStudents]; 

        int newRollNumber, newMark1, newMark2, newMark3;
        char newName[MAX_NAME_LENGTH];

        printf("Enter the student %d detail into following format : \n" ,numberOfStudents);
        printf("RollNumber Name Marks1 Marks2 Marks3\n");

        // Loop to take input for each student
        for(int i = 0; i < numberOfStudents ; i++){
            printf("Enter student %d details : ",i+1);
                scanf("%d %s %d %d %d",&newRollNumber ,newName, &newMark1, &newMark2, &newMark3);

                // Store values into the students array
                students[i].RollNumber = newRollNumber;
                strcpy(students[i].Name, newName);
                students[i].Marks[0] = newMark1;
                students[i].Marks[1] = newMark2;
                students[i].Marks[2] = newMark3;
        }

        // Loop to calculate and display student performance
        for(int i = 0; i < numberOfStudents ; i++){

                printf("RollNumber : %d \n",students[i].RollNumber);
                printf("Name : %s \n",students[i].Name);

                // Calculate total marks
                int totalMarks = getTotalMarks(students[i].Marks[0], students[i].Marks[1], students[i].Marks[2]);
                printf("Total Marks : %d \n", totalMarks);

                 // Calculate average marks
                float averageMarks = getAverageMarks(totalMarks);
                printf("Average Marks : %.2f \n", averageMarks);

                // Determine grade
                char grade = getGrade(averageMarks);

                // Print performance stars (will print nothing if grade F)
                printPerformance(grade); 
                printf("\n");     
        }

        // Print all roll numbers recursively
        printf("List of Roll Numbers (via recursion):");
        printRollNumberRecursively(students , 0, numberOfStudents);
    }
    return 0;
}