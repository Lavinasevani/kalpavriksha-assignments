#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Players_data.h"

#define MIN_ID 1
#define MAX_ID 1500
#define MIN_PLAYERS 11
#define MAX_PLAYERS 50
#define MIN_NAME_LENGTH 1
#define MAX_NAME_LENGTH 50

const char *roles[] = {"Batsman", "Bowler", "All-rounder"};
int numberOfRoles = sizeof(roles) / sizeof(roles[0]);
enum Role { BATSMAN = 1, BOWLER = 2, ALL_ROUNDER = 3 };

struct NodePlayer{

    int id;
    char* name;
    int teamID;
    int roleID;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float PerformanceIndex;
    struct NodePlayer *next;

};

struct Team{

    int teamId;
    char *name;
    int totalPlayers;
    float avgBattingStrikerate;
    struct Team *next;

};

struct Team *teamHead = NULL;
struct Team *teamTail = NULL;
void initializeTeam(){
    for(int i = 0; i < teamCount; i++){
        struct Team *newTeam = (struct Team*)malloc(sizeof(struct Team));
        if(newTeam == NULL){
            printf("Memory allocation Failed");
            return;
        }
        newTeam->teamId = i+1;
        newTeam->name = malloc(strlen(teams[i])+ 1);
        if(newTeam->name == NULL){
            printf("Memory allocation Failed");
            return;
        }
        strcpy(newTeam->name, teams[i]);
        newTeam->totalPlayers = 0;
        newTeam->avgBattingStrikerate = 0.0;
        newTeam->next = NULL;
        if(teamHead == NULL){
            teamHead = newTeam;
            teamTail = newTeam;
        }else{
            teamTail->next = newTeam;
            teamTail = newTeam;
        }    
    }
    return;
}

int getTeamId(const char *teamName){
    for(int i =0 ; i < teamCount ; i++){
        if(strcmp(teamName , teams[i]) == 0){
            return i+1;
        }
    }
    return -1;
}

int getRoleId(const char *roleName){
    for(int i =0 ; i < numberOfRoles ; i++){
        if(strcmp(roleName , roles[i]) == 0){
            return i+1;
        }
    }
    return -1;
}

struct NodePlayer *headPlayerLL = NULL;
struct NodePlayer *tailPlayerLL = NULL;
void calAvgSR(){
    struct Team *team = teamHead;
    while(team != NULL){
        struct NodePlayer *playerTemp = headPlayerLL;
        int batsmanCount = 0;
        float sumSR = 0.0;
        while (playerTemp != NULL)
        {   
            if((team->teamId == playerTemp->teamID) && (playerTemp->roleID == BATSMAN || playerTemp->roleID == ALL_ROUNDER)){
                sumSR += playerTemp->strikeRate;
                batsmanCount++;
            }
            playerTemp = playerTemp->next;
        }
        if(batsmanCount > 0){
            team->avgBattingStrikerate = sumSR / batsmanCount;
        }
        else{
            team->avgBattingStrikerate = 0;
        }
        team = team->next;
    }
    return;
}

