#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>  // open
#include <string.h> // strlen
#include <stdlib.h> // exit

/*
    Peterson's Solution (Synchronization)
    -------------------------------------
    This program simulates mutual exclusion for two processes
    using two threads.

    It also sends log messages to the centralized logger
    through the FIFO: /tmp/logger_fifo
*/

/* Shared variables used in Peterson's solution */
volatile int flag[2] = {0, 0};
volatile int turn = 0;

/* Shared resource */
int shared_value = 0;

/* FIFO path must match logger.c exactly */
const char *loggerFifoFilePath = "/tmp/logger_fifo";

/*
    send_log_message()
    ------------------
    This function opens the logger FIFO and sends one message.

    The logger program will receive this message, add a timestamp,
    and write it into logs.txt.
*/


void send_log_message(const char *message)
{
    int fd = open(loggerFifoFilePath, O_WRONLY);
    if (fd != -1)
    {
        write(fd, message, strlen(message));
        write(fd, "\n", 1);
        close(fd);
    }
}

/*
    process()
    ---------
    This function is run by each thread.

    id = 0 for process 0
    id = 1 for process 1
*/
void *process(void *arg)
{
    int id = *(int *)arg;
    int other = 1 - id;

    char logMessage[200];

    for (int i = 0; i < 5; i++)
    {

        /* ================= ENTRY SECTION ================= */
        printf("Process %d: Entry section\n", id);

        snprintf(logMessage, sizeof(logMessage),
                 "Process %d entered entry section", id);
        send_log_message(logMessage);

        /* This process says: "I want to enter" */
        flag[id] = 1;

        /* Give the other process priority if both want in */
        turn = other;

        /* Busy-wait until it is safe to enter */
        while (flag[other] && turn == other)
        {
            /* do nothing - just wait */
        }

        /* =============== CRITICAL SECTION =============== */
        printf("Process %d: Critical section\n", id);

        snprintf(logMessage, sizeof(logMessage),
                 "Process %d entered critical section", id);
        send_log_message(logMessage);

        /*
            Only one process should be here at a time.
            We update a shared variable to show protected access.
        */
        int temp = shared_value;
        printf("Process %d: reads shared_value = %d\n", id, temp);

        temp++;
        usleep(100000); // small pause so output is easier to see

        shared_value = temp;
        printf("Process %d: updates shared_value to %d\n", id, shared_value);

        /* ================= EXIT SECTION ================= */
        printf("Process %d: Exit section\n\n", id);

        snprintf(logMessage, sizeof(logMessage),
                 "Process %d entered exit section", id);
        send_log_message(logMessage);

        /* This process is done using the critical section */
        flag[id] = 0;

        /* remainder section */
        usleep(100000);
    }

    return NULL;
}

int main()
{
    pthread_t t0, t1;
    int id0 = 0;
    int id1 = 1;

    printf("Starting Peterson's Solution simulation...\n\n");

    send_log_message("Peterson module started");

    /* Create two threads to simulate two processes */
    pthread_create(&t0, NULL, process, &id0);
    pthread_create(&t1, NULL, process, &id1);

    /* Wait for both threads to finish */
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);

    printf("Final shared_value = %d\n", shared_value);

    send_log_message("Peterson module finished");

    return 0;
}