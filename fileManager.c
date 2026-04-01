#include <stdio.h>

void createFile()
{
    
}

//@camjbrook
int main()
{
    int inp;
    // main menu
    printf("--File Manager Simulator--\n");
    printf(
        "Options:\n"
        "(0) Exit\n"
        "(1) Create file\n"
        "(2) Delete file\n"
        "(3) List files\n"
        "(4) Read file\n"
        "Enter your choice: ");

    scanf("%d", &inp);

    switch (inp)
    {
    // exit case
    case 0:
        printf("exiting...");
        return 0;
    // create file
    case 1:
    createFile();

    // delete dile
    case 2:

    // list files
    case 3:

    // read fioles
    case 4:
    }

    return 0;
}