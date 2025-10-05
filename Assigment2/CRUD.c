#include <stdio.h>
#include <stdbool.h>
#include <string.h>
// Structure of user 
struct User {
    int uid;
    char name[100];
    int age;
};

void createUser(){
    struct User newUser, tempUser;
    //INPUT Validations
    printf("Enter ID: "); 
    if (scanf("%d", &newUser.uid) != 1) {       //check for integer input for id
        printf("Error: Invalid input. ID must be a number.\n");
        while (getchar() != '\n'); 
        return;
    }
    getchar(); //clear buffer
    printf("Enter Name: ");
    fgets(newUser.name, sizeof(newUser.name), stdin);       // can take input "FirstName LastName"
    newUser.name[strcspn(newUser.name, "\n")] = '\0';       // remove the newline character added by fgets at the end of input 
    if (strlen(newUser.name) == 0) {        // Validate name is not empty
        printf("Error: Name cannot be empty.\n");
        return;
    }
    printf("Enter Age: ");
    if (scanf("%d", &newUser.age) != 1) {       //check for integer input for age
        printf("Error: Invalid input. Age must be a number.\n");
        while (getchar() != '\n'); 
        return;
    }
    if (newUser.age < 0 || newUser.age > 150) {     // Validate age range
        printf("Error: Please enter a valid age (0-150).\n");
        return;
    }
    // Checking for Unique user ID
    FILE *filePtr;
    filePtr = fopen("user.txt", "r");
    if (filePtr != NULL) {
        // Check to ensure UID is unique
            while (fscanf(filePtr, "%d", &tempUser.uid) == 1) {
                fgets(tempUser.name, sizeof(tempUser.name), filePtr);  
                fgets(tempUser.name, sizeof(tempUser.name), filePtr);  
                tempUser.name[strcspn(tempUser.name, "\n")] = '\0';
                fscanf(filePtr, "%d", &tempUser.age);
                
                if (tempUser.uid == newUser.uid) {
                    printf("Error: User with ID %d already exists.\n", newUser.uid);
                    fclose(filePtr);
                    return;
                }
        }
        fclose(filePtr);
    }
    // Opening file to add te user in append mode
   filePtr = fopen("user.txt", "a");
    if (filePtr == NULL) {
        printf("Error: Could not open file for writing.\n");
        return;
    }

    fprintf(filePtr, "%d\n%s\n%d\n", newUser.uid, newUser.name, newUser.age); // adding new user to the file
    fclose(filePtr);
    printf("Success: User created successfully.\n");

}

void readUser(){
     FILE *filePtr;
    filePtr = fopen("user.txt", "r");
    if (filePtr == NULL) { // Check if user.txt exists or not 
        printf("No user file found.\n");
        return;
    }
    int userId, userAge;
    char userName[100];
    printf("\n%-10s %-30s %-10s\n", "UID", "Name", "Age");
    while (fscanf(filePtr, "%d", &userId) == 1) {
        fgets(userName, sizeof(userName), filePtr); 
        fgets(userName, sizeof(userName), filePtr);
        userName[strcspn(userName, "\n")] = '\0'; 
        fscanf(filePtr, "%d", &userAge);   
        printf("%-10d %-30s %-10d\n", userId, userName, userAge);
    }
    fclose(filePtr);
    printf("\nAll users fetched successfully.\n");
}

