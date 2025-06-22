#include "user.h"
#define CHILD_PROCESSES 4
#define PGSIZE 4096
#define BUFF_SIZE 1024
int
main(int argc, char *argv[])
{
    int *start = (int*)malloc(sizeof(int));
    *start = 0;
    void* child_va[CHILD_PROCESSES];
    void* shared_buf = malloc(BUFF_SIZE);
    
    int pid, index;
    int parent_pid = getpid();
    void* va;
    if (shared_buf == 0) {
        printf("failed to malloc\n");
        exit(1);
    }
    for(int i=0;i<CHILD_PROCESSES;i++){
        memset(child_va[i], 0, sizeof(void*));
    }
    memset(shared_buf, 0, BUFF_SIZE);

    index = -1; //for parent process
    for(int i = 0; i < CHILD_PROCESSES; i++) {
        pid = fork();
        if (pid < 0) {
            printf("fork failed\n");
            exit(1);
        } else if (pid == 0) { // child process
            index = i;

            break; // exit the loop in the child process
        } else { // parent process
            va = map_shared_pages(pid, shared_buf, BUFF_SIZE);
            if ( va < 0) {
                printf("Parent process failed to map shared pages\n");
                exit(1);
            }
            child_va[i] = va; // store the mapped address for the child
        }
    }
    void* mapped= map_shared_pages(parent_pid, getpid(), (int)shared_buf, sizeof(shared_buf));
    while(addr<buffer+sizeof(buffer) ){
            if(__sync_val_compare_and_swap(addr, 0,index)==0){

            }
            addr = (addr + 3) & ~3;
        }

    exit(0);
}
    