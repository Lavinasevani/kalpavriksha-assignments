#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// Product name constraints
#define MIN_LENGTH_OF_NAME 1
#define MAX_LENGTH_OF_NAME 50

// Product ID constraints
#define MIN_PRODUCT_ID 1
#define MAX_PRODUCT_ID 10000

// Price constraints
#define MIN_PRICE 0.00
#define MAX_PRICE 100000.00

// Quantity constraints
#define MIN_QUANTITY 0
#define MAX_QUANTITY 1000000

// Initial inventory size constraints
#define MIN_INITIAL_PRODUCTS 1
#define MAX_INITIAL_PRODUCTS 100

struct product {
    int productId;
    char productName[MAX_LENGTH_OF_NAME];
    float price;
    int quantity;
};

void removeExtraSpaces(char *name) {
    if (name == NULL) return;

    // Step 1: Remove leading whitespace (spaces and tabs)
    int leadingWhitespaceCount = 0;
    while (name[leadingWhitespaceCount] == ' ' || name[leadingWhitespaceCount] == '\t') {
        leadingWhitespaceCount++;
    }
    if (leadingWhitespaceCount > 0) {
        int writePosition = 0, readPosition = leadingWhitespaceCount;
        while (name[readPosition] != '\0') {
            name[writePosition++] = name[readPosition++]; // Shift string left if leading whitespace found
        }
        name[writePosition] = '\0';
    }

    // Step 2: Remove trailing whitespace
    int currentLength = strlen(name);
    while (currentLength > 0 && (name[currentLength - 1] == ' ' || name[currentLength - 1] == '\t')) {
        name[currentLength - 1] = '\0';
        currentLength--;
    }

    // Step 3: Replace multiple consecutive spaces with single space
    int readPosition = 0, writePosition = 0;
    bool previousWasSpace = false;

    while (name[readPosition] != '\0') {
        if (name[readPosition] == ' ' || name[readPosition] == '\t') {
            if (!previousWasSpace) {
                name[writePosition++] = ' ';  // Keep only one space
                previousWasSpace = true;
            }
        } else {
            name[writePosition++] = name[readPosition];
            previousWasSpace = false;
        }
        readPosition++;
    }

    name[writePosition] = '\0';
}

bool isValidName(char name[MAX_LENGTH_OF_NAME]) {
    //Name can not be NULL
    if (name == NULL) {
        printf("Error: Product name is NULL.\n");
        return false;
    }

    removeExtraSpaces(name); // Clean up spaces

    //Cannot be empty after removing extra spaces
    if (strlen(name) == 0) {
        printf("Error: Product name cannot be empty.\n");
        return false;
    }
    // Must be less than MAX_LENGTH_OF_NAME
    if (strlen(name) >= MAX_LENGTH_OF_NAME) {
        printf("Error: Product name too long (max %d characters).\n", MAX_LENGTH_OF_NAME - 1);
        return false;
    }

    bool hasNonSpaceChar = false;
    for (int indexOfName = 0; name[indexOfName] != '\0'; indexOfName++) {
        //Can only contain alphanumeric characters and spaces
        if (!isalnum((unsigned char)name[indexOfName]) && name[indexOfName] != ' ') {
            printf("Error: Product name contains invalid character '%c'. Only letters, digits, and spaces are allowed.\n", name[indexOfName]);
            return false;
        }
      
       // Check if there's at least one non-whitespace character
        if (name[indexOfName] != ' ' && name[indexOfName] != '\t' && name[indexOfName] != '\r' && name[indexOfName] != '\n') {
            hasNonSpaceChar = true;
            break;
        }
    }
     //Cannot contain only whitespace
    if (!hasNonSpaceChar) {
        printf("Error: Product name cannot contain only spaces.\n");
        return false;
    }
    return true;
}

