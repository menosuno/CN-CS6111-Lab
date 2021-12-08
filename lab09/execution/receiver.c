#include "header.h"

int main()
{
	struct sockaddr_in receiver_addr;
	int receiver_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (receiver_socket < 0)
	{
		printf("error in socket creation\n");
		return 0;
	}
	
	printf("socket created\n");
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_port = htons(port);
	receiver_addr.sin_addr.s_addr = INADDR_ANY;

	int ret = bind(receiver_socket, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr));
	if (ret < 0)
	{
		printf("error in binding\n");
		return 0;
	}
	printf("binding succesful\n");

	if (listen(receiver_socket, N) < 0)
	{
		printf("error in listening\n");
		return 0;
	}
	printf("listening to port %d\n\n", port);
	int sender_sock = accept(receiver_socket, NULL, NULL);
	int next_seq_num = 0, count = 0;
	char response[SIZE];
	int track_close = 0;
	int lost_count = 0, lost_seq_no = -1, flag = 0;
	tcppacket packet[SIZE];
	int num_packet = 0;
	recv(sender_sock, &num_packet, sizeof(num_packet), 0);
	printf("num_packet = %d\n", num_packet);
	while (1)
	{
		flag = 0;
		response[0] = '\0';
		int rb = recv(sender_sock, &packet[count], sizeof(tcppacket), 0);
		if (packet[count].header.seq_num == lost_seq_no)
		{
			printf("packet number %d received succesfully(seq num %d)\n", lost_count, packet[count].header.seq_num);
			printf("the data : %s\n", packet[count].data);

			packet[count].header.ack_flag = 1;
			sprintf(response, "ACK %d", lost_seq_no);
			next_seq_num = nextseqnum(next_seq_num);
			strcpy(packet[lost_count].data, packet[count].data);
			rb = send(sender_sock, &response, sizeof(response), 0);
			track_close++;
			flag = 1;
		}

		printf("packet with seq number %d received\n", packet[count].header.seq_num);
		if (flag != 1)
		{
			if (packet[count].header.drop_flag == 1)
			{
				lost_count = count;
				lost_seq_no = packet[count].header.seq_num;
				printf("packet number %d dropped(seq num %d)\n", count, packet[count].header.seq_num);
				bzero(&packet[count], sizeof(tcppacket));
				sprintf(response, "NACK %d", lost_seq_no);

				next_seq_num = nextseqnum(next_seq_num);
				int sb = send(sender_sock, &response, sizeof(response), 0);
				count++;
			}
			else
			{
				printf("packet number %d received succesfully(seq num %d)\n", count, packet[count].header.seq_num);
				printf("the data : %s\n", packet[count].data);

				packet[count].header.ack_flag = 1;
				sprintf(response, "ACK %d", packet[count].header.seq_num);
				next_seq_num = nextseqnum(next_seq_num);

				rb = send(sender_sock, &response, sizeof(response), 0);
				count++;
				track_close++;
			}
		}

		if (track_close == num_packet)
		{
			printf("CLOSING CONNECTION...\n");
			close(sender_sock);
			break;
		}
		printf("\n");
	}
	printf("DATA STORED IN BUFFER\n");
	for (int i = 0; i < count; i++)
	{
		printf("%s\n", packet[i].data);
	}
	close(receiver_socket);
	return 0;
}