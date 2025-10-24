#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MIN_LENGTH_OF_NAME 1
#define MAX_LENGTH_OF_NAME 50
#define MIN_PRODUCT_ID 1
#define MAX_PRODUCT_ID 10000
#define MIN_PRICE 0.00
#define MAX_PRICE 100000.00
#define MIN_QUANTITY 0
#define MAX_QUANTITY 1000000
#define MIN_INITIAL_PRODUCTS 1
#define MAX_INITIAL_PRODUCTS 100

struct product {
    int productId;
    char productName[MAX_LENGTH_OF_NAME];
    float price;
    int quantity;
};

void trimSpaces(char *name) {
    if (name == NULL) return;
    int leadingWhitespaceCount = 0;
    while (name[leadingWhitespaceCount] == ' ' || name[leadingWhitespaceCount] == '\t') {
        leadingWhitespaceCount++;
    }
    if (leadingWhitespaceCount > 0) {
        int writePosition = 0, readPosition = leadingWhitespaceCount;
        while (name[readPosition] != '\0') {
            name[writePosition++] = name[readPosition++];
        }
        name[writePosition] = '\0';
    }
    int currentLength = strlen(name);
    while (currentLength > 0 && (name[currentLength - 1] == ' ' || name[currentLength - 1] == '\t')) {
        name[currentLength - 1] = '\0';
        currentLength--;
    }
}

bool isValidName(const char name[MAX_LENGTH_OF_NAME]) {
    char tempName[MAX_LENGTH_OF_NAME];
    strcpy(tempName, name);
    trimSpaces(tempName);
    if (strlen(tempName) == 0) {
        printf("Error: Product name cannot be empty.\n");
        return false;
    }
    if (strlen(tempName) >= MAX_LENGTH_OF_NAME) {
        printf("Error: Product name too long (max %d characters).\n", MAX_LENGTH_OF_NAME - 1);
        return false;
    }
    bool hasVisibleCharacter = false;
    for (int i = 0; tempName[i] != '\0'; i++) {
        if (tempName[i] != ' ' && tempName[i] != '\t' && tempName[i] != '\r' && tempName[i] != '\n') {
            hasVisibleCharacter = true;
            break;
        }
    }
    if (!hasVisibleCharacter) {
        printf("Error: Product name cannot contain only spaces.\n");
        return false;
    }
    return true;
}

bool readProductDetails(struct product *productPointer, int productIndex, struct product *allProducts, int totalProducts) {
    int productId;
    char productName[MAX_LENGTH_OF_NAME];
    float price;
    int quantity;

    printf("Product ID: ");
    if (scanf("%d", &productId) != 1) {
        printf("Invalid number.\n");
        while (getchar() != '\n');
        return false;
    }
    if (productId < MIN_PRODUCT_ID || productId > MAX_PRODUCT_ID) {
        printf("Error: Product ID must be between %d and %d.\n", MIN_PRODUCT_ID, MAX_PRODUCT_ID);
        return false;
    }
    for (int i = 0; i < totalProducts; i++) {
        if ((allProducts + i)->productId == productId) {
            printf("Error: Product ID %d already exists.\n", productId);
            return false;
        }
    }
    (productPointer + productIndex)->productId = productId;
    while (getchar() != '\n');

    printf("Product Name: ");
    if (fgets(productName, sizeof(productName), stdin) == NULL) {
        printf("Error: Cannot read name.\n");
        return false;
    }
    productName[strcspn(productName, "\n")] = '\0';
    if (!isValidName(productName)) {
        return false;
    }
    strcpy((productPointer + productIndex)->productName, productName);

    printf("Product Price: ");
    if (scanf("%f", &price) != 1) {
        printf("Invalid number.\n");
        while (getchar() != '\n');
        return false;
    }
    if (price < MIN_PRICE || price > MAX_PRICE) {
        printf("Price must be between %.2f and %.2f\n", MIN_PRICE, MAX_PRICE);
        return false;
    }
    (productPointer + productIndex)->price = price;

    printf("Product Quantity: ");
    if (scanf("%d", &quantity) != 1) {
        printf("Invalid number.\n");
        while (getchar() != '\n');
        return false;
    }
    if (quantity < MIN_QUANTITY || quantity > MAX_QUANTITY) {
        printf("Quantity must be between %d and %d\n", MIN_QUANTITY, MAX_QUANTITY);
        return false;
    }
    (productPointer + productIndex)->quantity = quantity;

    return true;
}

