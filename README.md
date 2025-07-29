## Getting Started

- Build the project (3 targets)
- In one terminal, run `app`
- Run `generate_firmware` to generate a random binary file
- Then run, `./server firmware.bin` to send the binary file
- app should successfully receive the firmware
- Compare the 'sameness' of the two files by: `cmp firmware.bin firmware_received.bin && echo "Files match!" || echo "Files differ!"`

### Using Serial

- Use `socat` to create a virtual serial pair

```
socat -d -d pty,raw,echo=0 pty,raw,echo=0
# Output:
# PTY is /dev/pts/3
# PTY is /dev/pts/4
```

Here, /dev/pts/3 is app while /dev/pts/4 is Python script