void updateUser(){
    FILE *filePtr, *tempFilePtr;  
    struct User currentUser;
    int searchId, newAge;
    char newName[100];
    bool userFound = false;
    //user input to update data  
    printf("Enter User ID to update: ");
    if (scanf("%d", &searchId) != 1) {
        printf("Error: Invalid input. ID must be a number.\n");
        while (getchar() != '\n'); 
        return;
    }
    getchar();
    printf("Enter new name: "); //Get new name 
    fgets(newName, sizeof(newName), stdin);
    newName[strcspn(newName, "\n")] = '\0';  // remove the newline character added by fgets at the end of input 
    if (strlen(newName) == 0) {
        printf("Error: Name cannot be empty.\n");
        return;
    }
    printf("Enter new age: ");     // Get new age
    if (scanf("%d", &newAge) != 1) { 
        printf("Error: Invalid input. Age must be a number.\n");
        while (getchar() != '\n');
        return;
    }
    if (newAge < 0 || newAge > 150) {
        printf("Error: Please enter a valid age (0-150).\n");
        return;
    }
    filePtr = fopen("user.txt", "r"); //original file
    if (filePtr == NULL) {
        printf("No user file found.\n");
        return;
    }
    tempFilePtr = fopen("temp.txt", "w"); //temp file
    if (tempFilePtr == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(filePtr);
        return;
    }
    // copy the data from user.txt -> temp.txt
    while (fscanf(filePtr, "%d", &currentUser.uid) == 1) {
        fgets(currentUser.name, sizeof(currentUser.name), filePtr); 
        fgets(currentUser.name, sizeof(currentUser.name), filePtr); 
        currentUser.name[strcspn(currentUser.name, "\n")] = '\0';
        fscanf(filePtr, "%d", &currentUser.age);
        if (currentUser.uid == searchId) {
            fprintf(tempFilePtr, "%d\n%s\n%d\n", searchId, newName, newAge); // Write updated data
            userFound = true;
        } else {
            fprintf(tempFilePtr, "%d\n%s\n%d\n", currentUser.uid, currentUser.name, currentUser.age); // Write original data
        }
    }
    fclose(filePtr);
    fclose(tempFilePtr);
    // Update the original file with temp file
    remove("user.txt");
    rename("temp.txt", "user.txt");
    if (userFound) printf("Success: User ID %d updated with name '%s' and age %d.\n", searchId, newName, newAge);
    else printf("Error: User with ID %d not found.\n", searchId);
}

void deleteUser(){
    FILE *filePtr, *tempFilePtr;
    struct User currentUser;
    int searchId;
    bool userFound = false;
    printf("Enter Id to delete the user : ");
    if (scanf("%d", &searchId) != 1) {
        printf("Error: Invalid input. ID must be a number.\n");
        while (getchar() != '\n'); 
        return;
    }
    filePtr = fopen("user.txt", "r"); // Original file
    if (filePtr == NULL) {
        printf("No user file found.\n");
        return;
    }
    tempFilePtr = fopen("temp.txt", "w");  // temp file to store the data of remaining users
    if (tempFilePtr == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(filePtr);
        return;
    }
    // Copy all users except the one to be deleted
    while (fscanf(filePtr, "%d", &currentUser.uid) == 1) {
        fgets(currentUser.name, sizeof(currentUser.name), filePtr);
        fgets(currentUser.name, sizeof(currentUser.name), filePtr);
        currentUser.name[strcspn(currentUser.name, "\n")] = '\0';
        fscanf(filePtr, "%d", &currentUser.age);
        if (currentUser.uid == searchId) {
            userFound = true;
            continue; // Skip the deleted user
        }
        fprintf(tempFilePtr, "%d\n%s\n%d\n", currentUser.uid, currentUser.name, currentUser.age);
    }
    fclose(filePtr);
    fclose(tempFilePtr);
    //update the original file with temp file
    remove("user.txt");
    rename("temp.txt","user.txt");
    if(userFound) printf("Success: User with ID %d deleted successfully.\n", searchId);
    else printf("Error: User with ID %d not found.\n", searchId);
}

int main(){
    int choice;
    while(true)
    {
        printf("\nEnter the number to select your choice :\n1. Create User \n2. Read Users \n3. Update User \n4. Delete User \n5. Exit\n");
        if (scanf("%d", &choice) != 1) { //choice should be int to prevent the garbage value 
            printf("Error: Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        switch (choice) {
            case 1:
                createUser();
                break;
            case 2:
                readUser();
                break;
            case 3:
                updateUser();
                break;
            case 4:
                deleteUser();
                break;
            case 5:
                printf("Exiting...\nTHANK YOU!\n");
                return 0;
            default:
                printf("Error: Invalid choice. Please enter a number between 1 and 5.\n");
        }
    }  
    return 0;
}