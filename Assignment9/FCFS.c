#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h> 

#define TABLE_SIZE 13
#define BUFFER 100
#define MAX_KILLS 50

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
    bool wasKilled;
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

struct KillEvent {
    int pid;
    int killTime;
};

struct KillEvent killEvents[MAX_KILLS];
int killEventCount = 0;

struct Queue* createQueue() {
    struct Queue *q = malloc(sizeof(struct Queue));
    if (q == NULL) {
        printf("Error: Memory allocation failed for queue\n");
        exit(1);
    }
    q->front = q->rear = NULL;
    return q;
}

void enqueue(struct Queue *q, struct PCB *p) {
    struct QueueNode *node = malloc(sizeof(struct QueueNode));
    if (node == NULL) {
        printf("Error: Memory allocation failed for queue node\n");
        return;
    }
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

    if (str == NULL || str[0] == '\0') return false;

    if (strcmp(str, "-") == 0) return true; // Accept "-" this indicate no I/O (treated as 0)

    for (int i = 0; str[i]; i++) {
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}

int parseNumber(const char *str) { // convert "-" to 0 (no I/O operation)
    if (str == NULL || strcmp(str, "-") == 0) return 0;
    return atoi(str);
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

struct PCB* findProcessByPID(int pid) {
    int index = pid % TABLE_SIZE;
    struct PCB *current = bucket[index];
    while (current != NULL) {
        if (current->PID == pid) return current;
        current = current->next;
    }
    return NULL;
}

bool performKillCommand(char *input) {
    char *tokens[3];
    int i = 0;
    
    char inputCopy[BUFFER];
    strcpy(inputCopy, input);
    
    char *token = strtok(inputCopy, " ");
    while (token != NULL && i < 3) {
        tokens[i++] = token;
        token = strtok(NULL, " ");
    }
    
    if (i != 3) {
        printf("Invalid KILL format. Use: KILL <PID> <time>\n");
        return false;
    }
    
    if (strcmp(tokens[0], "KILL") != 0) {
        return false;
    }
    
    if (!isValidNumber(tokens[1]) || !isValidNumber(tokens[2])) {
        printf("PID and time must be numbers.\n");
        return false;
    }
    
    int pid = parseNumber(tokens[1]);
    int killTime = parseNumber(tokens[2]);
    if (!findProcessByPID(pid)) {
        printf("Error: PID %d does not exist.\n", pid);
        return false;
    }
    
    if (killEventCount < MAX_KILLS) {
        killEvents[killEventCount].pid = pid;
        killEvents[killEventCount].killTime = killTime;
        killEventCount++;
        return true;
    } else {
        printf("Error: Maximum kill events reached.\n");
        return false;
    }
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
    if (newBlock == NULL) {
        printf("Error: Memory allocation failed for PCB\n");
        return;
    }    
    newBlock->processName = malloc(strlen(tokens[0]) + 1);
    if (newBlock->processName == NULL) {
        printf("Error: Memory allocation failed for process name\n");
        free(newBlock);
        return;
    }
    strcpy(newBlock->processName, tokens[0]);
    newBlock->PID = pid;
    newBlock->burstTime = atoi(tokens[2]);
    newBlock->ioStartTime = atoi(tokens[3]);
    newBlock->ioDuration = atoi(tokens[4]);
    newBlock->executedTime = 0;
    newBlock->ioRemainingTime = 0;
    newBlock->completionTime = 0;
    newBlock->state = READY;
    newBlock->wasKilled = false;
    newBlock->next = NULL;
    
    int index = newBlock->PID % TABLE_SIZE;
    if (bucket[index] == NULL) {
        bucket[index] = newBlock;
    } else {
        newBlock->next = bucket[index];
        bucket[index] = newBlock;
    }

    enqueue(readyQueue, newBlock);
    
}

int systemClock = 0;

bool removeFromQueue(struct Queue *q, int pid) {
    if (q->front == NULL) return false;
    
    struct QueueNode *prev = NULL;
    struct QueueNode *current = q->front;
    
    while (current != NULL) {
        if (current->process->PID == pid) {
            if (prev == NULL) {
                q->front = current->next;
            } else {
                prev->next = current->next;
            }
            
            if (current == q->rear) {
                q->rear = prev;
            }
            
            free(current);
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

void checkKillEvents(struct Queue *readyQueue, struct Queue *waitingQueue, struct Queue *terminatedQueue, struct PCB **runningProcess) {
    for (int i = 0; i < killEventCount; i++) {
        if (killEvents[i].killTime == systemClock) {
            int pid = killEvents[i].pid;
            struct PCB *process = findProcessByPID(pid);
            
            if (process == NULL || process->state == TERMINATED) {
                continue;
            }

            if (runningProcess != NULL && *runningProcess != NULL && 
                (*runningProcess)->PID == pid) {
                (*runningProcess)->state = TERMINATED;
                (*runningProcess)->completionTime = systemClock;
                (*runningProcess)->wasKilled = true;
                enqueue(terminatedQueue, *runningProcess);
                *runningProcess = NULL;  
            }
            else if (removeFromQueue(readyQueue, pid)) {
                process->state = TERMINATED;
                process->completionTime = systemClock;
                process->wasKilled = true;
                enqueue(terminatedQueue, process);
            }
            else if (removeFromQueue(waitingQueue, pid)) {
                process->state = TERMINATED;
                process->completionTime = systemClock;
                process->wasKilled = true;
                enqueue(terminatedQueue, process);
            }
        }
    }
}

void processWaitingQueue(struct Queue *waitingQueue, struct Queue *readyQueue) {
    struct QueueNode *prev = NULL, *curr = waitingQueue->front;
    while (curr) {
        struct QueueNode *nextNode = curr->next;
        struct PCB *p = curr->process;
        p->ioRemainingTime--;
        if (p->ioRemainingTime <= 0) {
            p->state = READY;
            enqueue(readyQueue, p);
            if (!prev) waitingQueue->front = nextNode;
            else prev->next = nextNode;
            if (curr == waitingQueue->rear) waitingQueue->rear = prev;
            free(curr);
        } else {
            prev = curr;
        }
        curr = nextNode;
    }
}

// void executeProcess(struct PCB *p, struct Queue *readyQueue, struct Queue *waitingQueue, struct Queue *terminatedQueue) {
//     while (p) {
//         p->state = RUNNING;
//         checkKillEvents(readyQueue, waitingQueue, terminatedQueue, &p);
//         if (!p) return;

//         p->executedTime++;
//         systemClock++;
//     if (p->ioDuration > 0 && p->executedTime == p->ioStartTime) {
//             p->state = WAITING;
//             p->ioRemainingTime = p->ioDuration;
//             enqueue(waitingQueue, p);
//             return;
//         }

//         if (p->executedTime >= p->burstTime) {
//             p->state = TERMINATED;
//             p->completionTime = systemClock;
//             enqueue(terminatedQueue, p);
//             return;
//         }

//         processWaitingQueue(waitingQueue, readyQueue);
//     }
// }

// void runScheduler(struct Queue *readyQueue, struct Queue *waitingQueue, struct Queue *terminatedQueue) {
//     struct PCB *current = NULL;

//     while (readyQueue->front || waitingQueue->front || current) {
//         if (!current && readyQueue->front) {
//             current = dequeue(readyQueue);
//         }
//         if (current) {
//             current->state = RUNNING;
//             current->executedTime++;
//             systemClock++;

//             if (current->ioDuration > 0 && current->executedTime == current->ioStartTime) {
//                 current->state = WAITING;
//                 current->ioRemainingTime = current->ioDuration;
//                 enqueue(waitingQueue, current);
//                 current = NULL;
//             }
//             else if (current->executedTime >= current->burstTime) {
//                 current->state = TERMINATED;
//                 current->completionTime = systemClock;
//                 enqueue(terminatedQueue, current);
//                 current = NULL;
//             }
//         } else {
//             systemClock++;
//         }

//         struct QueueNode *prev = NULL;
//         struct QueueNode *currNode = waitingQueue->front;
//         while (currNode) {
//             struct PCB *p = currNode->process;
//             p->ioRemainingTime--;
//             if (p->ioRemainingTime <= 0) {
//                 p->state = READY;
//                 enqueue(readyQueue, p);
//                 struct QueueNode *temp = currNode;
//                 if (!prev) waitingQueue->front = currNode->next;
//                 else prev->next = currNode->next;
//                 if (currNode == waitingQueue->rear) waitingQueue->rear = prev;
//                 currNode = currNode->next;
//                 free(temp);
//             } else {
//                 prev = currNode;
//                 currNode = currNode->next;
//             }
//         }
//     }
// }


void runScheduler(struct Queue *readyQueue, struct Queue *waitingQueue, struct Queue *terminatedQueue) {
    struct PCB *current = NULL;

    while (readyQueue->front || waitingQueue->front || current) {
        if (!current && readyQueue->front) {
            current = dequeue(readyQueue);
        }
        
        if (current) {
            current->state = RUNNING;
            current->executedTime++;
            systemClock++;
            
            checkKillEvents(readyQueue, waitingQueue, terminatedQueue, &current);
            
            if (current == NULL) {
                struct QueueNode *prev = NULL;
                struct QueueNode *currNode = waitingQueue->front;
                while (currNode) {
                    struct PCB *p = currNode->process;
                    p->ioRemainingTime--;
                    if (p->ioRemainingTime <= 0) {
                        p->state = READY;
                        enqueue(readyQueue, p);
                        struct QueueNode *temp = currNode;
                        if (!prev) waitingQueue->front = currNode->next;
                        else prev->next = currNode->next;
                        if (currNode == waitingQueue->rear) waitingQueue->rear = prev;
                        currNode = currNode->next;
                        free(temp);
                    } else {
                        prev = currNode;
                        currNode = currNode->next;
                    }
                }
                continue;
            }

            if (current->ioDuration > 0 && current->executedTime == current->ioStartTime) {
                current->state = WAITING;
                current->ioRemainingTime = current->ioDuration;
                enqueue(waitingQueue, current);
                current = NULL;
            }
            else if (current->executedTime >= current->burstTime) {
                current->state = TERMINATED;
                current->completionTime = systemClock;
                enqueue(terminatedQueue, current);
                current = NULL;
            }
        } else {
            systemClock++;
            checkKillEvents(readyQueue, waitingQueue, terminatedQueue, &current);
        }

        struct QueueNode *prev = NULL;
        struct QueueNode *currNode = waitingQueue->front;
        while (currNode) {
            struct PCB *p = currNode->process;
            p->ioRemainingTime--;
            if (p->ioRemainingTime <= 0) {
                p->state = READY;
                enqueue(readyQueue, p);
                struct QueueNode *temp = currNode;
                if (!prev) waitingQueue->front = currNode->next;
                else prev->next = currNode->next;
                if (currNode == waitingQueue->rear) waitingQueue->rear = prev;
                currNode = currNode->next;
                free(temp);
            } else {
                prev = currNode;
                currNode = currNode->next;
            }
        }
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

    printf("\nEnter process details in format: <name> <PID> <burst> <ioStart> <ioDuration> or KILL command: KILL <PID> <time>\n");
    
    while (1) {
        printf("Enter process (or type 'exit' to finish): ");
        if (fgets(processDetails, BUFFER, stdin) == NULL) continue;

        processDetails[strcspn(processDetails, "\n")] = 0;

        if (strcmp(processDetails, "exit") == 0) break;

         if (strncmp(processDetails, "KILL", 4) == 0) {
            performKillCommand(processDetails);
        } else {
            processInitialization(processDetails, readyQueue);
        }
    }

    runScheduler(readyQueue, waitingQueue, terminatedQueue);
    
    displayStatistics(terminatedQueue);
    
    free(readyQueue);
    free(waitingQueue);
    free(terminatedQueue);
    
    return 0;
}
