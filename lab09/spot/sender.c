#include "header.h"

int main()
{

	struct sockaddr_in receiver_addr;
	int sender_sock = socket(AF_INET, SOCK_STREAM, 0);

	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_port = htons(port);
	receiver_addr.sin_addr.s_addr = INADDR_ANY;
	int ret = connect(sender_sock, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr));
	if (ret < 0)
	{
		printf("error in connection\n");
		return 0;
	}
	printf("connected to reciever on port %d\n", port);

	int next_seq_num = 0;
	int window_begin = 0;

	int lost_count, lost_seq_no = -1, track_close = 0;
	tcppacket packets[SIZE];
	char response[SIZE], ack_response[SIZE];
	int seq_num_response;

	int i = 0, j = 0, count = 0, retransmit = 0;
	int num_packets;

	printf("enter the number of packets to send\n");
	scanf("%d", &num_packets);
	send(sender_sock, &num_packets, sizeof(num_packets), 0);
	FILE *fd;
	fd = fopen("packets.txt", "r");
	if (!fd)
	{
		printf("error in opening file\n");
		return 0;
	}
	printf("window starting position - %d\nwindow ending point - %d\n\n", window_begin, window_begin + N - 1);
	while (track_close != num_packets)
	{
		while ((i < num_packets) && (i < (window_begin + N)))
		{
			packets[i].data[0] = '\0';
			packets[i].header.seq_num = next_seq_num;
			packets[i].header.ack_flag = 0;
			if (fgets(packets[i].data, sizeof(packets[i].data), fd))
			{
					printf("the data to be sent : %s\n", packets[i].data);
			}
			else
			{
					printf("no data found\n");
					return 0;
			}

			printf("packet number %d is sent(seq no %d)\n", i, packets[i].header.seq_num);
			int sb = send(sender_sock, &packets[i], sizeof(tcppacket), 0);
			next_seq_num = nextseqnum(next_seq_num);
			i++;
		}
		if (retransmit)
		{
			printf("RETRANSMISSION.......\n");
			printf("Resending packet %d\n", j);
			packets[j].header.drop_flag = 0;
			packets[j].header.ack_flag = 0;
			int nb = send(sender_sock, &packets[j], sizeof(tcppacket), 0);
			printf("RETRANSMISSION COMPLETED\n");

			retransmit = 0;
		}
		ack_response[0] = '\0';
		seq_num_response = -1;

		int rb = recv(sender_sock, &response, sizeof(response), 0);
		sscanf(response, "%s %d", ack_response, &seq_num_response);
		if (seq_num_response == lost_seq_no)
		{

			printf("ACK for packet number %d received(sequence number %d)\n", lost_count, lost_seq_no);
			packets[lost_count].header.ack_flag = 1;

			window_begin += N;
			track_close++;

			printf("window start position - %d\nwindow end position - %d\n", window_begin, window_begin + N - 1);
			lost_seq_no = -1;
		}
		else
		{
			if (strcmp(ack_response, "ACK") == 0)
			{
				printf("ACK for packet number %d received(sequence number %d)\n", count, seq_num_response);
				packets[count].header.ack_flag = 1;
				if ((seq_num_response < lost_seq_no) || (lost_seq_no == -1))
				{
						window_begin++;
				}
				count++;
				track_close++;
				printf("window start position - %d\nwindow end position - %d\n", window_begin, window_begin + N - 1);
			}
			else
			{
				printf("ACK LOST...\n");
				printf("ack has been lost for packet number %d (sequence number %d)\n", count, seq_num_response);
				j = count;
				lost_count = count;
				lost_seq_no = seq_num_response;
				count++;
				retransmit = 1;
			}
		}
	}
	fclose(fd);
	close(sender_sock);
	return 0;
}