void initalizePlayers(){
    for(int index = 0; index < playerCount ; index++){

        struct NodePlayer *newNodePlayer = (struct NodePlayer*)malloc(sizeof(struct NodePlayer));
        if(newNodePlayer== NULL){
            printf("Memory allocation Failed");
            return;
        }
        newNodePlayer->id= players[index].id;
        newNodePlayer->name = malloc(strlen(players[index].name) + 1);
        if(newNodePlayer->name == NULL){
            printf("Memory allocation Failed");
            return;
        }
        strcpy(newNodePlayer->name, players[index].name);
        newNodePlayer->teamID = getTeamId(players[index].team);
        newNodePlayer->roleID = getRoleId(players[index].role);
        newNodePlayer->totalRuns = players[index].totalRuns;
        newNodePlayer->battingAverage = players[index].battingAverage;
        newNodePlayer->strikeRate = players[index].strikeRate;
        newNodePlayer->wickets = players[index].wickets;
        newNodePlayer->economyRate = players[index].economyRate;
        if(newNodePlayer->roleID  == BATSMAN){ 
            newNodePlayer->PerformanceIndex = (newNodePlayer->battingAverage * newNodePlayer->strikeRate) /100; //(BattingAverage × StrikeRate) / 100
        }else if(newNodePlayer->roleID  == BOWLER){ //Bowler
            newNodePlayer->PerformanceIndex  = (newNodePlayer->wickets * 2) + (100 - newNodePlayer->economyRate);// (Wickets × 2) + (100 − EconomyRate)
        }else if(newNodePlayer->roleID  == ALL_ROUNDER){ // All-rounder
            newNodePlayer->PerformanceIndex = ((newNodePlayer->battingAverage * newNodePlayer->strikeRate ) / 100)+(newNodePlayer->wickets * 2);//[(BattingAverage × StrikeRate) / 100] + (Wickets × 2)
        }
        newNodePlayer->next = NULL;
        if(headPlayerLL == NULL){
            headPlayerLL = newNodePlayer;
            tailPlayerLL = newNodePlayer;
        }else{
            tailPlayerLL->next = newNodePlayer;
            tailPlayerLL = newNodePlayer;
        }

        struct Team *updateTeam = teamHead;
        while (updateTeam != NULL)
        {
            if(updateTeam->teamId == newNodePlayer->teamID){
                updateTeam->totalPlayers = updateTeam->totalPlayers + 1;
                break;
            }
            updateTeam = updateTeam->next;
        }  
    }
    calAvgSR();
    return;
}

