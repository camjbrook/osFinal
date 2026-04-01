//@camjbrook
#include <stdio.h>
#include <fcntl.h>  //open()
#include <stdlib.h> //system() (for clearing cli)

void createFile()
{
    int masterFile;
    char userFile[100];

    // linux "touch" sim
    system("clear");
    printf("--Create File--\n");
    printf("touch ");
    scanf("%s", &userFile);

    masterFile = open(userFile, O_CREAT | O_RDWR, 00700);

}

int main()
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

    // list case
    case 3:

    // read case
    case 4:
    }

    return 0;
}