#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int fathertochild[2];
    int childtofather[2];

    pipe(fathertochild);
    pipe(childtofather);

    int n = fork();

    if (n == 0) {
        close(fathertochild[1]);
        close(childtofather[0]);

        int num;
        while (read(fathertochild[0], &num, sizeof(int)) > 0) {
            if (num % 2 == 0) {
                write(childtofather[1], &num, sizeof(int));
            }
        }

        close(fathertochild[0]);
        close(childtofather[1]);
        exit(0);

    } else {
        close(fathertochild[0]);
        close(childtofather[1]);

        for (int i = 1; i < argc; i++) {
            int num = atoi(argv[i]);
            write(fathertochild[1], &num, sizeof(int));
        }

        close(fathertochild[1]);
        wait(NULL);

        int even;
        while (read(childtofather[0], &even, sizeof(int)) > 0) {
            printf("Even number: %d\n", even);
        }

        close(childtofather[0]);
        _exit(0);
    }
}
