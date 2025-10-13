#include <stdio.h>
#include <string.h>

#define MAX_NAME_LENGTH 100
#define NUMBER_OF_SUBJECTS 3
#define MIN_NumberOfStudents 1
#define MAX_NumberOfStudents 100

struct student
{
    int RollNumber;
    char Name[MAX_NAME_LENGTH];
    int Marks[NUMBER_OF_SUBJECTS];
};

int main(){
    int numberOfStudents;
    printf("Enter number of Students : ");
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
        for(int i = 0; i < numberOfStudents ; i++){
            printf("Enter student %d details : ",i+1);
                scanf("%d %s %d %d %d",&newRollNumber ,newName, &newMark1, &newMark2, &newMark3);
                students[i].RollNumber = newRollNumber;
                strcpy(students[i].Name, newName);
                students[i].Marks[0] = newMark1;
                students[i].Marks[1] = newMark2;
                students[i].Marks[2] = newMark3;
        }
        for(int i = 0; i < numberOfStudents ; i++){
                printf("RollNumber : %d \n",students[i].RollNumber);
                printf("Name : %s \n",students[i].Name);
                printf("Marks1 : %d  Marks2 : %d Marks3 : %d \n",students[i].Marks[0] , students[i].Marks[1], students[i].Marks[2] );
        }
    }
    return 0;
}