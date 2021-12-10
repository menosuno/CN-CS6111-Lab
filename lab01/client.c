int main() {
	// creating client socket
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);

	// feeding server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(3071);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// connecting to the server
	int connection_status = connect(client_socket, (struct sockaddr*)& server_address, sizeof(server_address));
	if (connection_status < 0) {
		printf("ERROR in connecting\n");
		return -1;
	}

	// receiving the message from server
	char buf[200];
	recv(client_socket, &buf, sizeof(buf), 0);

	if (sizeof(buf) > 0) {
		printf("Received message: %s", buf);
		char response[200] = "Message received\n";
		// sending the response
		send(client_socket, response, sizeof(response), 0);
	}

	// closing the socket
	close(client_socket);

	return 0;
}
