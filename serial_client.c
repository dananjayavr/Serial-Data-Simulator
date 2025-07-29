#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

void configure_serial(int fd) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        exit(1);
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8 bits
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag = 0;                                // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                                // no remapping, no delays
    tty.c_cc[VMIN]  = 1;                            // blocking read until 1 byte arrives
    tty.c_cc[VTIME] = 5;                            // 0.5 seconds read timeout

    tty.c_cflag |= (CLOCAL | CREAD);                // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);              // no parity
    tty.c_cflag &= ~CSTOPB;                         // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                        // no hardware flow control

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <firmware_file> <serial_port>\n", argv[0]);
        return 1;
    }

    const char *firmware_path = argv[1];
    const char *serial_port = argv[2];

    FILE *fw = fopen(firmware_path, "rb");
    if (!fw) {
        perror("Failed to open firmware file");
        return 1;
    }

    int fd = open(serial_port, O_WRONLY | O_NOCTTY);
    if (fd < 0) {
        perror("Failed to open serial port");
        fclose(fw);
        return 1;
    }

    configure_serial(fd);

    printf("Sending firmware '%s' to '%s'\n", firmware_path, serial_port);

    unsigned char buffer[256];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fw)) > 0) {
        ssize_t bytes_written = 0;
        while (bytes_written < bytes_read) {
            ssize_t n = write(fd, buffer + bytes_written, bytes_read - bytes_written);
            if (n < 0) {
                perror("write");
                close(fd);
                fclose(fw);
                return 1;
            }
            bytes_written += n;
        }
        usleep(10000); // 10ms delay to avoid flooding the receiver
    }

    printf("Firmware sent successfully.\n");

    close(fd);
    fclose(fw);
    return 0;
}
