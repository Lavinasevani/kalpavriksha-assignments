#include <stdio.h>
#include <stdbool.h>
struct User {
    int uid;
    char name[100];
    int age;
};
void createUser(){
    FILE *fptr;
    fptr = fopen("user.txt" , "a");
    struct User u;
    printf("Enter ID , Name , age (Saprated by space) :\n");
    scanf("%d %s %d" , &u.uid , u.name ,&u.age); 
    fprintf(fptr,"%d %s %d\n" ,u.uid,u.name,u.age);
    fclose(fptr);
    printf("User Created.\n");
}
void readUser(){
    FILE *fptr;
    fptr = fopen("user.txt" , "r");
    int UID,age;
    char name[100];
    printf("UID\tName\tAge\n");
    while(fscanf(fptr , "%d %s %d", &UID ,&name, &age)==3){
        printf("%d\t%s\t%d\n",UID , name ,age);
    }
    fclose(fptr);
    printf("All User fetched.");
}
int main(){
    int choice ;
    while(true)
    {
        printf("\nEnter the number to select your choice :\n1.Create User \n2.Read Users \n3.Update User \n4.Delete User \n5.Exit\n");
        scanf("%d",&choice);
        if(choice==1){
            createUser();
        }
        else if(choice==2){
            readUser();
        }
        else if(choice==3){
            printf("User Updated.");
        }
        else if(choice==4){
             printf("User Deleted.");
        }
        else if(choice==5){
            printf("Exiting.. \nTHANK YOU !!");
            break;
        }
        else{
            printf("Invalid Number.");
        }
    }  
    return 0;
}