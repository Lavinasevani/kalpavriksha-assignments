#include<stdio.h>
int main(){
    char exp[50];
    fgets(exp , sizeof(exp) , stdin); //String Input
    printf("Entered : %s",exp); 

}