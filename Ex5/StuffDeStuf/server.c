#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int sockfd, n, i, j, count;
    char word[5], binary[17], stuffed[1024];
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Socket creation failed\n");
        return 1;
    }
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0)
    {
        printf("Bind failed\n");
        close(sockfd);
        return 1;
    }
    printf("UDP Bit Stuffing Server is running...\n");
    while (1)
    {
        n = recvfrom(sockfd, word, sizeof(word) - 1, 0,(struct sockaddr *)&clientAddr,&addrLen);
        if (n < 0)
        {
            printf("Receive failed\n");
            break;
        }
        word[n] = '\0';
        printf("Client: %s\n", word);
        for (i = 0; i < 4; i++)
        {
            int value;
            if (word[i] >= '0' && word[i] <= '9')value = word[i] - '0';
            else if (word[i] >= 'A' && word[i] <= 'F')value = word[i] - 'A' + 10;
            else value = word[i] - 'a' + 10;
            for (j = 3; j >= 0; j--) binary[i * 4 + (3 - j)] = ((value >> j) & 1) + '0';
        }
        binary[16] = '\0';
        printf("16-bit binary: %s\n", binary);
        j = 0;count = 0;
        for (i = 0; i < 16; i++)
        {
            stuffed[j++] = binary[i];
            if (binary[i] == '1')count++;
            else count = 0;
            if (count == 5)
            {
                stuffed[j++] = '0';
                count = 0;
            }
        }
        stuffed[j] = '\0';
        printf("Bit-stuffed binary: %s\n", stuffed);
        sendto(sockfd, stuffed, strlen(stuffed), 0,(struct sockaddr *)&clientAddr,addrLen);
        printf("Bit-stuffed string sent\n");
    }
    close(sockfd);
    return 0;
}
