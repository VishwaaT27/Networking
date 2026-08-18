#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int sockfd;
    char message[1024], buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addrLen = sizeof(serverAddr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Socket creation failed\n");
        return 1;
    }
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    printf("Enter message: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0';
    sendto(sockfd,message,strlen(message),0,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
    int n = recvfrom(sockfd,buffer,sizeof(buffer) - 1,0,(struct sockaddr *)&serverAddr,&addrLen);
    if (n < 0)
    {
        printf("Receive failed\n");
        return 1;
    }
    buffer[n] = '\0';
    printf("Server Echo: %s\n", buffer);
    close(sockfd);
    return 0;
}
