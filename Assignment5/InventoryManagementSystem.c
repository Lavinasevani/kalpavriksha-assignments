#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_LENGTH_OF_NAME 100

struct product{
    int productId;
    char productName[MAX_LENGTH_OF_NAME];
    float price;
    int quantity;
};

bool isValidName(const char name[MAX_LENGTH_OF_NAME]){
    return strlen(name) > 0; 
}

bool inputProductDetails(int numberOfProducts, struct product *productPointer){

    int productId;
    char productName[MAX_LENGTH_OF_NAME];
    float price;
    int quantity;

    for(int productIndex = 0; productIndex < numberOfProducts; productIndex++) {

            printf("Enter details for product: %d ", productIndex + 1);

            printf("\nProduct ID: ");
            if(scanf("%d", &productId) != 1){
                printf("Invalid number.\n");
                return false;
            }
            (productPointer + productIndex) -> productId = productId;
            while (getchar() != '\n');

            printf("Product Name: ");
            if(fgets(productName, sizeof(productName) , stdin) == NULL) {
                printf("Error: Cannot read name.\n");

            }
            productName[strcspn(productName, "\n")] = '\0';
            if (!isValidName(productName)) {
                printf("Invalid product name.\n");
                return false;
            }
            strcpy((productPointer + productIndex)->productName, productName);
            printf("Product Price: ");
            if(scanf("%f", &price) != 1){
                printf("Invalid number.\n");
                return false;
            }
            (productPointer + productIndex) -> price = price;

            printf("Product Quantity: ");
            if(scanf("%d", &quantity) != 1){
                printf("Invalid number.\n");
                return false;
            }
            (productPointer + productIndex) -> quantity = quantity;
    }
    return true;
}

int main(){

    int numberOfProducts;
    printf("Enter initial number of products:");
    if(scanf("%d", &numberOfProducts) != 1 || numberOfProducts <= 0){
        printf("Invalid number.\n");
        return 1;
    }

    struct product *productPointer = calloc(numberOfProducts , sizeof(struct product) );
    if (productPointer == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }
    if (inputProductDetails(numberOfProducts, productPointer)){
        bool shouldExit = false;
        while(! shouldExit){
                printf("========= INVENTORY MENU =========\n1. Add New Product\n2. View All Products\n3. Update Quantity\n4. Search Product by ID\n5. Search Product by Name\n6. Search Product by Price Range\n7. Delete Product\n8. Exit\n");
                printf("Enter your choice: ");
                int choice;
                scanf("%d", &choice);
                switch (choice)
                {
                case 1:
                    printf("Product added successfully!\n");
                    break;
                case 2:
                    printf("===2===\n");
                    break;
                case 3:
                    printf("Quantity updated successfully!\n");
                    break;
                case 4:
                    printf("===4===\n");
                    break;
                case 5:
                    printf("===5===\n");
                    break;
                case 6:
                    printf("===6===\n");
                    break;
                case 7:
                    printf("Product deleted successfully! \n");
                    break;
                case 8:
                    free(productPointer);
                    printf("Memory released successfully. Exiting program...\n");
                    shouldExit = true;
                    break;
                default:
                    printf("Invalid input try again.\n");
                    break;
                }
        }
    }
    return 0; 
}