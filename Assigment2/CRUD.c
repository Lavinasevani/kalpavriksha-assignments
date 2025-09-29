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
            printf("All User fetched.");
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