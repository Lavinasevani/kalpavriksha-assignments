#include <stdio.h>
#include <stdbool.h>
// Structure of user 
struct User {
    int uid;
    char name[100];
    int age;
};
void createUser(){
    struct User u , tUser;
    int id;
    printf("Enter ID , Name , age (Saprated by space) :\n");
    scanf("%d %s %d" , &u.uid , u.name ,&u.age); // user input to add new user 
    FILE *tptr;
    tptr = fopen("user.txt" , "r");
    if (tptr != NULL) {
     //Check to ensure UID is unique
    while(fscanf(tptr , "%d %s %d", &tUser.uid, tUser.name ,&tUser.age)==3){
        if(tUser.uid== u.uid){
            printf("User already exist.");
            fclose(tptr);
            return;
        }
    }
    fclose(tptr);
    }
    FILE *fptr;
    fptr = fopen("user.txt" , "a");
    if (fptr == NULL) {
        printf("Error: Could not open file for writing.\n");
        return;
    }
    fprintf(fptr,"%d %s %d\n" ,u.uid,u.name,u.age); //Add the new user to the file -> user.txt
    fclose(fptr);
    printf("User Created.\n"); // Success
}
void readUser(){
    FILE *fptr;
    fptr = fopen("user.txt" , "r");
    if( fptr == NULL ) { //Check if user.txt exists or not 
        printf("No user file found.\n" );
        return;
    }
    int UID,age;
    char name[100];
    printf("UID\tName\tAge\n");
    while(fscanf(fptr , "%d %s %d", &UID ,name, &age)==3){
        printf("%d\t%s\t%d\n",UID , name ,age); // to display all users
    }
    fclose(fptr);
    printf("All User fetched."); // Success
}
void updateUser(){
    FILE *fptr, *tptr;  
    struct User u;
    int id = 0 , age =0;
    char name[100];
    bool found = false;
    //user input to update data
    printf("Enter Id to Update the user : ");
    scanf("%d",&id); 
    printf("Enter name and age (saprated by space) : ");
    scanf("%s %d",name ,&age);
    fptr = fopen("user.txt", "r");   // original file
    if( fptr == NULL ) {
        printf("No user file found.\n" );
        return;
    }
    tptr = fopen("temp.txt","w"); // temp file
    // copy the data from user.txt -> temp.txt
    while (fscanf(fptr,"%d %s %d", &u.uid, u.name ,&u.age) ==3 ) {
        if (u.uid == id) { // id found write updated data
            fprintf(tptr, "%d %s %d\n", id, name, age);
            found = true; 
        }else{
            fprintf(tptr, "%d %s %d\n", u.uid, u.name, u.age);
        }
    }
    fclose(fptr);
    fclose(tptr);
     //update the original file with temp file
    remove("user.txt");
    rename("temp.txt","user.txt");
    if(found) printf("User id %d updated  with %s %d.",id ,name,age); // Success
    else printf("User not found.");
}
void deleteUser(){
    FILE *fptr, *tptr;
    struct User u;
    int id = 0;
    bool found = false;
    printf("Enter Id to delete the user : ");
    scanf("%d",&id);
    fptr = fopen("user.txt", "r"); // original file
    if( fptr == NULL ) {
        printf("No user file found.\n" );
        return;
    }
    tptr = fopen("temp.txt","w"); // temp file to store the data of remaining users
    while (fscanf(fptr,"%d %s %d", &u.uid, u.name ,&u.age) ==3 ) {
        if (u.uid == id) {
            found = true; 
            continue;  // Skip the deleted user
        }
        fprintf(tptr, "%d %s %d\n", u.uid, u.name, u.age);
    }
    fclose(fptr);
    fclose(tptr);
    //update the original file with temp file
    remove("user.txt");
    rename("temp.txt","user.txt");
    if(found) printf("User with id %d deleted.",id); // Success
    else printf("User not found.");
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
            updateUser();
        }
        else if(choice==4){
             deleteUser();
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