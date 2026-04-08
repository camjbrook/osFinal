#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_BLOCKS 100
#define MAX_NAME_LEN 50
#define TOTAL_MEMORY 1024

typedef struct
{
    int start;
    int size;
    int free;
    char processName[MAX_NAME_LEN];
} MemoryBlock;

/* ---------- LOGGER ---------- */
/* ... (existing defines and structs) ... */
void sendLogMessage(const char *logMessage)
{
    const char *loggerFifoPath = "/tmp/logger_fifo";
    int fd = open(loggerFifoPath, O_WRONLY | O_NONBLOCK);
    if (fd != -1)
    {
        write(fd, logMessage, strlen(logMessage));
        close(fd);
    }
}

/* ---------- MEMORY MANAGEMENT ---------- */
MemoryBlock memory[MAX_BLOCKS];
int blockCount = 0;

void sortMemoryByStart()
{
    for (int i = 0; i < blockCount - 1; i++)
    {
        for (int j = 0; j < blockCount - i - 1; j++)
        {
            if (memory[j].start > memory[j + 1].start)
            {
                MemoryBlock temp = memory[j];
                memory[j] = memory[j + 1];
                memory[j + 1] = temp;
            }
        }
    }
}

void mergeFreeBlocks()
{
    sortMemoryByStart();

    for (int i = 0; i < blockCount - 1; i++)
    {
        if (memory[i].free && memory[i + 1].free &&
            memory[i].start + memory[i].size == memory[i + 1].start)
        {
            memory[i].size += memory[i + 1].size;

            for (int j = i + 1; j < blockCount - 1; j++)
            {
                memory[j] = memory[j + 1];
            }

            blockCount--;
            i--;
        }
    }
}

void initializeMemory()
{
    int holes;
    printf("Enter number of initial free memory holes: ");
    scanf("%d", &holes);

    if (holes <= 0 || holes > MAX_BLOCKS)
    {
        printf("Invalid number of holes.\n");
        return;
    }

    blockCount = 0;

    for (int i = 0; i < holes; i++)
    {
        int start, size;
        printf("Enter start address and size of hole %d: ", i + 1);
        scanf("%d %d", &start, &size);

        if (start < 0 || size <= 0 || start + size > TOTAL_MEMORY)
        {
            printf("Invalid hole. Try again.\n");
            i--;
            continue;
        }

        memory[blockCount].start = start;
        memory[blockCount].size = size;
        memory[blockCount].free = 1;
        strcpy(memory[blockCount].processName, "FREE");
        blockCount++;
    }

    mergeFreeBlocks();
    printf("Memory initialized.\n");
    sendLogMessage("Memory: Initialized memory holes\n");
}

void displayMemoryMap()
{
    sortMemoryByStart();

    printf("\n----- Memory Map -----\n");
    for (int i = 0; i < blockCount; i++)
    {
        printf("Block %d | Start: %d | Size: %dKB | %s",
               i,
               memory[i].start,
               memory[i].size,
               memory[i].free ? "FREE" : "ALLOCATED");

        if (!memory[i].free)
        {
            printf(" | Process: %s", memory[i].processName);
        }
        printf("\n");
    }
    printf("----------------------\n");

    sendLogMessage("Memory: Displayed memory map\n");
}

void allocateMemory()
{
    char processName[MAX_NAME_LEN];
    int size;

    printf("Enter process name: ");
    scanf("%s", processName);
    printf("Enter memory size to allocate (KB): ");
    scanf("%d", &size);

    if (size <= 0)
    {
        printf("Invalid size.\n");
        return;
    }

    for (int i = 0; i < blockCount; i++)
    {
        if (memory[i].free && memory[i].size >= size)
        {
            if (memory[i].size == size)
            {
                memory[i].free = 0;
                strcpy(memory[i].processName, processName);
            }
            else
            {
                if (blockCount >= MAX_BLOCKS)
                {
                    printf("Cannot allocate: too many memory blocks.\n");
                    return;
                }

                for (int j = blockCount; j > i; j--)
                {
                    memory[j] = memory[j - 1];
                }

                memory[i + 1].start = memory[i].start + size;
                memory[i + 1].size = memory[i].size - size;
                memory[i + 1].free = 1;
                strcpy(memory[i + 1].processName, "FREE");

                memory[i].size = size;
                memory[i].free = 0;
                strcpy(memory[i].processName, processName);

                blockCount++;
            }

            printf("Allocated %dKB to %s using first-fit.\n", size, processName);

            char logMessage[200];
            snprintf(logMessage, sizeof(logMessage),
                     "Memory: Allocated %dKB to %s using first-fit\n",
                     size, processName);
            sendLogMessage(logMessage);
            return;
        }
    }

    printf("Allocation failed: no suitable free block found.\n");
    sendLogMessage("Memory: Allocation failed\n");
}

void deallocateMemory()
{
    char processName[MAX_NAME_LEN];

    printf("Enter process name to deallocate: ");
    scanf("%s", processName);

    for (int i = 0; i < blockCount; i++)
    {
        if (!memory[i].free && strcmp(memory[i].processName, processName) == 0)
        {
            memory[i].free = 1;
            strcpy(memory[i].processName, "FREE");

            mergeFreeBlocks();

            printf("Deallocated memory for process %s.\n", processName);

            char logMessage[200];
            snprintf(logMessage, sizeof(logMessage),
                     "Memory: Deallocated process %s\n",
                     processName);
            sendLogMessage(logMessage);
            return;
        }
    }

    printf("Process not found.\n");
    sendLogMessage("Memory: Deallocation failed - process not found\n");
}

void compactMemory()
{
    MemoryBlock newMemory[MAX_BLOCKS];
    int newCount = 0;
    int currentStart = 0;
    int freeTotal = 0;

    sortMemoryByStart();

    for (int i = 0; i < blockCount; i++)
    {
        if (!memory[i].free)
        {
            newMemory[newCount].start = currentStart;
            newMemory[newCount].size = memory[i].size;
            newMemory[newCount].free = 0;
            strcpy(newMemory[newCount].processName, memory[i].processName);

            currentStart += memory[i].size;
            newCount++;
        }
        else
        {
            freeTotal += memory[i].size;
        }
    }

    if (freeTotal > 0)
    {
        newMemory[newCount].start = currentStart;
        newMemory[newCount].size = freeTotal;
        newMemory[newCount].free = 1;
        strcpy(newMemory[newCount].processName, "FREE");
        newCount++;
    }

    for (int i = 0; i < newCount; i++)
    {
        memory[i] = newMemory[i];
    }
    blockCount = newCount;

    printf("Memory compaction completed.\n");
    sendLogMessage("Memory: Performed compaction\n");
}

int main()
{
    int choice;

    initializeMemory();

    do
    {
        printf("\n=== Contiguous Memory Allocation Menu ===\n");
        printf("1. Allocate Memory (First-Fit)\n");
        printf("2. Deallocate Memory\n");
        printf("3. Display Memory Map\n");
        printf("4. Compact Memory\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            allocateMemory();
            break;
        case 2:
            deallocateMemory();
            break;
        case 3:
            displayMemoryMap();
            break;
        case 4:
            compactMemory();
            break;
        case 5:
            printf("Exiting memory module.\n");
            sendLogMessage("Memory: Exited memory module\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 5);

    return 0;
}