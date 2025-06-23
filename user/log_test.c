#include "kernel/types.h"
#include "user.h"
#define CHILD_PROCESSES 4
#define PGSIZE 4096
#define BUFF_SIZE 4096

int main(int argc, char *argv[]) {
    char* buffer = malloc(BUFF_SIZE);
    char* va=0;
    if (!buffer) {
        printf("malloc failed\n");
        exit(1);
    }

    memset(buffer, 0, BUFF_SIZE);
    int parent_pid = getpid();
    int index = -1;
    for (int i = 0; i < CHILD_PROCESSES; i++) {
        int pid = fork();
        if (pid < 0) {
            printf("fork failed\n");
            exit(1);
        } else if (pid == 0) { //child
            index = i;
            va = map_shared_pages(parent_pid, getpid(), buffer, BUFF_SIZE);
            if (va == (char*) -1) {
                printf("child %d failed to map\n", i);
                exit(1);
            }
            break;
        }
    }

    if (index == -1) { // parent
        char* addr = buffer;
        int header=0;
        int idx=0;
        int msg_len=0;
        do{
            header = *(int*)addr;
            if (header == 0){
                sleep(1); // No more messages
                continue;
            } 
            idx = header & 0xFFFF;
            msg_len = (header >> 16) & 0xFFFF;
  
            if ((addr - buffer) + 4 + msg_len > BUFF_SIZE) break; // prevent overflow

            printf("Message from child %d: ", idx);
            for (int i = 0; i < msg_len; i++) {
                printf("%c", addr[4 + i]);
            }
            printf("\n");

            addr = addr + 4 + msg_len;
            addr = (char*)(((uint64)(addr + 3)) & ~3); // 4-byte alignment

        } while ((addr - buffer) + 4 + msg_len <= BUFF_SIZE);
        
        sleep(3); // wait for children to finish writing
        for (int i = 0; i < CHILD_PROCESSES; i++) {
            wait(0);
        }
        printf("All messages printed\n");
        

    } else { //child

        char* addr = va;
        int msg_len = index +1;
        while ((addr - va) + 4 + msg_len < BUFF_SIZE) {
            if (__sync_val_compare_and_swap((int*)addr, 0, (index & 0xFFFF) | (msg_len << 16)) == 0) {
                char ch = 'a' + index;
                for (int i = 0; i < msg_len; i++) {
                    addr[4 + i] = ch;
                }
            }
            int header = *(int*)addr;
            int existing_len = (header >> 16) & 0xFFFF;
            addr += 4 + existing_len;
            addr = (char*)(((uint64)(addr + 3)) & ~3); // align
            sleep(1);
        }
        
        sleep(index*2);
        printf("Child %d finished writing messages\n", index);
    }
    exit(0);
}
