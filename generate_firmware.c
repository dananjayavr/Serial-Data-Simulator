#include <stdio.h>
#include <stdint.h>

int main() {
    FILE *f = fopen("firmware.bin", "wb");
    if (!f) {
        perror("Failed to create firmware.bin");
        return 1;
    }

    // Example: write 256 bytes of increasing pattern 0x00 to 0xFF
    for (uint16_t i = 0; i < 256; i++) {
        uint8_t byte = i & 0xFF;
        fwrite(&byte, 1, 1, f);
    }

    fclose(f);
    printf("firmware.bin generated.\n");
    return 0;
}
