#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int sockfd;
    char buffer[1024];
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
    if (bind(sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0)
    {
        printf("Bind failed\n");
        return 1;
    }
    printf("UDP Echo Server is running...\n");
    int n = recvfrom(sockfd,buffer,sizeof(buffer) - 1,0,(struct sockaddr *)&clientAddr,&addrLen);
    if (n < 0)
    {
        printf("Receive failed\n");
        return 1;
    }
    buffer[n] = '\0';
    printf("Client: %s\n", buffer);
    sendto(sockfd,buffer, n,0,(struct sockaddr *)&clientAddr,addrLen);
    printf("Echo sent to client\n");
    close(sockfd);
    return 0;
}
