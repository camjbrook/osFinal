//@camjbrook A00473064
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // fork(), execv()
#include <sys/wait.h> // wait()

void launchModule(char *path)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return;
    }
    else if (pid == 0)
    {
        // child: replace process image with the module
        char *args[] = {path, NULL};
        execv(path, args);

        // if execv returns, something went wrong
        perror("execv failed");
        exit(1);
    }
    else
    {
        // parent: wait for child to finish before returning to menu
        int status;
        wait(&status);
    }
}

int main()
{
    int inp;
    system("clear");
    while (1)
    {
        printf("--OS Final Project -- Main Menu--\n\n");
        printf(
            "Options:\n"
            "(0) Exit\n"
            "(1) Logger\n"
            "(2) Peterson's Solution\n"
            "(3) File Management\n"
            "(4) Memory Allocation\n"
            "(5) Amdahl's Law\n\n"
            "Enter your choice: ");

        if (scanf("%d", &inp) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("Invalid input. Press Enter to continue...\n");
            getchar();
            continue;
        }

        if (inp < 0 || inp > 5)
        {
            printf("Invalid choice. Press Enter to continue...\n");
            getchar();
            getchar();
            continue;
        }

        switch (inp)
        {
        case 0:
            printf("Exiting...\n");
            return 0;
        case 1:
            launchModule("./logger");
            break;
        case 2:
            launchModule("./peterson");
            break;
        case 3:
            launchModule("./fileManager");
            break;
        case 4:
            launchModule("./memory");
            break;
        case 5:
            launchModule("./amdahl");
            break;
        }
    }

    return 0;
}