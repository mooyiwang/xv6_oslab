#include "kernel/types.h"
#include "user.h"

int main(int argc,char* argv[]){
    if(argc != 1){
        fprintf(2, "error input! there is no args in pingpong\n");
        exit(-1);
    }

    int parent_to_child[2];
    int child_to_parent[2];
    pipe(parent_to_child);
    pipe(child_to_parent);

    if(fork() == 0){
        //child process
        int child_pid = getpid();
        char from_parent[4];
        close(parent_to_child[1]);
        read(parent_to_child[0], from_parent, 4);
        close(parent_to_child[0]);
        fprintf(1, "%d: received %s\n", child_pid, from_parent);
        close(child_to_parent[0]);
        write(child_to_parent[1], "pong", 4);
        close(child_to_parent[1]);
        exit(0);
    }
    else{
        int parent_pid = getpid();
        char *from_child[4];
        close(parent_to_child[0]);
        write(parent_to_child[1], "ping", 4);
        close(parent_to_child[1]);
        close(child_to_parent[1]);
        read(child_to_parent[0], from_child, 4);
        close(child_to_parent[0]);
        fprintf(1, "%d: received %s\n", parent_pid, from_child);
        wait(0);
        exit(0);
    }
}