bool collectProductInfo(struct product *productPointer, int productIndex, struct product *allProducts, int totalProducts) {
    int productId;
    char productName[MAX_LENGTH_OF_NAME];
    float price;
    int quantity;
    // Input and validate ProductID
    printf("Product ID: ");
    if (scanf("%d", &productId) != 1) {
        printf("Invalid number.\n");
        while (getchar() != '\n'); // Clear input buffer
        return false;
    }
    if (productId < MIN_PRODUCT_ID || productId > MAX_PRODUCT_ID) {
        printf("Error: Product ID must be between %d and %d.\n", MIN_PRODUCT_ID, MAX_PRODUCT_ID);
        return false;
    }
    // Check for duplicate ProductID
    for (int i = 0; i < totalProducts; i++) {
        if ((allProducts + i)->productId == productId) {
            printf("Error: Product ID %d already exists.\n", productId);
            return false;
        }
    }
    (productPointer + productIndex)->productId = productId;
    while (getchar() != '\n');
    // Input and validate Product Name
    printf("Product Name: ");
    if (fgets(productName, sizeof(productName), stdin) == NULL) {
        printf("Error: Cannot read name.\n");
        return false;
    }
    productName[strcspn(productName, "\n")] = '\0'; // Remove newline at the end added by fgets
    if (!isValidName(productName)) {
        return false;
    }
    strcpy((productPointer + productIndex)->productName, productName);
    // Input and validate Product price
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
    while (getchar() != '\n');
    // Input and validate Product Quantity
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
    while (getchar() != '\n');
    return true;
}
// Collects initial product details for all products
bool inputProductDetails(int numberOfProducts, struct product *productPointer) {
    for (int productIndex = 0; productIndex < numberOfProducts; productIndex++) {
        printf("Enter details for product: %d\n", productIndex + 1);
        if (!collectProductInfo(productPointer, productIndex, productPointer, numberOfProducts))
            return false;
    }
    return true;
}
//Adds a new product to the inventory
struct product* addProduct(int *numberOfProducts, struct product *productPointer) {
    // Expand memory to accommodate one more product
    struct product *temporaryPointer = realloc(productPointer, (*numberOfProducts + 1) * sizeof(struct product));
    if (!temporaryPointer) {
        perror("Cannot add product, realloc failed");
        return productPointer; // Return original pointer if realloc fails
    }
    productPointer = temporaryPointer;

    // Collect new product information
    printf("Enter new product details:\n");
    if (collectProductInfo(productPointer, *numberOfProducts, productPointer, *numberOfProducts)) {
        (*numberOfProducts)++;
        printf("Product added successfully!\n");
    }
    return productPointer;
}
// Displays all products in the inventory
void viewProductList(int numberOfProducts, struct product *productPointer) {
    // Handle empty inventory case
    if (numberOfProducts == 0) {
        printf("No products in inventory.\n");
        return;
    }
    printf("\n================= PRODUCT LIST =================\n");
    for (int productIndex = 0; productIndex < numberOfProducts; productIndex++) {
        printf("Product ID: %-10d | Name: %-25s | Price: %-15.2f | Quantity: %-10d\n",
               (productPointer + productIndex)->productId,
               (productPointer + productIndex)->productName,
               (productPointer + productIndex)->price,
               (productPointer + productIndex)->quantity);
    }
    return;
}
//########### SEARCH OPERATIONS ##########


void searchProductByID(struct product *productPointer, int totalProducts){
    int searchID;
    printf("Enter Product ID to search: ");
    if(scanf("%d", &searchID) != 1){
        printf("Invalid number.\n");
        while (getchar() != '\n');
        return;
    }
    bool isIDFound = false;

    // Linear search through all products 
    for(int productIndex = 0; productIndex < totalProducts; productIndex++) {
        if(searchID == (productPointer + productIndex) -> productId) {
            isIDFound = true;
            printf("\nProduct Found: Product ID: %d | Name: %s | Price: %.2f | Quantity: %d \n", 
            (productPointer + productIndex)->productId,
            (productPointer + productIndex)->productName,
            (productPointer + productIndex)->price,
            (productPointer + productIndex)->quantity);
            break; // Product ID is unique, stop after finding
        }
    }
    if(! isIDFound){
        printf("Product not found with the ID: %d \n", searchID);
        return;
    }
    return;
}
//searchName is a substring of productName (case-insensitive)
bool matchName(const char *productName, const char *searchName) {

    int indexProductName = 0; 
    int indexSearchName = 0; 

    while (productName[indexProductName] != '\0' && searchName[indexSearchName] != '\0') {
        if (tolower((unsigned char)productName[indexProductName]) == tolower((unsigned char)searchName[indexSearchName])) {
            indexSearchName++;
        } else {
            indexSearchName = 0; 
        }
        indexProductName++;
    }

    return (searchName[indexSearchName] == '\0'); 
}

