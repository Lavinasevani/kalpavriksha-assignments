#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Stack for numbers
int numStack[100];
int numTop = -1;
void pushNum(int val) {
    numStack[++numTop] = val;
}
int popNum() {
    return numStack[numTop--];
}
int peekNum() {
    return numStack[numTop];
}

// Stack for operators
char opStack[100];
int opTop = -1;
void pushOp(char op) {
    opStack[++opTop] = op;
}
char popOp() {
    return opStack[opTop--];
}
char peekOp() {
    return opStack[opTop];
}

// Function to get operator precedence
int precedence(char op) {
    if (op == '*' || op == '/') return 2; //higher
    if (op == '+' || op == '-') return 1; //lower
    return 0;
}

// Function to perform operation
int applyOp(int a, int b, char op, int *error) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) {
                *error = 1;
                return 0;
            }
            return a / b;
    }
    return 0;
}

void evaluateExpression(char input[]) {       // Function to evaluate arithmetic expression
    //White space handling
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (!isspace((unsigned char)input[i])) {  // handles spaces, tabs, newlines
            input[j] = input[i];
            j++;
        }
    }
    input[j] = '\0';

    //Validity check 
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {   // Remove the newline character added by fgets at the end of input
        input[len - 1] = '\0';
    }

    // Check empty input
    if (strlen(input) == 0) {
        printf("Invalid expression.\n");
        return;
    }

    // Check for invalid characters
    for (int k = 0; input[k] != '\0'; k++) {
        if (input[k] != '+' && input[k] != '-' && input[k] != '*' && input[k] != '/' && !isdigit(input[k])) {
            printf("Invalid expression.\n");
            return;
        }
    }

    // Check logical operator placement first/last 
    if (!isdigit(input[0]) || !isdigit(input[strlen(input) - 1])) {
        printf("Invalid expression.\n");
        return;
    }
    // Check consecutive operators
    for (int k = 1; input[k] != '\0'; k++) {
        if (!isdigit(input[k]) && !isdigit(input[k - 1])) {
            printf("Invalid expression.\n");
            return;
        }
    }

    //Expression Evaluation
    numTop = -1; //reset stack 
    opTop = -1;
    
    int error = 0;
    int currentNumber = 0;
    
    // Stack-based expression evaluation with DMAS
    for (int i = 0; input[i] != '\0'; i++) {
        if (isdigit(input[i])) { 
            currentNumber = currentNumber * 10 + (input[i] - '0'); //Build number
            if (input[i + 1] == '\0' || !isdigit(input[i + 1])) { // If next char is operator or end, push the number
                pushNum(currentNumber);
                currentNumber = 0;
            }
        } else {
            char currentOp = input[i];
            // Process operators with higher or equal precedence
            while (opTop >= 0 && precedence(peekOp()) >= precedence(currentOp)) {
                char op = popOp();
                int b = popNum();
                int a = popNum();
                int result = applyOp(a, b, op, &error);  
                if (error) {
                    printf("Error: Division by zero.\n");
                    return;
                }
                pushNum(result);
            }
            pushOp(currentOp);
        }
    }
    // Process remaining operators in stack
    while (opTop >= 0) {
        char op = popOp();
        int b = popNum();
        int a = popNum();
        int result = applyOp(a, b, op, &error);  
        if (error) {
            printf("Error: Division by zero.\n");
            return;
        }
        pushNum(result);
    }
    printf("%d\n", peekNum()); //result
}

int main() {
    char expression[50];
    printf("Enter an expression: ");
    fgets(expression, sizeof(expression), stdin); //String Input
    evaluateExpression(expression); //Function Calling
    return 0;
}
