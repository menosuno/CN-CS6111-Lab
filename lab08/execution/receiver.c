#include "header.h"

int main()
{
    int receiverSocket, senderSocket;
    receiverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (receiverSocket < 0)
    {
        printf("ERROR in socket CREATION\n\n");
        exit(1);
    }

    struct sockaddr_in receiver_addr;

    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(PORT);
    receiver_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(receiverSocket, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0)
    {
        printf("ERROR while CONNECTING\n\n");
        exit(1);
    }

    if (listen(receiverSocket, 3) < 0)
    {
        printf("ERROR while LISTENING\n\n");
        exit(1);
    }

    senderSocket = accept(receiverSocket, NULL, NULL);
    printf("CONNECTED to sender on PORT %d\n\n", PORT);

    //count -> number of packets successfully received (index to buffer)
    int recv_len, sent_len, next_seq_num = 0, count = 0;

    //REQUESTS and RESPONSES
    char responseFromReceiver[SIZE];
    tcpPacket requestPacket[SIZE];

    while (1)
    {
        responseFromReceiver[0] = '\0';
        recv_len = recv(senderSocket, &requestPacket[count], sizeof(tcpPacket), 0);
        printf("Next packet with sequence number %d\n", requestPacket[count].header.seq_num);

        //FOR TERMINATING PURPOSES
        if (requestPacket[count].header.seq_num == -2)
        {
            printf("-----Closing Connection-----\n");
            close(senderSocket);
            break;
        }

        //CHECKING if the expected sequence number matches with the received sequence number
        if (requestPacket[count].header.seq_num == next_seq_num)
        {
            if (requestPacket[count].header.drop_flag == 1)
            {
                printf("Packet number %d dropped (sequence number %d)\n", count, requestPacket[count].header.seq_num);
                bzero(&(requestPacket[count]), sizeof(tcpPacket));
                sprintf(responseFromReceiver, "NACK %d", next_seq_num);

                sent_len = send(senderSocket, &responseFromReceiver, sizeof(responseFromReceiver), 0);
            }
            else
            {

                printf("Packet number %d received successfully (sequence number %d)\n", count, requestPacket[count].header.seq_num);
                printf("The data: %s\n", requestPacket[count].data);

                requestPacket[count].header.ack_flag = 1;
                sprintf(responseFromReceiver, "ACK %d", next_seq_num);
                next_seq_num = nextSeqNum(next_seq_num);

                sent_len = send(senderSocket, &responseFromReceiver, sizeof(responseFromReceiver), 0);
                count++;
            }
        }
        else
        {
            bzero(&(requestPacket[count]), sizeof(tcpPacket));
        }

        printf("\n");
    }

    close(receiverSocket);
    return 0;
}