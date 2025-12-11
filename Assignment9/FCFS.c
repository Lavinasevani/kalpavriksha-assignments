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

struct PCB* peek(struct Queue *q) {
    if (q->front == NULL) return NULL;
    return q->front->process;
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

void processWaitingQueue(struct Queue *waitingQueue, struct Queue *readyQueue) {
    if (waitingQueue->front == NULL) return;
    
    struct QueueNode *prev = NULL;
    struct QueueNode *current = waitingQueue->front;
    
    while (current != NULL) {
        struct PCB *process = current->process;
        process->ioRemainingTime--;
        
        printf("    [I/O] PID %d: I/O remaining %d units\n", 
               process->PID, process->ioRemainingTime);
        
        if (process->ioRemainingTime <= 0) {
            printf("    [I/O Complete] PID %d moved to Ready Queue\n", process->PID);
            
            process->state = READY;
            enqueue(readyQueue, process);
            
            struct QueueNode *toRemove = current;
            if (prev == NULL) {
                waitingQueue->front = current->next;
                current = current->next;
            } else {
                prev->next = current->next;
                current = current->next;
            }
            
            if (toRemove == waitingQueue->rear) {
                waitingQueue->rear = prev;
            }
            
            free(toRemove);
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void executeTick(struct PCB *current, struct Queue *readyQueue, struct Queue *waitingQueue, struct Queue *terminatedQueue) {
    
    current->state = RUNNING;
    printf("\n[Tick %d] Running: PID %d (%s) | Executed: %d/%d\n", systemClock, current->PID, current->processName, current->executedTime, current->burstTime);
    
    sleep(1);  
    current->executedTime++;
    systemClock++;
    
    if (current->executedTime == current->ioStartTime && current->ioDuration > 0) {
        printf("  [I/O Request] PID %d starting I/O (%d units)\n", 
               current->PID, current->ioDuration);
        current->state = WAITING;
        current->ioRemainingTime = current->ioDuration;
        enqueue(waitingQueue, current);
    }
    else if (current->executedTime >= current->burstTime) {
        printf("  [Completed] PID %d finished execution\n", current->PID);
        current->state = TERMINATED;
        current->completionTime = systemClock;
        enqueue(terminatedQueue, current);
    }
    else {
        current->state = READY;
        enqueue(readyQueue, current);
    }
    
    processWaitingQueue(waitingQueue, readyQueue);
}

void runScheduler(struct Queue *readyQueue, struct Queue *waitingQueue, 
                  struct Queue *terminatedQueue) {
    printf("\n=== Starting FCFS Scheduler with I/O Handling ===\n");
    
    // Continue while there are processes in ready or waiting queues
    while (readyQueue->front != NULL || waitingQueue->front != NULL) {
        
        // If ready queue has processes, execute one tick
        if (readyQueue->front != NULL) {
            struct PCB *current = dequeue(readyQueue);
            executeTick(current, readyQueue, waitingQueue, terminatedQueue);
        } 
        // If ready queue is empty but waiting queue has processes (CPU idle)
        else if (waitingQueue->front != NULL) {
            printf("\n[Tick %d] CPU IDLE - All processes in I/O\n", systemClock);
            sleep(1);
            systemClock++;
            processWaitingQueue(waitingQueue, readyQueue);
        }
    }
    
    printf("\n=== All processes completed ===\n");
}


void displayStatistics(struct Queue *terminatedQueue) {
    printf("\n=== Final Statistics ===\n");
    printf("%-6s %-12s %-8s %-8s %-12s %-10s\n", 
           "PID", "Name", "CPU", "I/O", "Turnaround", "Waiting");
    
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

    runScheduler(readyQueue, waitingQueue, terminatedQueue);
    
    displayStatistics(terminatedQueue);
    
    free(readyQueue);
    free(waitingQueue);
    free(terminatedQueue);
    
    return 0;
}
