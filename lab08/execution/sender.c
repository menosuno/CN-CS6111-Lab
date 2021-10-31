#include "commonop.h"

int main()
{
	int skt = createSocket();

	struct sockaddr_in addr;
	defineAddress(&addr, PORT);

	if (bind(skt, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("BIND: ");
		exit(EXIT_FAILURE);
	}

	if (listen(skt, 5) < 0) {
		perror("LISTEN: ");
		exit(EXIT_FAILURE);
	}
	printf("Listening for connections...\n");

	int cskt = accept(skt, NULL, NULL);
	printf("Receiver connected!\n");

	TCP_Packet sendBuffer[SEND_BUF_SIZE];
	TCP_Packet pkt;
	TCP_Packet res;

	// Retrieve first advertised window
	pkt.seq_no = -1;
	pkt.dropped = false;
	pkt.data[0] = '\0';
	sendPacket(cskt, &pkt);
	recvPacket(cskt, &res);

	// Reading data from the file and copying it to the send buffer
	FILE* fptr = fopen("dataToSend.txt", "r");
	if (!fptr) {
		perror("READ FOPEN: ");
		exit(EXIT_FAILURE);
	}

	// Initialize max window size based on buffer size
	int window_size = (WIN_SIZE < SEND_BUF_SIZE)? WIN_SIZE : SEND_BUF_SIZE;
	int advertisedWindow = res.advertisedWindow;
	int effectiveWindow;

	// Index values pointing to last written, sent, and ACKed packets in the buffer
	int last_sent = -1;
	int last_acked = -1;
	int last_written = -1;

	int expected_seq = 0;
	int cur_seq_no = -1;
	int last_lost = -1;

	bool retransmit = false;
	bool readDone = false;

	int counter = 1;
	while (true) {

		// Fill the buffer as long as there is data to be read
		if (!readDone) {
			// Remove ACKed packets if buffer is full
			if (last_written == SEND_BUF_SIZE - 1) {
				int shift = last_acked + 1;
				for (int i = 0; i + shift < SEND_BUF_SIZE; ++i) {
					sendBuffer[i] = sendBuffer[i + shift];
				}

				last_acked   -= shift;
				last_sent    -= shift;
				last_written -= shift;
				last_lost    -= shift; 
			}

			for (int i = last_written + 1; i < SEND_BUF_SIZE; i++) {
				readFile(fptr, sendBuffer[i].data, sizeof(sendBuffer[i].data));
				if (feof(fptr))	{
					readDone = true;
				}

				++last_written;
			}
		}
		
		printf("Advertised Window: %d\n", advertisedWindow);
		// Sending probe packet if Advertised window is 0
		if (advertisedWindow == 0) {
			pkt.seq_no = -1;
			pkt.data[0] = '\0';

			printf("Do you wish to send a probe packet? (Enter any number): ");
			scanf("%*d");

			sendPacket(cskt, &pkt);
			printf("Sent packet no %d (Probe packet)\n\n", counter);

			++counter;
		}

		// If there are packets left to be sent
		else if (last_sent < last_written) {

			// Effective window calculation
			effectiveWindow = advertisedWindow - (last_sent - last_acked);
			printf("Effective window: %d\n\n", effectiveWindow);

			// Check if data available for sending is sufficient for chosen effective window
			// If not, reduce effective window
			if (last_written - last_sent < effectiveWindow) {
				effectiveWindow = last_written - last_sent;
			}

			// Filling the pipe
			for (int i = 0; i < effectiveWindow; ++i) {
				
				// Sending the next packet
				pkt = sendBuffer[last_sent + 1];

				cur_seq_no = wrap(cur_seq_no + 1, SEQ_MAX);
				pkt.seq_no = cur_seq_no;
				pkt.ack = false;
	
				if (retransmit && (last_sent < last_lost)) {
					printf("Retransmitting packet (Sequence number: %d)\n", pkt.seq_no);
				}

				printf("Do you want to drop packet no %d (Sequence number: %d) (0: No; 1: Yes): ", counter, pkt.seq_no);
				int drop;
				scanf("%d", &drop);
				pkt.dropped = (drop != 0)? true : false;
		
				sendPacket(cskt, &pkt);
				++last_sent;
				
				printf("Sent packet no %d (Sequence number: %d)\n", counter, pkt.seq_no);
				printf("Data sent: %s\n\n", pkt.data);

				++counter;
			}
		}
		retransmit = false;

		recvPacket(cskt, &res);

		// In order response pkt
		if (res.seq_no == expected_seq) {
			if (res.ack == true) {
				advertisedWindow = res.advertisedWindow;

			    printf("Received ACK %d\n", res.seq_no);

			 	expected_seq = wrap(expected_seq + 1, SEQ_MAX);
				++last_acked;
			}

			// Handling NACK for dropped packet
			else {
				printf("\nReceived NACK %d (Dropped packet)\n", res.seq_no);
				last_lost = last_sent;
				last_sent = last_acked;
				cur_seq_no = expected_seq - 1;

				retransmit = true;
			}
		}
		// Probe packet response
		else if (res.seq_no == -1) {
			printf("Received response for probe packet\n");
			advertisedWindow = res.advertisedWindow;
		}

		// End the transmission if all packets have been acked
		if (readDone && !retransmit && (last_acked == last_sent) && (last_sent == last_written)) {
			printf("Finished transmission\n");
			break;
		}
	}

	fclose(fptr);
	close(cskt);
	close(skt);
	return 0;
}
