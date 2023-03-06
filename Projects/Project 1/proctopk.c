#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/fcntl.h>


struct freqTable{

    char word [64];
    int frequency;
};

// Definitions
#define SNAME "shmname"
#define MAX_LETTERS 64 

// Function Declaration
void findKMostWords(char fileName[]);

// Program Start

int main(int argc, char *argv[])
{
    int numberOfFiles = 0;
    int kMostWords = 0;
   
    
    if (argc > 1) {
        kMostWords = atoi(argv[1]);
        numberOfFiles = atoi(argv[3]);
    }

    // File name table
    int fileNames[numberOfFiles];
    for (int i = 0; i < numberOfFiles; i++){
            fileNames[i] = argv[i + 4];
    }


    // Shared memory is created by the parent process
    const int SIZE = 4096;
    int shm_fd;
    void *ptr;
    shm_fd = shm_open(SNAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd,SIZE);
    ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED) {
        printf("Map failed\n");
        return -1;
    }

    // Create N child processes

    for(int i = 0; i < numberOfFiles; i++) {
        if(fork() == 0) {

            // Find the k-most occuring words
            exit(0);
        }
    }

    

    return 0;
}


void findKMostWords(char fileName[]) {

    FILE* fp = fopen("test.txt", "r");
    char buffer[1000];
    struct freqTable table [1000];
    int cnt = 0;
    while (fgets(buffer, 1000, fp)) {

        // Gets a single line from the file
        char oneLine[strlen(buffer)];
        strcpy(oneLine, buffer);
        oneLine[strcspn(oneLine, "\n\r")] = '\0'; // Removes next line characters

        printf("%s%s%s", "The line is: ", oneLine, "\n");

        // Converts the characters to upper case.
        for (int i = 0; i < strlen(oneLine); i++) {

            oneLine[i] = toupper(oneLine[i]);
        }

        // Takes a single word from the line
        char* singleWord;
        singleWord = strtok(oneLine, " ");

        while(singleWord != NULL) {

            printf("%s%s%s", "The word is: " , singleWord, "\n");
            int found = 0;

            for (int i = 0; i < cnt; i++) {

                // If the word is present, increase the frequency
                if(!strcmp(table[i].word,singleWord)) {

                    table[i].frequency++;
                    found = 1;
                    break;

                }       
            }

            if(!found){

                // If the word is not present, create a new entry and increment the counter.   
                strcpy(table[cnt].word,singleWord);
                table[cnt].frequency = 1;
                cnt++;
            } 

            // Get the next word
            singleWord = strtok(NULL, " ");
            
        }
         
    }

    for (int i = 0; i < cnt; i++) {

        char w[strlen(table[i].word)];
        strcpy(w, table[i].word);
        printf("%s%s%s", "Word: ", w, " Frequency: ");
        printf("%d", table[i].frequency);
        printf("\n");

    }
    fclose(fp);

    
}