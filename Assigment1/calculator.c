#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_STACK_SIZE 100

// ---------- Stack Utilities ----------
int isStackFull(int top) {
    return top >= MAX_STACK_SIZE - 1;
}

int isStackEmpty(int top) {
    return top < 0;
}

// ---------- Number Stack ----------
int numberStack[MAX_STACK_SIZE];
int numberTop = -1;

void pushNumber(int value) {
    if (isStackFull(numberTop)) {
        printf("Error: Number stack overflow.\n");
        return;
    }
    numberStack[++numberTop] = value;
}

int popNumber() {
    if (isStackEmpty(numberTop)) {
        printf("Error: Number stack underflow.\n");
        return 0;
    }
    return numberStack[numberTop--];
}

int peekNumber() {
    return numberStack[numberTop];
}

// ---------- Operator Stack ----------
char operatorStack[MAX_STACK_SIZE];
int operatorTop = -1;

void pushOperator(char operator) {
    if (isStackFull(operatorTop)) {
        printf("Error: Operator stack overflow.\n");
        return;
    }
    operatorStack[++operatorTop] = operator;
}

char popOperator() {
    if (isStackEmpty(operatorTop)) {
        printf("Error: Operator stack underflow.\n");
        return 0;
    }
    return operatorStack[operatorTop--];
}

char peekOperator() {
    return operatorStack[operatorTop];
}

// ---------- Helper Functions ----------

int getOperatorPrecedence(char operator) {
    if (operator == '*' || operator == '/') return 2; //higher
    if (operator == '+' || operator == '-') return 1; //lower
    return 0;
}

// Function to perform operation
int applyOperation(int operandOne, int operandTwo, char operator, int *error) {
    switch(operator) {
        case '+': return operandOne + operandTwo;
        case '-': return operandOne - operandTwo;
        case '*': return operandOne * operandTwo;
        case '/':
            if (operandTwo == 0) {
                *error = 1;
                return 0;
            }
            return operandOne / operandTwo;
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
    numberTop = -1; //reset stack 
    operatorTop = -1;
    
    int error = 0;
    int currentNumber = 0;
    
    // Stack-based expression evaluation with DMAS
    for (int i = 0; input[i] != '\0'; i++) {
        if (isdigit(input[i])) { 
            currentNumber = currentNumber * 10 + (input[i] - '0'); //Build number
            if (input[i + 1] == '\0' || !isdigit(input[i + 1])) { // If next char is operator or end, push the number
                pushNumber(currentNumber);
                currentNumber = 0;
            }
        } else {
            char currentOperator = input[i];
            // Process operators with higher or equal precedence
            while (operatorTop >= 0 && getOperatorPrecedence(peekOperator()) >= getOperatorPrecedence(currentOperator)) {
                char operator = popOperator();
                int operandTwo = popNumber();
                int operandOne = popNumber();
                int result = applyOperation(operandOne, operandTwo, operator, &error);  
                if (error) {
                    printf("Error: Division by zero.\n");
                    return;
                }
                pushNumber(result);
            }
            pushOperator(currentOperator);
        }
    }
    
    // Process remaining operators in stack
    while (operatorTop >= 0) {
        char operator = popOperator();
        int operandTwo = popNumber();
        int operandOne = popNumber();
        int result = applyOperation(operandOne, operandTwo, operator, &error);  
        if (error) {
            printf("Error: Division by zero.\n");
            return;
        }
        pushNumber(result);
    }
    
    printf("%d\n", peekNumber()); //result
}

int main() {
    char expression[50];
    printf("Enter an expression: ");
    fgets(expression, sizeof(expression), stdin); //String Input
    evaluateExpression(expression); //Function Calling
    return 0;
}