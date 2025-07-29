## Getting Started

- Build the project (4 targets)
- Optionally, install `socat` and `pyserial` using `pip` (or equivalent)

### Using TCP Sockets

- In one terminal, run `tcp_server`
- Run `generate_firmware` to generate a random binary file
- Then run, `./tcp_client firmware.bin` to send the binary file
- server should successfully receive the firmware
- Compare the 'sameness' of the two files by: `cmp firmware.bin firmware_received.bin && echo "Files match!" || echo "Files differ!"`

### Using (Virtual) Serial Terminal

- Use `socat` to create a virtual serial pair

```
socat -d -d pty,raw,echo=0 pty,raw,echo=0
# Output:
# PTY is /dev/pts/3
# PTY is /dev/pts/4
```

Here, `/dev/pts/3` is server while `/dev/pts/4` is client.

- Run serial_server (e.g., `./serial_server /dev/pts/2`).
- In another terminal, run `./serial_client firmware.bin /dev/pts/3`. If using Python script, run `serial_client.py firmware.bin /dev/pts/3`
- Server should acknoledge the received data. After a 5 second timeout, it should quit.
- Use the same command as above to compare the two files

## Further Reading

- Socat : https://www.baeldung.com/linux/socat-command
