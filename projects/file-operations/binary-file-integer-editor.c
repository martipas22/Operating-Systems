#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>


int allocateSpace(const char *filename, int position, int nBytes) {
    int fd = open(filename, O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    int original_size = lseek(fd, 0, SEEK_END);
    int new_size = original_size + nBytes;
    if (ftruncate(fd, new_size) == -1) {
        perror("ftruncate");
        close(fd);
        return -1;
    }

    for (int current = original_size - 1; current >= position; current--) {
        lseek(fd, current, SEEK_SET);
        char byte;
        read(fd, &byte, 1);
        lseek(fd, current + nBytes, SEEK_SET);
        write(fd, &byte, 1);
    }

    close(fd);
    return 0;
}

int removeSpaceFile(const char *filename, int position, int nBytes) {
    int fd = open(filename, O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    int original_size = lseek(fd, 0, SEEK_END);
    int new_size = original_size - nBytes;

    for (int current = position + nBytes; current < original_size; current++) {
        lseek(fd, current, SEEK_SET);
        char byte;
        read(fd, &byte, 1);
        lseek(fd, current - nBytes, SEEK_SET);
        write(fd, &byte, 1);
    }

    if (ftruncate(fd, new_size) == -1) {
        perror("ftruncate");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int modifyIntBinary(const char *filename, int offset, int value) {
    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    if (lseek(fd, offset * sizeof(int), SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        return -1;
    }

    if (write(fd, &value, sizeof(int)) != sizeof(int)) {
        perror("write");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int modifyIntText(const char *filename, int offset, int value) {
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }

    char buffer[256];
    int current_offset = 0;
    long start_of_number, end_of_number;

    while (fscanf(file, "%c,", buffer) != EOF) {
        if (current_offset == offset) {
            start_of_number = ftell(file) - strlen(buffer) - 1;
            end_of_number = ftell(file);
            break;
        }
        current_offset++;
    }

    if (current_offset != offset) {
        fprintf(stderr, "Offset out of range.\n");
        fclose(file);
        return -1;
    }

    char new_value[32];
    sprintf(new_value, "%d,", value); 
    int new_digits = strlen(new_value);
    int old_digits = end_of_number - start_of_number;

    if (new_digits > old_digits) {
        allocateSpace(filename, end_of_number, new_digits - old_digits);
    } else if (new_digits < old_digits) {
        removeSpaceFile(filename, end_of_number - (old_digits - new_digits), old_digits - new_digits);
    }
    
    fseek(file, start_of_number, SEEK_SET);
    fwrite(new_value, 1, new_digits, file);

    fclose(file);
    return 0;
}



int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <filename> <offset> <value>\n", argv[0]);
        return 1;
    }
    
    const char *filename = argv[1];
    int offset = atoi(argv[2]) - 1;
    int value = atoi(argv[3]);

    if (strstr(argv[1], ".bin")) {
        if (modifyIntBinary(filename, offset, value) != 0) {
            fprintf(stderr, "Error modifying binary file.\n");
            return 1;
        }
    } else {
        if (modifyIntText(filename, offset, value) != 0) {
            fprintf(stderr, "Error modifying text file.\n");
            return 1;
        }
    }
    printf("File modified successfully.\n");
    return 0;
}



 