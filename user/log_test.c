#include "user.h"
#define CHILD_PROCESSES 4
#define PGSIZE 4096
#define BUFF_SIZE 1024
int
main(int argc, char *argv[])
{
    int *start = (int*)malloc(sizeof(int));
    *start = 0;
    void* addrs[CHILD_PROCESSES];
    void* shared_buf = malloc(BUFF_SIZE);
    void* start_va;
    void* addrs_va;
    char* addr;
    void* buffer;
    int msg_len;
    
    int pid, index;
    int parent_pid = getpid();
    if (shared_buf == 0) {
        printf("failed to malloc\n");
        exit(1);
    }
    for(int i=0;i<CHILD_PROCESSES;i++){
        memset(addrs[i], 0, sizeof(void*));
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
            start_va = map_shared_pages(parent_pid,getpid(), start, sizeof(int));
            if (start_va == (void*)-1) {
                printf("Child process failed to map shared pages\n");
                exit(1);
            }
            addrs_va = map_shared_pages(parent_pid, getpid(), addrs, sizeof(void*) * CHILD_PROCESSES);
            if (addrs_va == (void*)-1) {
                printf("Child process failed to map shared pages for addresses\n");
                exit(1);
            }
            break; // exit the loop in the child process
        } else { // parent process
            addrs[i] = map_shared_pages(parent_pid ,pid, shared_buf, BUFF_SIZE);
            if (addrs[i] == (void*)-1) {
                printf("Parent process failed to map shared pages for child %d\n", i);
                exit(1);
            }
        }
    }

    if (index == -1) { // parent process
        *start = 1;
        
    } else { // child process
        while (*start == 0) {
            // wait for the parent to set start
            sleep(0.5);
        }
        printf("Child %d started with address %p\n", index, addrs[index]);
    }
    
    if (index == -1){
        // TODO wait for all child processes to finish
    } else {
        buffer = (void*)addrs[index];
        addr = (char*)buffer;
        msg_len = 1 + index;
        while ((void*)(addr + 4 + msg_len) <= (void*)(buffer + BUFF_SIZE)) {
            // Try to write header atomically if slot is empty (0)
            if (__sync_val_compare_and_swap((int*)addr, 0, (index & 0xFFFF) | (msg_len << 16)) == 0) {
                // Header written successfully. Now write message
                char ch = 'a' + index;  // message character
                for (int i = 0; i < msg_len; i++) {
                    addr[4 + i] = ch;
                }
            } 
            // Slot was taken — read the existing header
            int existing = *(int*)addr;
            int existing_msg_len = (existing >> 16) & 0xFFFF;

            // Skip to the next slot past this message
            addr += 4 + existing_msg_len;
            addr = (char*)((addr + 3) & ~3);  // align again
        }

    }

}
    