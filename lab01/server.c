int main() {
	// message
	char msg[200] = "Hello World\n";
	// buffer string
	char buf[200];

	// server socket creation
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		printf("ERROR in socket creation\n");
	}
	printf("Socket established\n\n");

	// feed server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(3071);	
	server_address.sin_addr.s_addr = INADDR_ANY;

	// bind the server
	int status = bind(server_socket, (struct sockaddr*)& server_address, sizeof(server_address));
	if (status < 0)	{
		printf("Error in establishing server\n\n");
		return -1;
	}

	printf("SERVER ESTABLISHED\n");
	printf("Listening on port %d\n\n", ntohs(server_address.sin_port));

	// listen to 1 client
	listen(server_socket, 1);

	// accepting from client socket
	int client_socket = accept(server_socket, NULL, NULL);

	// send the message to the client
	send(client_socket, msg, sizeof(msg), 0);
	// receiving the message from client
	recv(client_socket, buf, sizeof(buf), 0);

	// checking for the message from the client
	if (sizeof(buf) > 0) {
		printf("Message from client: %s\n", buf);
	}

	// closing the connection
	close(server_socket);

	return 0;
}
