#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Players_data.h"

#define MIN_ID 1
#define MAX_ID 1000
#define MIN_PLAYERS 11
#define MAX_PLAYERS 50
#define MIN_NAME_LENGTH 1
#define MAX_NAME_LENGTH 50

enum Role {Batsman = 1, Bowler, All_rounder};
struct NodePlayer{
    Player data;
    struct NodePlayer *next;
};

struct NodePlayer *headPlayerLL = NULL;
struct NodePlayer *tailPlayerLL = NULL;
void initalizePlayers(){
    for(int index = 0; index < playerCount ; index++){
        struct NodePlayer *newNodePlayer = (struct NodePlayer*)malloc(sizeof(struct NodePlayer));
        newNodePlayer->data = players[index];
        newNodePlayer->next = NULL;
        if(headPlayerLL == NULL){
            headPlayerLL = newNodePlayer;
            tailPlayerLL = newNodePlayer;
        }else{
            tailPlayerLL->next = newNodePlayer;
            tailPlayerLL = newNodePlayer;
        }
    }
}

void displayList(struct NodePlayer *head) {
    struct NodePlayer *temp = head;
    while (temp != NULL) {
        Player p = temp->data;
        printf("%d %s (%s) - %s\n", p.id, p.name, p.team, p.role);
        printf("   Runs: %d | Avg: %.1f | SR: %.1f | Wkts: %d | Eco: %.1f\n\n",
               p.totalRuns, p.battingAverage, p.strikeRate, p.wickets, p.economyRate);
        temp = temp->next;
    }
}

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

void addPlayerToTeam(){
    printf("Choice 1 -> Add Player to Team");
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
    case 1:{
        addPlayerToTeam();
        
        break;
    } 
    case 2:
        printf("Choice 2 -> Display All Players of a Specific Team");
        break;
    case 3:
        printf("Choice 3 -> Display Teams by Average Batting StrikeRate");
        break;
    case 4:
        printf("Choice4 -> Display Top K Players of a Specific Team of specific role");
        break;
    case 5:
        printf("Choice5 -> Display All Players Across All Teams of specific role");
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
    initalizePlayers();
    displayList(headPlayerLL);
    printMenu();
    printf("%d", teamCount);
    printf("%d", playerCount);
    while (true)
    {
        getChoice();
    }
    return 0;
}