#include "kernel/types.h"
#include "user.h"
#define SIZE 5000

int
main(int argc, char *argv[])
{
    int unmap = 1; // Set to 1 to unmap shared memory, 0 to keep it mapped
    char* shmem_text = (char*) malloc(SIZE);
    strcpy(shmem_text, "Hello from shared memory!");
    printf("%s\n", shmem_text);
    int parent_pid = getpid();
    if(fork() == 0) {
        printf("1. Child process size before shared mapping: %d\n", sbrk(0));
        char* va = map_shared_pages(parent_pid, getpid(),shmem_text, SIZE);
        if (va == (char*)-1) {
            printf("Error mapping shared memory\n");
            exit(1);
        }
        printf("2. Child process size after shared mapping: %d\n",sbrk(0));
        strcpy(va, "Hello daddy");
        if (unmap){
            if (unmap_shared_pages(getpid(), va, SIZE) < 0) {
                printf("Error unmapping shared memory\n");
                exit(1);
            }
            printf("3. Child process size after unmapping: %d\n",sbrk(0));
            char* buffer = (char*)malloc(30*SIZE);
            printf("4. Child process size after malloc: %d\n",sbrk(0));
            strcpy(buffer, "Saoko puppi saoko");
            printf("Child process text: %s\n", buffer);
        }
    }
    else{
        sleep(5);
        printf("Parent process text : %s\n", shmem_text);
        printf("Parent process size: %d\n", sbrk(0));
    }
    
    exit(0);
}
    