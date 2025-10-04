#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

    // Check logical operator placement (first/last and consecutive operators)
    if (!isdigit(input[0]) || !isdigit(input[strlen(input) - 1])) {
        printf("Invalid expression.\n");
        return;
    }
    for (int k = 1; input[k] != '\0'; k++) {
        if (!isdigit(input[k]) && !isdigit(input[k - 1])) {
            printf("Invalid expression.\n");
            return;
        }
    }

    //Expression Evaluation
    int currentNumber = 0;
    int result = 0, lastValue = 0;
    char currentOperator = '+';

    for (int i = 0; input[i] != '\0'; i++) {
        char ch = input[i];

        if (isdigit(ch)) {
            currentNumber = currentNumber * 10 + (ch - '0'); // build multi-digit number
        }

        // operation when operator or at the end 
        if (!isdigit(ch) || input[i + 1] == '\0') {
            if (currentOperator == '+') {
                result = result + lastValue;
                lastValue = currentNumber;
            } else if (currentOperator == '-') {
                result = result + lastValue;
                lastValue = -currentNumber;
            } else if (currentOperator == '*') {
                lastValue = lastValue * currentNumber;
            } else if (currentOperator == '/') {
                if (currentNumber == 0) {
                    printf("Error: Division by zero.\n");
                    return;
                }
                lastValue = lastValue / currentNumber;
            }

            currentOperator = ch;
            currentNumber = 0;
        }
    }

    result = result + lastValue;
    printf(" %d \n", result);
}

int main() {
    char expression[50];
    printf("Enter an expression: ");
    fgets(expression, sizeof(expression), stdin); //String Input
    evaluateExpression(expression); //Function Calling
    return 0;
}
