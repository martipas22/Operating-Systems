#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int num = atoi(argv[1]);
    int fahtertochild[2];
    int childtofather[2];

    pipe(fahtertochild);
    pipe(childtofather);

    int n = fork();

    if (n == 0){
        close(childtofather[0]);
        close(fahtertochild[1]);
        int recivednum;
        read(fahtertochild[0], &recivednum, sizeof(recivednum));
        int squarednum = recivednum * recivednum;
        write(childtofather[1], &squarednum, sizeof(squarednum));
        exit(0);
    }else{
        close(childtofather[1]);
        close(fahtertochild[0]);
        int num = atoi(argv[1]);
        write(fahtertochild[1], &num, sizeof(num));
        wait(NULL);
        int squarednumfather;
        read(childtofather[0], &squarednumfather, sizeof(squarednumfather));
        printf("Cuadrat: %d", squarednumfather);
    }
    return 0;
}