void searchProductByName(struct product *productPointer, int totalProducts){

    char searchName[MAX_LENGTH_OF_NAME];

    printf("Enter name to search (partial allowed): ");

    if (fgets(searchName, sizeof(searchName), stdin) == NULL) {
        printf("Error: Cannot read name.\n");
        return;
    }
    searchName[strcspn(searchName, "\n")] = '\0';
    // Validate search input
    if (!isValidName(searchName)) {
        printf("Invalid name.\n");
        return;
    }

    // Search through all products
    bool isNameFound = false;
    for(int productIndex = 0; productIndex < totalProducts; productIndex++) {
        if(matchName((productPointer + productIndex)->productName , searchName)){
            if (!isNameFound) {
                printf("Products Found.\n");
                isNameFound = true;
            }
            printf(" Product ID: %d | Name: %s | Price: %.2f | Quantity: %d \n", 
            (productPointer + productIndex)->productId,
            (productPointer + productIndex)->productName,
            (productPointer + productIndex)->price,
            (productPointer + productIndex)->quantity);
        }
    }
    if(! isNameFound){
        printf("Product not found with the name: %s \n", searchName);
        return;
    }
}

void searchProductByPriceRange(struct product *productPointer, int totalProducts){
    float minPrice, maxPrice;
    // Input minimum price
    printf("Enter minimum price: ");
    if(scanf("%f", &minPrice) != 1){
        printf("Invalid number.\n");
        while (getchar() != '\n');
        return;
    }
    if (minPrice < MIN_PRICE) {
        printf("Minimum price range must be greater than or equal %.2f \n", MIN_PRICE);
        return;
    }
    // Input maximum price
    printf("Enter maximum price: ");
    if(scanf("%f", &maxPrice) != 1){
        printf("Invalid number.\n");
        while (getchar() != '\n');
        return;
    }
    if (maxPrice > MAX_PRICE) {
        printf("Maximum price range must be smaller than or equal %.2f \n", MAX_PRICE);
        return;
    }
    // Validate price range
    if (maxPrice < minPrice) {
        printf("Error: Maximum price range must be greater than or equal to minimum price range.\n");
        return;
    }

    // Search for products in price range
    bool isPriceFound = false;
    for(int productIndex = 0; productIndex < totalProducts; productIndex++) {
        if((productPointer + productIndex) -> price >= minPrice && (productPointer + productIndex) -> price <= maxPrice) {
            if (!isPriceFound) {
                printf("Products in price range:\n");
                isPriceFound = true;
            }
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d \n", 
            (productPointer + productIndex)->productId,
            (productPointer + productIndex)->productName,
            (productPointer + productIndex)->price,
            (productPointer + productIndex)->quantity);
        }
    }
    if(! isPriceFound){
        printf("Product not found with the range %.2f - %.2f\n", minPrice, maxPrice);
        return;
    }
    return;
}

