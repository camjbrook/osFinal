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

    Paths are set to "../" because boot.sh executes this from the /bin folder.
    */

    // 1. Path to the FIFO pipe (lives in the project root)
    const char *loggerFifoFilePath = "../logger_fifo";

    // 2. Path to the log file (lives in the /logs folder)
    const char *logOutputFilePath = "../logs/logs.txt";

    char incomingLogMessageBuffer[1024];
    ssize_t numberOfBytesReceivedFromFifo;

    /*
    STEP 1 — CREATE FIFO (NAMED PIPE)
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
    */
    while (1)
    {
        // Open FIFO for reading. This blocks until a writer connects.
        int fifoReadFileDescriptor = open(loggerFifoFilePath, O_RDONLY);

        if (fifoReadFileDescriptor == -1)
        {
            perror("Logger Error: Failed to open FIFO for reading");
            return 1;
        }

        while ((numberOfBytesReceivedFromFifo =
                    read(fifoReadFileDescriptor,
                         incomingLogMessageBuffer,
                         sizeof(incomingLogMessageBuffer) - 1)) > 0)
        {
            // Null-terminate and clean newline
            incomingLogMessageBuffer[numberOfBytesReceivedFromFifo] = '\0';
            incomingLogMessageBuffer[strcspn(incomingLogMessageBuffer, "\n")] = '\0';

            /*
            STEP 3 — WRITE TO LOG FILE
            */
            FILE *logFileWritePointer = fopen(logOutputFilePath, "a");

            if (logFileWritePointer == NULL)
            {
                // If this fails, it's usually because the /logs directory doesn't exist
                perror("Logger Error: Failed to open log file");
                close(fifoReadFileDescriptor);
                return 1;
            }

            // Timestamp generation
            time_t currentSystemTime = time(NULL);
            struct tm *localTimeStructurePointer = localtime(&currentSystemTime);
            char formattedTimestampString[100];

            strftime(formattedTimestampString,
                     sizeof(formattedTimestampString),
                     "%Y-%m-%d %H:%M:%S",
                     localTimeStructurePointer);

            // Write and Close
            fprintf(logFileWritePointer, "[%s] %s\n",
                    formattedTimestampString,
                    incomingLogMessageBuffer);
            fclose(logFileWritePointer);

            printf("Logger: Recorded log entry -> %s\n", incomingLogMessageBuffer);

            memset(incomingLogMessageBuffer, 0, sizeof(incomingLogMessageBuffer));
        }

        // Close and re-open on next writer connection
        close(fifoReadFileDescriptor);
    }

    return 0;
}