void addPlayerToTeam(){
    int teamId;
    printf("Choice 1 -> Add Player to Team\n");
    printf("Enter Team ID to add player: ");
    if(scanf("%d", &teamId) != 1){
        printf("Id should be the number only.");
        while(getchar() != '\n');
        return;
    }
    if(teamId < 1 || teamId > 10){
        printf("Invalid Team Id. It should be between 1 to 10 only!");
        return;
    }
    int teamPlayerCount = 0;
    struct NodePlayer *temp = headPlayerLL;
    while (temp != NULL)
    {
        if(temp->teamID == teamId){
            teamPlayerCount++;
        }
        temp = temp->next;
    }
    if(teamPlayerCount >= MAX_PLAYERS){
        printf("The team %s is Full can't add more player into this them.",  teams[teamId -1]);
        return;
    }
    int playerId, roleId, totalRun, wicket;
    float battingAvg, strikeRate, economyRate;
    char playerName[MAX_NAME_LENGTH];
    printf("Enter Player Details:\n");
    printf("Player ID: \n");
    if(scanf("%d", &playerId) != 1){
        printf("Id should be the number only.");
        while(getchar() != '\n');
        return;
    }
    if(playerId < MIN_ID || playerId > MAX_ID){
        printf("Invalid player id. It should be between %d to %d !", MIN_ID, MAX_ID);
        return;
    }
    temp = headPlayerLL;
    while (temp != NULL)
    {
        if(temp->id == playerId){
            printf("Player Id should be unique.");
            return;
        }
        temp = temp->next;
    }
    getchar();
    printf("Name: \n");
    if(fgets(playerName, MAX_NAME_LENGTH , stdin) == NULL){
        printf("Something went wrong.");
        while (getchar() != '\n');
        return;
    }
    playerName[strcspn(playerName,"\n")]= '\0';
    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): \n");
    if(scanf("%d", &roleId) != 1){
        printf("Id should be the number only.");
        while(getchar() != '\n');
        return;
    }
    if(roleId < 1 || roleId > 3){
        printf("Role should be between 1 to 3");
        return;
    }
    printf("Total Runs: \n");
    if(scanf("%d", &totalRun) != 1){
        printf("Total number of run should be the number only.");
        while(getchar() != '\n');
        return;
    }
    printf("Batting Average:\n");
    if(scanf("%f", &battingAvg) != 1){
        printf("It should be valid batting average.");
        while(getchar() != '\n');
        return;
    }
    printf("Strike Rate:\n");
    if(scanf("%f", &strikeRate) != 1){
        printf("It should be valid Strike Rate.");
        while(getchar() != '\n');
        return;
    }
    printf("Wickets:\n");
    if(scanf("%d", &wicket) != 1){
        printf("It should be valid wickets.");
        while(getchar() != '\n');
        return;
    }
    printf("Economy Rate:\n");
    if(scanf("%f", &economyRate) != 1){
        printf("It should be valid economy rate.");
        while(getchar() != '\n');
        return;
    }

    struct NodePlayer *newNodePlayer = (struct NodePlayer*)malloc(sizeof(struct NodePlayer));
    if(newNodePlayer == NULL){
            printf("Memory allocation Failed");
            return;
    }

    newNodePlayer->id= playerId;
    newNodePlayer->name = malloc(strlen(playerName) +1);
    if(newNodePlayer->name == NULL){
        printf("Memory allocation Failed");
        return;
    }
    strcpy(newNodePlayer->name, playerName);
    newNodePlayer->teamID = teamId;
    newNodePlayer->roleID = roleId;
    newNodePlayer->totalRuns = totalRun;
    newNodePlayer->battingAverage = battingAvg;
    newNodePlayer->strikeRate = strikeRate;
    newNodePlayer->wickets = wicket;
    newNodePlayer->economyRate = economyRate;

    if(newNodePlayer->roleID  == BATSMAN){ // Batsman
        newNodePlayer->PerformanceIndex = (newNodePlayer->battingAverage * newNodePlayer->strikeRate) /100; //(BattingAverage × StrikeRate) / 100
    }else if(newNodePlayer->roleID  == BOWLER){ //Bowler
        newNodePlayer->PerformanceIndex  = (newNodePlayer->wickets * 2) + (100 - newNodePlayer->economyRate);// (Wickets × 2) + (100 − EconomyRate)
    }else if(newNodePlayer->roleID  == ALL_ROUNDER){ // All-rounder
        newNodePlayer->PerformanceIndex = ((newNodePlayer->battingAverage * newNodePlayer->strikeRate ) / 100)+(newNodePlayer->wickets * 2);//[(BattingAverage × StrikeRate) / 100] + (Wickets × 2)
    }

    newNodePlayer->next = NULL;

    tailPlayerLL->next = newNodePlayer;
    tailPlayerLL = newNodePlayer;

    struct Team *updateTeam = teamHead;
    while (updateTeam != NULL)
    {
        if(updateTeam->teamId == newNodePlayer->teamID){
            updateTeam->totalPlayers = updateTeam->totalPlayers + 1;
            break;
        }
        updateTeam = updateTeam->next;
    }  

    calAvgSR();
    printf("Player added successfully to Team %s!\n", teams[teamId-1]);
    struct NodePlayer *lastPlayer = tailPlayerLL;
    printf("%d | %s | %s | %s | %d | %0.1f | %.1f | %d | %.1f | %.2f \n", lastPlayer->id, lastPlayer->name, teams[lastPlayer->teamID -1], roles[lastPlayer->roleID -1], lastPlayer->totalRuns, lastPlayer->battingAverage, lastPlayer->strikeRate, lastPlayer->wickets, lastPlayer->economyRate, lastPlayer->PerformanceIndex);
    return;
}

