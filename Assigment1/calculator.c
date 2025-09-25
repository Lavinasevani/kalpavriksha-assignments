#include<stdio.h>
#include <string.h> 
#include<ctype.h>
void cal(char inp[]){
    //White space handling
    int j =0 ;
    for(int i = 0 ; inp[i] != '\0' ; i++){
        if(inp[i] != ' '){
            inp[j] = inp[i];
            j++;
        }
    }
    inp[j] = '\0';
    printf("White Space Ignored : %s",inp);

    //Validity check 
    size_t len = strlen(inp);
    if(len > 0 && inp[len-1] == '\n') {
        inp[len-1] = '\0';
    }
    int valid = 1;
    for(int k = 0 ; inp[k] != '\0' ; k++){
        if(inp[k] != '+' && inp[k] != '-' && inp[k] != '*' && inp[k] != '/' && !isdigit(inp[k])){
            valid =0;
            break;
        }
    }
    if(valid == 0){
        printf("Invalid expression.\n");
        return;
    }
    //Expression Evalution
    printf("Expression evalution: ");
    int num = 0;
    int res =0 , lastTerm =0;
    char op = '+';
    for(int i =0; inp[i] != '\0' ;i++){
        char ch = inp[i];
        if(isdigit(ch)){
            num = num  * 10 +(ch-'0'); //Number 
        }
        //operation
        if(!isdigit(ch)||inp[i+1] == '\0'){
            if(op=='+'){
                res = res + lastTerm;
                lastTerm =num;
            }
            else if(op == '-'){
                res = res +lastTerm;
                lastTerm = -num;
            }
            else if(op == '*'){
                lastTerm = lastTerm * num;
            }
            else if(op == '/'){
                lastTerm = lastTerm /num;
            }
            op = ch;
            num = 0;
        }
    }
    res = res+ lastTerm;
    printf("%d",res);   
}
int main(){
    char exp[50];
    fgets(exp , sizeof(exp) , stdin); //String Input
    printf("Entered : %s",exp); 
    cal(exp);
}