#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define CHUNK_SIZE 256

void configure_serial(int fd) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag = 0;
    tty.c_oflag = 0;

    // THIS IS THE KEY CHANGE:
    tty.c_cc[VMIN]  = 0;  // return as soon as any data is available or timeout
    tty.c_cc[VTIME] = 10; // wait max 1 second (10 * 0.1s)

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB | CRTSCTS);

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
        perror("tcsetattr");
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <serial_port>\n", argv[0]);
        return 1;
    }

    const char *port = argv[1];
    int fd = open(port, O_RDONLY | O_NOCTTY);
    tcflush(fd, TCIFLUSH);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    configure_serial(fd);

    FILE *fw_file = fopen("firmware_received.bin", "wb");
    if (!fw_file) {
        perror("fopen");
        close(fd);
        return 1;
    }

    char buffer[CHUNK_SIZE];
    ssize_t n;
    printf("Receiving firmware on %s...\n", port);

    int total = 0;
    int empty_reads = 0;
    const int MAX_EMPTY = 5;  // fewer, because timeout is 1 sec now

    while (1) {
        ssize_t n = read(fd, buffer, sizeof(buffer));
        if (n > 0) {
            printf("Read %zd bytes\n", n);
            fwrite(buffer, 1, n, fw_file);
            total += n;
            empty_reads = 0;
        } else if (n == 0) {
            empty_reads++;
            if (empty_reads > MAX_EMPTY) {
                printf("Timeout: no data for %d seconds. Exiting.\n", MAX_EMPTY);
                break;
            }
        } else {
            perror("read");
            break;
        }
    }
    
    printf("Firmware received: %d bytes total.\n", total);

    fclose(fw_file);
    close(fd);

    return 0;
}