void getPlayersByTeamID(){
    printf("Choice 2 -> Display All Players of a Specific Team\n");
    int teamId;
    printf("Enter Team ID: ");
    if(scanf("%d", &teamId) != 1){
        printf("Id should be the number only.");
        while (getchar() != '\n');
        return;
    }
    if(teamId < 1 || teamId > 10){
        printf("Invalid Team Id. It should be between 1 to 10 only!");
        return;
    }
    struct NodePlayer *temp = headPlayerLL;
    printf("Players of Team %s:\n", teams[teamId -1]);
    printf("====================================================================================\n");
    printf("ID | Name | Runs | Avg | SR | Wkts | ER | Pref.Index\n");
    printf("====================================================================================\n");
    int teamPlayerCount = 0;
    float totalSR = 0.0;
    while (temp != NULL)
    {
        if(temp->teamID == teamId){
            printf("%d | %s |  %d | %0.1f | %.1f | %d | %.1f | %.2f\n", temp->id, temp->name,  temp->totalRuns, temp->battingAverage, temp->strikeRate, temp->wickets, temp->economyRate, temp->PerformanceIndex);
            teamPlayerCount++;
            totalSR += temp->strikeRate;
        }
        temp = temp->next;
    }
    printf("====================================================================================\n");
    printf("Total Players : %d\n", teamPlayerCount);
    float avg = (teamPlayerCount == 0) ? 0 : totalSR / teamPlayerCount;
    printf("Average Batting Strike Rate: %.2f\n", avg);
    return;
}
void swap(struct Team **avgSR1, struct Team **avgSR2){
    struct Team  *temp = *avgSR1;
    *avgSR1 = *avgSR2;
    *avgSR2 = temp;
}
void teamsbyAverageBatting(){
    printf("Choice 3 -> Display Teams by Average Batting StrikeRate\n");
    printf("====================================================================================\n");
    printf("ID | TeamName | AvgBatSR | TotalPlayers\n");
    printf("====================================================================================\n");
    struct Team *teamArray[teamCount];
    struct Team *curr = teamHead;
    int index = 0;
    while (curr != NULL)
    {
        teamArray[index++] = curr;
        curr = curr->next;
    }
    // sort 
    for(int i = 0; i < teamCount; i++){
        for (int j = 0; j < teamCount - i - 1; j++)
        {
            if(teamArray[j]->avgBattingStrikerate < teamArray[j+1]->avgBattingStrikerate )
                swap(&teamArray[j], &teamArray[j+1]);
        }
    }
    for(int i = 0 ; i < teamCount ; i++){
        printf("%d | %s | %.1f | %d \n", teamArray[i]->teamId, teamArray[i]->name, teamArray[i]->avgBattingStrikerate, teamArray[i]->totalPlayers);
    }
    return;
}

bool usedPlayer(int id, int usedID[], int k){
    for(int i = 0; i < k; i++){
        if(usedID[i] == id) return true;
    }
    return false;
}

void topKPlayers(){
    printf("Choice4 -> Display Top K Players of a Specific Team of specific role\n");
    int teamId , roleId, k;

    printf("EnterTeamID:");
    if(scanf("%d", &teamId) != 1){
        printf("Id should be the number only.");
        while (getchar() != '\n');
        return;
    }
    if(teamId < 1 || teamId > 10){
        printf("Invalid Team Id. It should be between 1 to 10 only!");
        return;
    }
    printf("EnterRole(1-Batsman,2-Bowler,3-All-rounder):");
    if(scanf("%d", &roleId) != 1){
        printf("Id should be the number only.");
        while (getchar() != '\n');
        return;
    }
    if(roleId < 1 || roleId > 3){
        printf("Role should be between 1 to 3");
        return;
    }
    printf("Enter number of players:");
    if(scanf("%d", &k) != 1){
        printf("It should be the number only.");
        while (getchar() != '\n');
        return;
    }

    int usedID[k];  
    for(int i = 0; i < k; i++) usedID[i] = -1;

    printf("\nTop %d Players:\n", k);
    printf("=============================================================\n");
    printf("ID | Name | Runs |  Avg  | SR  | Wkts | PerfIdx\n");
    printf("=============================================================\n");

    for(int t = 0; t < k; t++){
        struct NodePlayer *temp = headPlayerLL;
        struct NodePlayer *best = NULL;
        float bestPI = -1.0;

        while(temp != NULL){
            if(temp->teamID == teamId &&
               temp->roleID == roleId &&
               !usedPlayer(temp->id, usedID, k) &&
               temp->PerformanceIndex > bestPI)
            {
                bestPI = temp->PerformanceIndex;
                best = temp;
            }
            temp = temp->next;
        }

        // no more players available
        if(best == NULL) break;

        usedID[t] = best->id;

        printf("%d  %-15s %d  %.1f  %.1f  %d  %.2f\n",
               best->id, best->name, best->totalRuns,
               best->battingAverage, best->strikeRate,
               best->wickets, best->PerformanceIndex);
    }
}

