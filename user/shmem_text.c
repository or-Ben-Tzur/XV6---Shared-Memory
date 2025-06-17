#include "user.h"

int
main(int argc, char *argv[])
{
    char shmem_text[30];
    strcpy(shmem_text, "Hello from shared memory!");
    printf("%s\n", shmem_text);
    int parent_pid = getpid();
    if(fork() == 0) {
        printf("1. Child process size before shared mapping: %d\n", );
        int va= map_shared_pages(parent_pid, getpid(), (int)shmem_text, sizeof(shmem_text));
        printf("2. Child process size after shared mapping: %d\n",);
        strcpy(va, "Hello daddy");
        if(unmap_shared_pages(getpid(), va, sizeof(shmem_text))<0){
            printf("Error unmapping shared memory\n");
            exit(1);
        }
        printf("3. Child process size after delete shared mapping: %d\n",);
        malloc(100);
        printf("4. Child process size after malloc: %d\n",);
        
    }
    else{
        sleep(5);
        printf("Parent process text : %s\n", shmem_text);
        printf("Parent process size: %d\n", );}
    
    // Exit the program
    exit(0);
}
    