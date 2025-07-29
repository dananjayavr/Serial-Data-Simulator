#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 9000
#define CHUNK_SIZE  256

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s firmware.bin\n", argv[0]);
        return 1;
    }

    const char *firmware_path = argv[1];
    FILE *fw = fopen(firmware_path, "rb");
    if (!fw) {
        perror("Failed to open firmware file");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        fclose(fw);
        return 1;
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
    };
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    printf("Connecting to bootloader at %s:%d...\n", SERVER_IP, SERVER_PORT);
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect failed");
        close(sock);
        fclose(fw);
        return 1;
    }

    printf("Connected. Sending firmware...\n");

    char buffer[CHUNK_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, fw)) > 0) {
        if (send(sock, buffer, bytes_read, 0) < 0) {
            perror("Send failed");
            break;
        }
        usleep(10000); // simulate UART delay (10ms)
    }

    printf("Done sending.\n");
    fclose(fw);
    close(sock);
    return 0;
}
