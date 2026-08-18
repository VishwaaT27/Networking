#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int sockfd, n, i;
    char buffer[1024], temp;
    struct sockaddr_in serverAddr, clientAddr;
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
        close(sockfd);return 1;
    }
    printf("UDP Reverse Server is running...\n");
    while (1)
    {
        n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,(struct sockaddr *)&clientAddr,&addrLen);
        if (n < 0)
        {
            printf("Receive failed\n");
            break;
        }
        buffer[n] = '\0';
        printf("Client: %s\n", buffer);
        for (i = 0; i < n / 2; i++)
        {
            temp = buffer[i];
            buffer[i] = buffer[n - i - 1];
            buffer[n - i - 1] = temp;
        }
        sendto(sockfd, buffer, n, 0,(struct sockaddr *)&clientAddr,addrLen);
        printf("Reversed string sent\n");
    }
    close(sockfd);
    return 0;
}
