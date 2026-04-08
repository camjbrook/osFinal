#include <stdio.h>
#include <fcntl.h>      // open
#include <unistd.h>     // write, close
#include <string.h>     // strlen
#include <stdlib.h>     // exit

/*
    Module 5 - Amdahl's Law
    -----------------------
    This program:
    1. Accepts the parallel fraction (0 to 1)
    2. Accepts the number of cores
    3. Calculates speedup using Amdahl's Law
    4. Optionally compares serial vs parallel execution
    5. Sends important events to the logger
*/

/* ---------- LOGGER ---------- */
void sendLogMessage(const char *logMessage)
{
    const char *loggerFifoPath = "/tmp/logger_fifo";

    int fifoFileDescriptor = open(loggerFifoPath, O_WRONLY | O_NONBLOCK);

    if (fifoFileDescriptor == -1)
    {
        perror("Failed to open logger FIFO");
        return;
    }

    write(fifoFileDescriptor, logMessage, strlen(logMessage));
    write(fifoFileDescriptor, "\n", 1);   // add newline so logger formats nicely
    close(fifoFileDescriptor);
}

/*
    Function to calculate speedup using Amdahl's Law

    Formula:
    Speedup = 1 / ((1 - p) + (p / n))
*/
double calculateSpeedup(double parallelFraction, int numberOfCores)
{
    return 1.0 / ((1.0 - parallelFraction) + (parallelFraction / numberOfCores));
}

int main()
{
    double parallelFraction;
    int numberOfCores;
    char compareChoice;

    printf("=== Amdahl's Law Calculator ===\n\n");
    sendLogMessage("Amdahl: Program started");

    /* Ask user for the parallel fraction */
    printf("Enter the parallel fraction (0 to 1): ");
    scanf("%lf", &parallelFraction);

    sendLogMessage("Amdahl: User entered parallel fraction");

    /* Validate parallel fraction */
    if (parallelFraction < 0.0 || parallelFraction > 1.0)
    {
        printf("Error: Parallel fraction must be between 0 and 1.\n");
        sendLogMessage("Amdahl: Invalid parallel fraction entered");
        return 1;
    }

    /* Ask user for number of cores */
    printf("Enter the number of cores: ");
    scanf("%d", &numberOfCores);

    sendLogMessage("Amdahl: User entered number of cores");

    /* Validate number of cores */
    if (numberOfCores <= 0)
    {
        printf("Error: Number of cores must be greater than 0.\n");
        sendLogMessage("Amdahl: Invalid number of cores entered");
        return 1;
    }

    /* Calculate speedup */
    double speedup = calculateSpeedup(parallelFraction, numberOfCores);

    printf("\n--- Results ---\n");
    printf("Parallel fraction: %.2f\n", parallelFraction);
    printf("Number of cores: %d\n", numberOfCores);
    printf("Calculated speedup: %.4f\n", speedup);

    sendLogMessage("Amdahl: Calculated speedup");

    /* Ask whether user wants comparison */
    printf("\nWould you like to compare serial vs parallel execution? (y/n): ");
    scanf(" %c", &compareChoice);

    if (compareChoice == 'y' || compareChoice == 'Y')
    {
        double serialTime = 1.0;
        double parallelTime = 1.0 / speedup;

        printf("\n--- Serial vs Parallel Comparison ---\n");
        printf("Serial execution time: %.4f unit(s)\n", serialTime);
        printf("Parallel execution time: %.4f unit(s)\n", parallelTime);
        printf("Speedup: %.4f x faster\n", speedup);

        sendLogMessage("Amdahl: Compared serial vs parallel execution");
    }
    else
    {
        sendLogMessage("Amdahl: User skipped serial vs parallel comparison");
    }

    sendLogMessage("Amdahl: Program finished");
    return 0;
}