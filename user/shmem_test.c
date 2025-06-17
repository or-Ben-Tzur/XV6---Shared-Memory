#include "kernel/types.h"
#include "user.h"


int
main(int argc, char *argv[])
{
    char shmem_text[30];
    strcpy(shmem_text, "Hello from shared memory!");
    printf("%s\n", shmem_text);
    int parent_pid = getpid();
    if(fork() == 0) {
        printf("1. Child process size before shared mapping: %d\n", sbrk(0));
        char* va = map_shared_pages(parent_pid, getpid(), shmem_text, sizeof(shmem_text));
        printf("2. Child process size after shared mapping: %d\n",sbrk(0));
        strcpy(va, "Hello daddy");
        if (unmap_shared_pages(getpid(), va, sizeof(shmem_text)) < 0) {
            printf("Error unmapping shared memory\n");
            exit(1);
        }
        printf("3. Child process size after delete shared mapping: %d\n",sbrk(0));
        malloc(100);
        printf("4. Child process size after malloc: %d\n",sbrk(0));
        
    }
    else{
        sleep(5);
        printf("Parent process text : %s\n", shmem_text);
        printf("Parent process size: %d\n", sbrk(0));}
    
    // Exit the program
    exit(0);
}
    