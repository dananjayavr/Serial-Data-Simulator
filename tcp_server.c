#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 9000

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    char buffer[1024];
    FILE *fw_file;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 1);

    printf("Bootloader simulator: waiting for connection...\n");
    socklen_t addrlen = sizeof(address);
    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    printf("Connected!\n");

    fw_file = fopen("firmware_received.bin", "wb");

    int bytes_read;
    while ((bytes_read = read(new_socket, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, bytes_read, fw_file);
    }

    fclose(fw_file);
    close(new_socket);
    close(server_fd);

    printf("Firmware received and saved.\n");
    return 0;
}
