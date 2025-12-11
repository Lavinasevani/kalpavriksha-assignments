#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h> 

#define TABLE_SIZE 13
#define BUFFER 100

enum ProcessState { READY, RUNNING, WAITING, TERMINATED };

struct PCB {
    char *processName;
    int PID;
    int burstTime;
    int ioStartTime;
    int ioDuration;
    int executedTime;       
    int ioRemainingTime;     
    int completionTime;    
    enum ProcessState state; 
    struct PCB *next; 
};

struct PCB *bucket[TABLE_SIZE];

struct QueueNode {
    struct PCB *process;
    struct QueueNode *next;
};

struct Queue {
    struct QueueNode *front;
    struct QueueNode *rear;
};

struct Queue* createQueue() {
    struct Queue *q = malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

void enqueue(struct Queue *q, struct PCB *p) {
    struct QueueNode *node = malloc(sizeof(struct QueueNode));
    node->process = p;
    node->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = node;
        return;
    }

    q->rear->next = node;
    q->rear = node;
}

struct PCB* dequeue(struct Queue *q) {
    if (q->front == NULL) return NULL;

    struct QueueNode *temp = q->front;
    struct PCB *p = temp->process;

    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;

    free(temp);
    return p;
}

bool isValidNumber(const char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}

bool isPIDUnique(int pid) {
    int index = pid % TABLE_SIZE;
    struct PCB *current = bucket[index];
    while (current != NULL) {
        if (current->PID == pid) return false;
        current = current->next;
    }
    return true;
}

void processInitialization(char *processDetails, struct Queue *readyQueue) {
    char *tokens[5];
    int i = 0;

    char *token = strtok(processDetails, " ");
    while (token != NULL && i < 5) {
        tokens[i++] = token;
        token = strtok(NULL, " ");
    }

    if (i != 5) {
        printf("Invalid input format. Use: <Process_name> <PID> <burst> <ioStart> <ioDuration>\n");
        return;
    }

    for (int j = 1; j <= 4; j++) {
        if (!isValidNumber(tokens[j])) {     
            if(j == 1){
                printf("PID should be number only.\n");
            }else if(j ==2){
                printf("Burst Time should be number only.\n");
            }else if(j == 3){
                printf("Invalid ioStart time\n");
            }
            else{
                printf("Invalid ioDuration\n");
            }
            return;
        }
    }

    int pid = atoi(tokens[1]);
    if (!isPIDUnique(pid)) {
        printf("Error: PID %d already exists. Enter a unique PID.\n", pid);
        return;
    }
    
    struct PCB *newBlock = malloc(sizeof(struct PCB));
    newBlock->processName = malloc(strlen(tokens[0]) + 1);
    strcpy(newBlock->processName, tokens[0]);
    newBlock->PID = pid;
    newBlock->burstTime = atoi(tokens[2]);
    newBlock->ioStartTime = atoi(tokens[3]);
    newBlock->ioDuration = atoi(tokens[4]);
    newBlock->executedTime = 0;
    newBlock->ioRemainingTime = 0;
    newBlock->completionTime = 0;
    newBlock->state = READY;
    
    newBlock->next = NULL;
    
    int index = newBlock->PID % TABLE_SIZE;
    if (bucket[index] == NULL) {
        bucket[index] = newBlock;
    } else {
        newBlock->next = bucket[index];
        bucket[index] = newBlock;
    }

    enqueue(readyQueue, newBlock);
    
    printf("Process %s (PID %d) added to PCB and Ready queue successfully.\n", newBlock->processName, newBlock->PID);
}

int systemClock = 0;

void runScheduler(struct Queue *readyQueue, struct Queue *terminatedQueue) {
    
    while (readyQueue->front != NULL) {
        struct PCB *current = dequeue(readyQueue);
        
        if (current == NULL) break;
        
        current->state = RUNNING;
        printf("\n[Tick %d] Running Process: PID %d (%s)\n", 
               systemClock, current->PID, current->processName);
        
        for (int i = 0; i < current->burstTime; i++) {
            sleep(1); 
            current->executedTime++;
            systemClock++;
            
            printf("  Tick %d: PID %d executed %d/%d units\n", 
                   systemClock, current->PID, current->executedTime, current->burstTime);
        }
        
        current->state = TERMINATED;
        current->completionTime = systemClock;
        enqueue(terminatedQueue, current);
        
        printf("  PID %d completed at Tick %d\n", current->PID, systemClock);
    }
 
}

void displayStatistics(struct Queue *terminatedQueue) {
    printf("%-6s %-12s %-8s %-8s %-12s %-10s\n", "PID", "Name", "CPU", "I/O", "Turnaround", "Waiting");
    
    struct PCB *p;
    while ((p = dequeue(terminatedQueue)) != NULL) {
        int turnaroundTime = p->completionTime;
        
        int waitingTime = turnaroundTime - p->burstTime - p->ioDuration;
        
        printf("%-6d %-12s %-8d %-8d %-12d %-10d\n",
               p->PID, p->processName, p->burstTime, p->ioDuration, 
               turnaroundTime, waitingTime);
        
        free(p->processName);
        free(p);
    }
}

int main() {
    struct Queue *readyQueue = createQueue();
    struct Queue *waitingQueue = createQueue();
    struct Queue *terminatedQueue = createQueue();
    
    char processDetails[BUFFER];

    printf("\nEnter process details in format: <name> <PID> <burst> <ioStart> <ioDuration>\n");
    
    while (1) {
        printf("Enter process (or type 'exit' to finish): ");
        if (fgets(processDetails, BUFFER, stdin) == NULL) continue;

        processDetails[strcspn(processDetails, "\n")] = 0;

        if (strcmp(processDetails, "exit") == 0) break;

        processInitialization(processDetails, readyQueue);
    }

    printf("\nAll processes stored in PCB hash table.\n");

    runScheduler(readyQueue, terminatedQueue);
    
    displayStatistics(terminatedQueue);
    
    free(readyQueue);
    free(waitingQueue);
    free(terminatedQueue);

    return 0;
}
