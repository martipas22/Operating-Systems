#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int sigint_pressed = 0;
void handler(int s){
    sigint_pressed +=1;
}

int main(int argc, char* argv[]){
    signal(SIGUSR1, handler);
    while (sigint_pressed == 0){
        sleep(1);
        printf("My PID is: %d\n", getpid());
    }
   return 0;
}




