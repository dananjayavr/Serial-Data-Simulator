import serial
import time
import sys

if len(sys.argv) < 3:
    print("Usage: python firmware_sender_serial.py <firmware.bin> <serial_port>")
    print("Ex: python firmware_sender_serial.py build/firmware.bin /dev/pts/4")
    sys.exit(1)

firmware_path = sys.argv[1]
serial_port = sys.argv[2]

with open(firmware_path, "rb") as f:
    firmware = f.read()

ser = serial.Serial(serial_port, baudrate=115200, timeout=1)

print(f"Sending {len(firmware)} bytes to {serial_port}...")
chunk_size = 256
for i in range(0, len(firmware), chunk_size):
    chunk = firmware[i:i + chunk_size]
    ser.write(chunk)
    time.sleep(0.01)  # simulate transmission delay

ser.close()
print("Transmission complete.")
