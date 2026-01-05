#include <unistd.h>  // sbrk()

int main() {
    // --- First allocation: 100 bytes ---
    size_t old_break = (size_t)sbrk(0);  // current break
    size_t pad = 0;
    if (old_break % 8 != 0) pad = 8 - (old_break % 8);
    char* ptr1 = (char*)sbrk(100 + pad + 16);  // allocate: user + pad + header(16)
    ptr1 += pad;                               // align
    *(size_t*)ptr1 = 100;                      // store user size
    *(size_t*)(ptr1 + 8) = pad;                // store pad
    ptr1 += 16;                                // user data starts after 16-byte header
    for (size_t i = 0; i < 100; i++) ptr1[i] = i;

    // --- Second allocation: 50 bytes ---
    old_break = (size_t)sbrk(0);  // new break
    pad = 0;
    if (old_break % 8 != 0) pad = 8 - (old_break % 8);
    char* ptr2 = (char*)sbrk(50 + pad);  // allocate memory
    ptr2 += pad;                         // adjust to aligned address
    for (size_t i = 0; i < 50; i++) ptr2[i] = i;

    return 0;
}