void getPlayerbyRoleId(){

    struct NodePlayer *playerArray[playerCount];
    struct NodePlayer *curr = headPlayerLL;
    int index = 0;
    while (curr != NULL)
    {
        playerArray[index++] = curr;
        curr = curr->next;
    }
    for(int i = 0 ; i < playerCount-1 ; i++){
        for(int j = 0 ; j < playerCount - i -1 ; j++){
            if(playerArray[j]->PerformanceIndex < playerArray[j+1]->PerformanceIndex){
                struct NodePlayer *temp = playerArray[j];
                playerArray[j] = playerArray[j+1];
                playerArray[j+1] = temp;
            }
        }
    }
    printf("Choice5 -> Display All Players Across All Teams of specific role");
    int roleId;
    printf("\nEnter RoleID: ");
    if(scanf("%d", &roleId) != 1){
        printf("Id should be the number only.");
        while (getchar() != '\n');
        return;
    }
    if(roleId < 1 || roleId > 3){
        printf("Invalid Role Id. It should be between 1 to 3 only!");
        return;
    }
    printf("%s of all teams:\n", roles[roleId -1]);

    printf("====================================================================================\n");
    printf("ID | Name | Team | Role | Runs |  Avg |  SR |  Wkts | ER | Perf.IndexName\n");
    printf("====================================================================================\n");
    
    for(int i = 0 ; i < playerCount ; i++){
        if(playerArray[i]->roleID == roleId){
            printf("%d | %s | %s | %s | %d | %0.1f | %.1f | %d | %.1f | %.1f\n", playerArray[i]->id , playerArray[i]->name, teams[playerArray[i]->teamID -1], roles[playerArray[i]->roleID -1], playerArray[i]->totalRuns, playerArray[i]->battingAverage, playerArray[i]->strikeRate, playerArray[i]->wickets, playerArray[i]->economyRate,playerArray[i]->PerformanceIndex );
        }   
    }
    printf("====================================================================================\n");
    return;
}

void freePlayers() {
    struct NodePlayer *temp = headPlayerLL;
    while (temp != NULL) {
        struct NodePlayer *next = temp->next;

        free(temp->name);     // free string
        free(temp);           // free node

        temp = next;
    }
}
void freeTeams() {
    struct Team *temp = teamHead;
    while (temp != NULL) {
        struct Team *next = temp->next;

        free(temp->name);     // free string
        free(temp);           // free node

        temp = next;
    }
}
void printMenu(){
    printf("\n==================================================================================\n");
    printf("ICC ODI Player Performance Analyzer\n");
    printf("==================================================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    printf("==================================================================================\n");
    return;
}

void getChoice(){
    int choice;
    printf("\nEnter your choice: ");
    if(scanf("%d", &choice) != 1){
        printf("Invalid Number.");
        while(getchar() != '\n');
        return;
        // exit(0);
    }
    switch (choice)
    {
    case 1:{
        addPlayerToTeam();
        break;
    } 
    case 2:{
        getPlayersByTeamID();
        break;
    }
    case 3:{
        teamsbyAverageBatting();
        break;
    }
    case 4:
        topKPlayers();
        break;
    case 5:
        getPlayerbyRoleId();
        break;
    case 6:
        freePlayers();
        freeTeams();
        printf("Exiting...");
        exit(0);
        break;
    default:
        printf("Choice a number between 1 to 6.");
        break;
    }
}

int main(){

    initializeTeam();
    initalizePlayers();
    
    while (true)
    {   
        printMenu();
        getChoice();
    }
    return 0;
}