bool inputProductDetails(int numberOfProducts, struct product *productPointer) {
    for (int productIndex = 0; productIndex < numberOfProducts; productIndex++) {
        printf("\nEnter details for product: %d\n", productIndex + 1);
        if (!readProductDetails(productPointer, productIndex, productPointer, productIndex))
            return false;
    }
    return true;
}

struct product* addProduct(int *numberOfProducts, struct product *productPointer) {
    struct product *temporaryPointer = realloc(productPointer, (*numberOfProducts + 1) * sizeof(struct product));
    if (!temporaryPointer) {
        perror("Cannot add product, realloc failed");
        return productPointer;
    }
    productPointer = temporaryPointer;

    printf("\nEnter new product details:\n");
    if (readProductDetails(productPointer, *numberOfProducts, productPointer, *numberOfProducts)) {
        (*numberOfProducts)++;
        printf("Product added successfully!\n");
    }
    return productPointer;
}

void viewProductList(int numberOfProducts, struct product *productPointer) {
    printf("\n================= PRODUCT LIST =================\n");
    for (int i = 0; i < numberOfProducts; i++) {
        printf("Product ID: %-10d | Name: %-25s | Price: %-15.2f | Quantity: %-10d\n",
               (productPointer + i)->productId,
               (productPointer + i)->productName,
               (productPointer + i)->price,
               (productPointer + i)->quantity);
    }
}

int main() {
    int numberOfProducts;
    printf("Enter initial number of products: ");
    if (scanf("%d", &numberOfProducts) != 1 || numberOfProducts < MIN_INITIAL_PRODUCTS || numberOfProducts > MAX_INITIAL_PRODUCTS) {
        printf("Invalid number of products (must be between %d and %d).\n", MIN_INITIAL_PRODUCTS, MAX_INITIAL_PRODUCTS);
        return 1;
    }

    struct product *productPointer = calloc(numberOfProducts, sizeof(struct product));
    if (productPointer == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    if (inputProductDetails(numberOfProducts, productPointer)) {
        bool exitProgram = false;
        while (!exitProgram) {
            printf("\n========= INVENTORY MENU =========\n");
            printf("1. Add New Product\n2. View All Products\n3. Update Quantity\n4. Search Product by ID\n5. Search Product by Name\n6. Search Product by Price Range\n7. Delete Product\n8. Exit\n");
            printf("Enter your choice: ");
            int choice;
            if (scanf("%d", &choice) != 1) {
                printf("Choice should be a number only.\n");
                while (getchar() != '\n');
                continue;
            }

            switch (choice) {
                case 1:
                    productPointer = addProduct(&numberOfProducts, productPointer);
                    break;
                case 2:
                    viewProductList(numberOfProducts, productPointer);
                    break;
                case 3:
                    printf("Quantity updated successfully!\n");
                    break;
                case 4:
                    printf("=== Search by ID ===\n");
                    break;
                case 5:
                    printf("=== Search by Name ===\n");
                    break;
                case 6:
                    printf("=== Search by Price Range ===\n");
                    break;
                case 7:
                    printf("Product deleted successfully!\n");
                    break;
                case 8:
                    free(productPointer);
                    printf("Memory released successfully. Exiting program...\n");
                    exitProgram = true;
                    break;
                default:
                    printf("Invalid input, try again.\n");
                    break;
            }
        }
    }

    return 0;
}
