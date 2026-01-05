/*
 * PROCESS B: Allocates memory at SAME virtual address range, shows different CR3
 * Compile: gcc -o process_b process_b.c
 * Run: ./process_b
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    /* Allocate 4KB on heap */
    char* buffer = malloc(4096);

    /* Write DIFFERENT pattern */
    memset(buffer, 'B', 4096);

    printf("═══════════════════════════════════════════════════════════════\n");
    printf("PROCESS B\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("PID            = %d\n", getpid());
    printf("buffer VA      = %p\n", (void*)buffer);
    printf("buffer[0]      = '%c'\n", buffer[0]);
    printf("\n");
    printf("TO WALK THIS ADDRESS:\n");
    printf("  echo '0x%lx' | sudo tee /proc/pagewalk\n", (unsigned long)buffer);
    printf("  cat /proc/pagewalk\n");
    printf("\n");
    printf("Press ENTER to exit...\n");

    getchar();

    free(buffer);
    return 0;
}
