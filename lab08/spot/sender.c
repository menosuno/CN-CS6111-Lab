#include "header.h"

int main()
{
    int senderSocket;
    senderSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (senderSocket < 0)
    {
        printf("ERROR in socket CREATION\n\n");
        exit(1);
    }

    struct sockaddr_in receiver_addr;

    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(PORT);
    receiver_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(senderSocket, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0)
    {
        printf("ERROR while CONNECTING!!\n\n");
        exit(1);
    }

    printf("CONNECTED to receiver on PORT %d\n", PORT);

    //SEQUENCE NUMBER VARIABLE
    int next_seq_num = 0;

    //BEGINNING OF WINDOW
    int window_begin = 0;

    //REQUESTS and RESPONSES
    tcpPacket packets[SIZE];
    char responseFromReceiver[SIZE], ack_status_response[SIZE];
    int seq_num_response;

    //i -> index to array of structures
    //j -> index to array of structures (while retransmitting)
    //count -> total number of packets successfully acknowledged by receiver
    int num_packets, sent_len, recv_len, i = 0, retransmit = 0;
    int j, count = 0;

    char fileName[SIZE], lineBuffer[SIZE];

    printf("Enter the number of packets (max of 20): ");
    scanf("%d", &num_packets);
    printf("\n");

    printf("Enter the file name: ");
    scanf("%s", fileName);

    FILE *ptr;
    ptr = fopen(fileName, "r");

    if (!ptr)
    {
        printf("ERROR in opening file\n\n");
        exit(1);
    }

    printf("Window start position: %d\tWindow end position: %d\n\n", window_begin, window_begin + N - 1);
    while (count < num_packets)
    {
        //Packets till the end of current window are sent
        while ((i < (window_begin + N)) && (i <= num_packets))
        {
            packets[i].data[0] = '\0';
            packets[i].header.seq_num = next_seq_num;
            packets[i].header.ack_flag = 0;

            //For termination purposes
            if (i == (num_packets))
            {
                packets[i].header.drop_flag = 0;
                packets[i].header.seq_num = -2;
            }
            else
            {
                if (fgets(packets[i].data, sizeof(packets[i].data), ptr))
                {
                    printf("The data: %s", packets[i].data);
                }
                else
                {
                    printf("No data present!!\n\n");
                    exit(1);
                }

                printf("Do you want to drop the packet number %d? (1 for YES and 0 for NO): ", i);
                scanf("%d", &(packets[i].header.drop_flag));

                printf("Packet number %d sent, sequence number is %d\n\n", i, packets[i].header.seq_num);
            }

            sent_len = send(senderSocket, &packets[i], sizeof(tcpPacket), 0);

            next_seq_num = nextSeqNum(next_seq_num);
            i++;
        }

        if (retransmit)
        {
            printf("-----RETRANSMISSION-----\n");

            while ((j < (window_begin + N)) && (j < num_packets))
            {

                printf("Packet number %d resent\n", j);
                packets[j].header.drop_flag = 0;
                packets[j].header.ack_flag = 0;
                sent_len = send(senderSocket, &packets[j], sizeof(tcpPacket), 0);
                j++;
            }

            printf("----------\n");
            j = 0;
            retransmit = 0;
        }

        //RECEIVING takes place
        ack_status_response[0] = '\0';
        seq_num_response = -1;
        recv_len = recv(senderSocket, &responseFromReceiver, sizeof(responseFromReceiver), 0);
        sscanf(responseFromReceiver, "%s %d", &ack_status_response, &seq_num_response);

        if (strcmp(ack_status_response, "ACK") == 0)
        {
            printf("Packet number %d received (sequence number %d)\n", count, seq_num_response);
            packets[count].header.ack_flag = 1;

            window_begin++;
            count++;

            printf("Window start position: %d\tWindow end position: %d\n\n", window_begin, window_begin + N - 1);
        }
        else
        {
            printf("Packet number %d dropped (sequence number %d)\n", count, seq_num_response);
            j = window_begin;
            retransmit = 1;
        }
    }

    fclose(ptr);
    close(senderSocket);
    return 0;
}