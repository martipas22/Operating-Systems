#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int num = atoi(argv[1]);
    kill(num, SIGUSR1);
    return 0;
}


