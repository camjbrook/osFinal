//@camjbrook
#include <stdio.h>
#include <fcntl.h>  //open()
#include <stdlib.h> //system() (for clearing cli)
#include <string.h> //need this so bad for nothing but aesthetics
#include <unistd.h> //close() unlink()

// global variables
int masterFile;
char path[300] = "Documents/";

void createFile()
{

    char userFile[100];

    // linux "touch" sim
    system("clear");
    printf("--Create File--\n");
    printf("touch ");
    scanf("%s", userFile);

    // create path to document folder
    strcpy(path, "Documents/");
    strcat(path, userFile);

    // allows operator permissions to user
    masterFile = open(path, O_CREAT | O_RDWR, 00700);

    if(masterFile == -1){
        printf("error opening file");
        return;
    }
    close(masterFile);
    printf("Saved to Documents\n");
}

void deleteFile()
{
    char delFile[100];

    system("clear");
    printf("--Delete File--\n");
    printf("rm ");
    scanf("%s", &delFile);

    //create path to document folder
    strcpy(path, "Documents/");
    strcat(path, delFile);

    //delete document
    unlink(path);
    
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
            break;
        // delete case
        case 2:
            deleteFile();
            break;
        // list case
        case 3:

        // read case
        case 4:
        }
    }
}