#include "commonop.h"

int main()
{
	int skt = createSocket();

	struct sockaddr_in addr;
	defineAddress(&addr, PORT);

	if (connect(skt, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("CONNECT: ");
		exit(EXIT_FAILURE);
	}

	printf("Connected to sender!\n");
	fclose(fopen("dataReceived.txt", "w"));

	// Receiver buffer and window variables
	TCP_Packet recvBuffer[RECV_BUF_SIZE];
	TCP_Packet pkt;
	TCP_Packet res;

	recvPacket(skt, &pkt);
	res = pkt;
	res.advertisedWindow = RECV_BUF_SIZE;
	sendPacket(skt, &res);

	// Index values pointing to last read and next expected
	int next_expected = 0;
	int last_read = -1;

	int expected_seq_no = 0;
	bool cont = true;
	int counter = 0;
	while (cont) {
		counter++;
		int stat = recvPacket(skt, &pkt);
		if (stat == 0) {
			printf("Connection closed by sender. Writing buffered data...\n");
			for (int i = 0; i < next_expected; ++i) {
				writeFile("dataReceived.txt", recvBuffer[i].data);
				printf("Wrote: %s\n", recvBuffer[i].data);
			}
			break;
		}
		printf("-----\n");

		res = pkt;
		if (next_expected == RECV_BUF_SIZE) {
			printf("Buffer full. Packet %d dropped\n", counter);
			res.ack = false;
			res.advertisedWindow = RECV_BUF_SIZE - ((next_expected - 1) - last_read);

			sendPacket(skt, &res);
			printf("Sent AdvWindow: %d\n", res.advertisedWindow);
		}

		// Handling probe packets
		else if (pkt.seq_no == -1) {
			printf("Received packet no %d (Probe packet)\n", counter);
			res.ack = true;
			res.advertisedWindow = RECV_BUF_SIZE - ((next_expected - 1) - last_read);
			
			sendPacket(skt, &res);
		}

		// Handling dropped in-order packets
		else if (pkt.dropped && pkt.seq_no == expected_seq_no) {
			printf("Packet %d dropped (Sequence number: %d)\n", counter, pkt.seq_no);
			res.ack = false;
			res.advertisedWindow = RECV_BUF_SIZE - ((next_expected - 1) - last_read);

			sendPacket(skt, &res);
		}

		// In-order packet received
		else if (pkt.seq_no == expected_seq_no) {
			printf("Received packet no %d (Sequence number: %d)\n", counter, pkt.seq_no);
			printf("Data received: %s\n", pkt.data);

			recvBuffer[next_expected] = pkt;
			strcpy(recvBuffer[next_expected].data, pkt.data);
			++next_expected;

			res.ack = true;
			res.data[0] = '\0';

			// Calculate advertisedWindow
			res.advertisedWindow = RECV_BUF_SIZE - ((next_expected - 1) - last_read);

			sendPacket(skt, &res);
			printf("Sent ACK %d\n", res.seq_no);

			expected_seq_no = wrap(expected_seq_no + 1, SEQ_MAX);
		}

		// Probe packet received
		else {
			printf("Dropped out of order packet no %d ()\n", counter);
		}

		// Unread data present in buffer
		if (last_read < next_expected - 1) {
			int readCount;
			printf("Used buffer space: %d. Do you want to free up buffer space?\n", (next_expected - 1) - last_read);
			printf("0 to %d packets can be read. Enter value: ", (next_expected - 1) - last_read);
			scanf("%d", &readCount);

			readCount = (readCount > (next_expected - 1) - last_read)? (next_expected - 1) - last_read : readCount;
			int shift = readCount;

			for (int i = 0; i < readCount; ++i) {
				writeFile("dataReceived.txt", recvBuffer[i].data);
				printf("Wrote: %s\n", recvBuffer[i].data);
			}

			for(int i = 0; i + shift < RECV_BUF_SIZE; ++i) {
				recvBuffer[i] = recvBuffer[i + shift];
			}

			next_expected -= shift;

			int used = (next_expected - 1) - last_read;
			printf("Used buffer space: %d\tRemaining: %d\n", used, RECV_BUF_SIZE - used);
		}
	}

	printf("\n-----\n\n");
	printf("Sender closed TCP connection, complete received data:\n");

	char buf[DATA_MAX];
	FILE* fptr = fopen("dataReceived.txt", "r");

	while(!feof(fptr)) {
		readFile(fptr, buf, sizeof(buf));
		printf("%s", buf);
	}
	fclose(fptr);

	printf("\n");

	close(skt);
	return 0;
}	
