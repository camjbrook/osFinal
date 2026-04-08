//@camjbrook A00473064
#include <stdio.h>
#include <fcntl.h>  //open()
#include <stdlib.h> //system() (for clearing cli)
#include <string.h> //need this so bad for nothing but aesthetics
#include <unistd.h> //close() unlink()
#include <dirent.h> //directory search
#include <sys/stat.h>

/* ... (existing includes) ... */
void sendLogMessage(const char *logMessage)
{
    const char *loggerFifoPath = "/tmp/logger_fifo";
    int fd = open(loggerFifoPath, O_WRONLY | O_NONBLOCK);
    if (fd != -1)
    {
        write(fd, logMessage, strlen(logMessage));
        write(fd, "\n", 1);
        close(fd);
    }
}

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

    if (masterFile == -1)
    {
        printf("error opening file\n");
        return;
    }

    close(masterFile);
    printf("Saved to Documents\n");
    sendLogMessage("FileManager: Created file");
}

void deleteFile()
{
    char delFile[100];

    system("clear");
    printf("--Delete File--\n");
    printf("rm ");
    scanf("%s", delFile);

    // create path to document folder
    strcpy(path, "Documents/");
    strcat(path, delFile);

    // delete document
    unlink(path);
    sendLogMessage("FileManager: Deleted file");
}

void listFile()
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir("./Documents"); // open current directory
    sendLogMessage("FileManager: Listed files");
    if (dir == NULL)
    {
        perror("opendir failed");
    }

    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    printf("Press Enter to continue...");
    while (getchar() != '\n')
        ;      // clear buffer
    getchar(); // wait for Enter

    closedir(dir);
}

void readFiles()
{
    DIR *dir;
    struct dirent *entry;

    char files[100][100]; // store filenames
    int count = 0;
    int choice;

    dir = opendir("./Documents");
    if (dir == NULL)
    {
        perror("opendir failed");
        return;
    }

    system("clear");
    printf("--Read File--\n");

    // list + store files
    while ((entry = readdir(dir)) != NULL && count < 100)
    {
        // skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        strcpy(files[count], entry->d_name);
        printf("%d: %s\n", count, files[count]);
        count++;
    }

    closedir(dir);

    if (count == 0)
    {
        printf("No files found.\n");
        return;
    }

    printf("Select file #: ");
    scanf("%d", &choice);

    if (choice < 0 || choice >= count)
    {
        printf("Invalid choice\n");
        return;
    }

    // build full path
    char fullPath[300];
    strcpy(fullPath, "Documents/");
    strcat(fullPath, files[choice]);

    FILE *fp = fopen(fullPath, "r");
    if (!fp)
    {
        perror("fopen failed");
        return;
    }

    char buffer[256];

    printf("\n--- Contents of %s ---\n\n", files[choice]);

    while (fgets(buffer, sizeof(buffer), fp))
    {
        printf("%s", buffer);
    }

    fclose(fp);

    printf("\n\nPress Enter to continue...");
    while (getchar() != '\n')
        ;
    getchar();

    sendLogMessage("FileManager: Read file");
}

int main()
{
    sendLogMessage("FileManager: Module started");
    while (1)
    {
        int inp;
        // main menu
        system("clear");
        printf("--File Manager Sim--\n\n");
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
            sendLogMessage("FileManager: Module exited");
            printf("\nPress Enter to exit...");
            while (getchar() != '\n')
                ;
            getchar();
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
            listFile();
            break;

        // read case
        case 4:
            readFiles();
            break;
        }
    }
}
