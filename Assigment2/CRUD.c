#include <stdio.h>
#include <stdbool.h>
int main(){
    int choice ;
    int i =5;
    while(true)
    {
        printf("\nEnter the number to select your choice :\n1.Create User \n2.Read Users \n3.Update User \n4.Delete User \n5.Exit\n");
        scanf("%d",&choice);
        if(choice==1){
            printf("User Created.");
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