void updateQuantitybyID(struct product *productPointer, int totalProducts){
    // Get Product ID to update
    int searchId;
    printf("Enter Product ID to update quantity: ");
    if(scanf("%d", &searchId) != 1){
        printf("Invalid number.\n");
        while (getchar() != '\n');
        return;
    }
    // Get new Quantity value
    int newQuantity;
    printf("Enter new Quantity: ");
    if (scanf("%d", &newQuantity) != 1) {
        printf("Invalid number.\n");
        while (getchar() != '\n');
        return;
    }
    // Validate new quantity
    if (newQuantity < MIN_QUANTITY || newQuantity > MAX_QUANTITY) {
        printf("Quantity must be between %d and %d\n", MIN_QUANTITY, MAX_QUANTITY);
        return ;
    }
    
    bool isIDFound = false;
    for(int productIndex = 0; productIndex < totalProducts; productIndex++) {
        if(searchId == (productPointer + productIndex) -> productId) {
            isIDFound = true;
            (productPointer + productIndex)->quantity = newQuantity; // replace the current quantity with the new one
            printf("Quantity updated successfully!\n");
            break;
        }
    }
    if(! isIDFound){
        printf("Product not found with the ID: %d\n", searchId);
        return;
    }
    return;

}

void deleteProductbyID(struct product **productPointer, int *totalProducts){
    // Get Product ID to delete
    int searchID;
    printf("Enter Product ID to delete: ");
    if(scanf("%d", &searchID) != 1){
        printf("Invalid number.\n");
        while (getchar() != '\n');
        return;
    }
    // Search to get index of product to delete
    bool isIDFound = false;
    int indexToDelete = -1;
    for (int indexProduct = 0; indexProduct < *totalProducts; indexProduct++) {
        if ((*productPointer + indexProduct)->productId == searchID) {
            isIDFound = true;
            indexToDelete = indexProduct;
            break;
        }
    }
    if (!isIDFound) {
        printf("Product not found with ID: %d\n", searchID);
        return;
    }
    
    // Shift elements to remove the product
    for (int indexProduct = indexToDelete; indexProduct < *totalProducts - 1; indexProduct++) {
        (*productPointer)[indexProduct] = (*productPointer)[indexProduct + 1];
    }

    (*totalProducts)--;
    
    // Handle memory reallocation
    if (*totalProducts == 0) {
        free(*productPointer); // If all products deleted, free memory completely
        *productPointer = NULL;
    } else {
        // decreasing memory to fit remaining products
        struct product *temp = realloc(*productPointer, (*totalProducts) * sizeof(struct product));
        if (temp != NULL) {
            *productPointer = temp;
        }
        // If realloc fails, original pointer is still valid (not shrunk but functional)
    }
    
    printf("Product deleted successfully!\n");
}

int main() {
    int numberOfProducts;
    printf("Enter initial number of products: ");
    
    if (scanf("%d", &numberOfProducts) != 1 || numberOfProducts < MIN_INITIAL_PRODUCTS || numberOfProducts > MAX_INITIAL_PRODUCTS) {
        printf("Invalid number of products (must be between %d and %d).\n", MIN_INITIAL_PRODUCTS, MAX_INITIAL_PRODUCTS);
        return 1;
    }
    while (getchar() != '\n');
    // Dynamically allocate memory for products
    struct product *productPointer = calloc(numberOfProducts, sizeof(struct product));
    if (productPointer == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }
    // Collect initial product details
    if (inputProductDetails(numberOfProducts, productPointer)) {
        // Main menu loop
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
                    updateQuantitybyID(productPointer, numberOfProducts);
                    while (getchar() != '\n'); 
                    break;
                case 4:
                    searchProductByID(productPointer, numberOfProducts);
                    while (getchar() != '\n'); 
                    break;
                case 5:
                    while (getchar() != '\n');  
                    searchProductByName(productPointer, numberOfProducts);
                    break;
                case 6:
                    searchProductByPriceRange(productPointer, numberOfProducts);
                    while (getchar() != '\n'); 
                    break;
                case 7:
                    deleteProductbyID(&productPointer, &numberOfProducts);
                    while (getchar() != '\n'); 
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
    }else{ //Handle initialization failure
        free(productPointer);  // Releasing  allocated memory
        printf("Failed to initialize products. Exiting.\n");
        return 1;
    }
    return 0;
}