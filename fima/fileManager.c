//@camjbrook
#include <stdio.h>
#include <fcntl.h>  //open()
#include <stdlib.h> //system() (for clearing cli)
#include <string.h> //need this so bad for nothing but aesthetics
#include <unistd.h> //close()

void createFile()
{
    int masterFile;
    char userFile[100];

    // linux "touch" sim
    system("clear");
    printf("--Create File--\n");
    printf("touch ");
    scanf("%s", &userFile);

    // save to "Documents" folder
    char path[300] = "Documents/";
    strcat(path, userFile);
    printf("Saved to Documents\n");

    // allows operator permissions to user
    masterFile = open(path, O_CREAT | O_RDWR, 00700);
    close(masterFile);
}

void deleteFile()
{
    system("clear");
    printf("--Delete File--\n");
}

int main()
{
    while (1)
    {
        int inp;
        // main menu
        system("clear");
        printf("--File Manager Simulator--\n");
        printf(
            "Options:\n"
            "(0) Exit\n"
            "(1) Create file\n"
            "(2) Delete file\n"
            "(3) List files\n"
            "(4) Read file\n"
            "Enter your choice: ");

        // user input
        scanf("%d", &inp);

        switch (inp)
        {
        // exit case
        case 0:
            printf("exiting...\n");
            return 0;
        // create case
        case 1:
            createFile();
        // delete case
        case 2:

        // list case
        case 3:

        // read case
        case 4:
        }
    }
}