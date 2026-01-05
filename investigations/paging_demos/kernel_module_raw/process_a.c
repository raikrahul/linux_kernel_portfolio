/*
 * PROCESS A: Allocates memory, prints VA, triggers page walk
 * Compile: gcc -o process_a process_a.c
 * Run: ./process_a
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    /* Allocate 4KB on heap */
    char* buffer = malloc(4096);

    /* Write pattern to make page dirty */
    memset(buffer, 'A', 4096);

    printf("═══════════════════════════════════════════════════════════════\n");
    printf("PROCESS A\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("PID            = %d\n", getpid());
    printf("buffer VA      = %p\n", (void*)buffer);
    printf("buffer[0]      = '%c'\n", buffer[0]);
    printf("\n");
    printf("TO WALK THIS ADDRESS:\n");
    printf("  echo '0x%lx' | sudo tee /proc/pagewalk\n", (unsigned long)buffer);
    printf("  cat /proc/pagewalk\n");
    printf("\n");
    printf("Press ENTER to exit (keep running to compare with Process B)...\n");

    getchar();

    free(buffer);
    return 0;
}
