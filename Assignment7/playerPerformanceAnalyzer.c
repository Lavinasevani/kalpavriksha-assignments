#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void printMenu(){
    printf("==================================================================================\n");
    printf("ICC ODI Player Performance Analyzer\n");
    printf("==================================================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific \n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    printf("==================================================================================");
    return;
}
void getChoice(){
    int choice;
    printf("\nEnter your choice: ");
    if(scanf("%d", &choice) != 1){
        printf("Invalid Number.");
        return;
    }
    switch (choice)
    {
    case 1:
        printf("Choice 1 -> AddPlayertoTeam");
        break;
    case 2:
        printf("Choice 2 -> Display AllPlayers of a Specific Team");
        break;
    case 3:
        printf("Choice 3 -> Display TeamsbyAverageBatting StrikeRate");
        break;
    case 4:
        printf("Choice4 -> DisplayTopKPlayersofaSpecificTeamofspecificrole");
        break;
    case 5:
        printf("Choice5 -> DisplayAllPlayersAcrossAllTeamsofspecificrole");
        break;
    case 6:
        printf("Exiting...");
        exit(0);
        break;
    default:
        printf("Choice a number between 1 to 6.");
        break;
    }
}
int main(){
    printMenu();
    getChoice();
    while (true)
    {
        getChoice();
    }
    return 0;
}