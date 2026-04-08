#include <stdio.h>    // printf, perror, fopen, fprintf, fclose
#include <unistd.h>   // read, close
#include <fcntl.h>    // open, O_RDONLY
#include <sys/stat.h> // mkfifo
#include <string.h>   // memset, strcspn
#include <time.h>     // time, localtime, strftime
#include <errno.h>    // errno

int main()
{

    /*

    CENTRALIZED LOGGER SYSTEM (FIFO-BASED)


    This program acts as the logging system for all modules.

    Every module sends messages through a named pipe (FIFO),
    and this logger receives those messages and writes them
    to a log file with timestamps.

    LOGGER ROLE: Receiver
    MODULE ROLE: Writers
    */

    // Shared communication channel (must match ALL modules)
    const char *loggerFifoFilePath = "/tmp/logger_fifo";

    // Log file location (relative to /bin execution)
    const char *logOutputFilePath = "../logs/logs.txt";

    // Buffer to store incoming log messages from modules
    char incomingLogMessageBuffer[1024];

    // Tracks number of bytes read from FIFO
    ssize_t numberOfBytesReceivedFromFifo;

    /*
    STEP 1 — CREATE FIFO (NAMED PIPE)

    This creates a shared communication channel between processes.

    If it already exists, we reuse it.
    */

    if (mkfifo(loggerFifoFilePath, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            perror("Logger Error: Failed to create FIFO");
            return 1;
        }
    }

    printf("Logger: FIFO initialized at %s\n", loggerFifoFilePath);
    printf("Logger: Waiting for incoming log messages...\n\n");

    /*
    STEP 2 — CONTINUOUSLY LISTEN FOR MESSAGES

    The logger runs indefinitely so it can capture logs at any time.
    */

    while (1)
    {

        /*

        STEP 3 — OPEN FIFO FOR READING


        This blocks until a module opens the FIFO for writing.
        */

        int fifoReadFileDescriptor = open(loggerFifoFilePath, O_RDONLY);

        if (fifoReadFileDescriptor == -1)
        {
            perror("Logger Error: Failed to open FIFO for reading");
            return 1;
        }

        /*

        STEP 4 — READ MESSAGES FROM FIFO


        Modules send messages using write().
        Logger receives them using read().
        */

        while ((numberOfBytesReceivedFromFifo =
                    read(fifoReadFileDescriptor,
                         incomingLogMessageBuffer,
                         sizeof(incomingLogMessageBuffer) - 1)) > 0)
        {

            // Ensure proper null-terminated string
            incomingLogMessageBuffer[numberOfBytesReceivedFromFifo] = '\0';

            // Remove newline for clean formatting
            incomingLogMessageBuffer[strcspn(incomingLogMessageBuffer, "\n")] = '\0';

            /*

            STEP 5 — OPEN LOG FILE

            */

            FILE *logFileWritePointer = fopen(logOutputFilePath, "a");

            if (logFileWritePointer == NULL)
            {
                perror("Logger Error: Failed to open log file");
                close(fifoReadFileDescriptor);
                return 1;
            }

            /*

            STEP 6 — GENERATE TIMESTAMP

            */

            time_t currentSystemTime = time(NULL);
            struct tm *localTimeStructurePointer = localtime(&currentSystemTime);

            char formattedTimestampString[100];

            strftime(formattedTimestampString,
                     sizeof(formattedTimestampString),
                     "%Y-%m-%d %H:%M:%S",
                     localTimeStructurePointer);

            /*

            STEP 7 — WRITE LOG ENTRY

            */

            fprintf(logFileWritePointer,
                    "[%s] %s\n",
                    formattedTimestampString,
                    incomingLogMessageBuffer);

            fclose(logFileWritePointer);

            /*

            STEP 8 — DISPLAY IN TERMINAL (FOR DEMO)

            */

            printf("Logger: Recorded log entry -> %s\n",
                   incomingLogMessageBuffer);

            // Clear buffer before next message
            memset(incomingLogMessageBuffer, 0,
                   sizeof(incomingLogMessageBuffer));
        }

        // Close FIFO after reading batch
        close(fifoReadFileDescriptor);
    }

    return 0;
}