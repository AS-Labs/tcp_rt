package main

import "core:bytes"
import "core:fmt"
import "core:net"
import "core:strings"

main :: proc() {
	// setting up socket for our server
	listen_socket, listen_err := net.listen_tcp(
		net.Endpoint{port = 8000, address = net.IP4_Loopback},
	)
	if listen_err != nil {
		fmt.panicf("listen error : %s", listen_err)
	}

	// setting up socket for the client to connect to
	client_socket, client_endpoint, accept_err := net.accept_tcp(listen_socket)

	if accept_err != nil {
		fmt.panicf("%s", accept_err)
	}

	handleClient(client_socket)
}

handleClient :: proc(client_soc: net.TCP_Socket) {
	// This loops till our client wants to disconnect
	for {
		// allocating memory for our data
		data_in_bytes: [8]byte
		// receving some data from client
		_, err := net.recv_tcp(client_soc, data_in_bytes[:])
		if err != nil {
			fmt.panicf("error while recieving data %s", err)
		}
		// this is just "exit" in byte format
		exit_code := [8]byte{101, 120, 105, 116, 13, 10, 0, 0}
		if data_in_bytes == exit_code {
			fmt.println("connection ended")
			break
		}
		// converting bytes data to string
		data, e := strings.clone_from_bytes(data_in_bytes[:], context.allocator)
		fmt.println("client said :", data)
	}
}
