#include<stdio.h>
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
}
int main(){
    char exp[50];
    fgets(exp , sizeof(exp) , stdin); //String Input
    printf("Entered : %s",exp); 
    cal(exp);
}