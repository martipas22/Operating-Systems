#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>


void convertToBinary(int fd, int fdWrite) {
    int endOfFile = 0;
    char buffer[1];
    char numberBuffer[100];
    int index = 0;

    while (!endOfFile) {
        while (1) {
            if (read(fd, buffer, 1) == 0) {
                endOfFile = 1;
                numberBuffer[index] = '\0';
                break;
            }

            if (buffer[0] == ',') {
                numberBuffer[index] = '\0';
                break;
            }

            numberBuffer[index++] = buffer[0];
        }

        if (index > 0) {
            int number = atoi(numberBuffer);
            write(fdWrite, &number, sizeof(int));
        }

        index = 0;
    }
}

void convertToText(int fd, int fdWrite) {
    int number;
    int firstNumber = 1;
    char buffer[100];

    while (read(fd, &number, sizeof(int)) > 0) {
        if (!firstNumber) {
            write(fdWrite, ",", 1);
        }
        sprintf(buffer, "%d", number);
        write(fdWrite, buffer, strlen(buffer));
        firstNumber = 0;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY); 
    if (fd == -1) {
        perror("Error opening input file");
        return 1;
    }

    int fdWrite = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644); 
    if (fdWrite == -1) {
        perror("Error opening output file");
        close(fd);
        return 1;
    }

    
    if (strstr(argv[1], ".bin")) {
        convertToText(fd, fdWrite);
    } else {
        convertToBinary(fd, fdWrite);
    }

    close(fd);
    close(fdWrite);

    return 0;
}








