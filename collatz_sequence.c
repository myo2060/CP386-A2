#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_ELEMENTS 100
#define SHARED_MEMORY_NAME "/collatz_shared_memory"

int load_initial_numbers(const char file_path, int values[]) {
    FILEfile_handle = fopen(file_path, "r");
    if (!file_handle) {
        perror("File opening error");
        return -1;
    }

    int index = 0;
    while (fscanf(file_handle, "%d", &values[index]) != EOF) {
        index++;
    }
    fclose(file_handle);
    return index;
}

void compute_collatz(int number, int shared_memory) {
    int count = 0;
    while (number != 1) {
        shared_memory[count++] = number;
        if (number % 2 == 0) {
            number /= 2;
        } else {
            number = (3 number) + 1;
        }
    }
    shared_memory[count] = 1;
}

int main() {
    int values[MAX_ELEMENTS];
    int total_numbers = load_initial_numbers("start_numbers.txt", values);

    if (total_numbers == -1) return 1;

    int shared_memory_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    if (shared_memory_fd == -1) {
        perror("Shared memory creation failed");
        return 1;
    }
    ftruncate(shared_memory_fd, sizeof(int) * 1000);

    int shared_memory_ptr = (int)mmap(0, sizeof(int) * 1000, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
    if (shared_memory_ptr == MAP_FAILED) {
        perror("Memory mapping error");
        return 1;
    }

    for (int i = 0; i < total_numbers; i++) {
        pid_t process_id = fork();
if (process_id == 0) {
            compute_collatz(values[i], shared_memory_ptr);
            for (int j = 0; shared_memory_ptr[j] != 1; j++) {
                printf("%d ", shared_memory_ptr[j]);
            }
            printf("\n");
            munmap(shared_memory_ptr, sizeof(int) * 1000);
            exit(0);
        } else {
            wait(NULL);
        }
    }

    shm_unlink(SHARED_MEMORY_NAME);
    return 0;
}
