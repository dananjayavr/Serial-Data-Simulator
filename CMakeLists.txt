cmake_minimum_required(VERSION 3.22)
project(tcp_simulator LANGUAGES C)

add_executable(tcp_server
        tcp_server.c
)

add_executable(tcp_client
	tcp_client.c
)

add_executable(serial_server
	serial_server.c
)

add_executable(serial_client
	serial_client.c
)

add_executable(generate_firmware
	generate_firmware.c
)