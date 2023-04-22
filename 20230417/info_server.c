#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
    // Check enough arguments
    if (argc != 2)
    {
        printf("wrong number of arguments\n");
        exit(1);
    }

    // tao socket
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("create socket failed");
        exit(1);
    }

   // Khai bao cau truc dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);;
    addr.sin_port = htons(atoi(argv[1]));

    // Bind socket to sockaddr
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind() failed");
        exit(1);
    }

    // Listen for connections
    if (listen(listener, 5) == -1)
    {
        printf("Listening Failed\n");
        exit(1);
    }
    printf("socket created\n waiting for connection");

    //accept
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == -1)
    {
        printf(" connection failed\n");
        exit(1);
    }
    printf("Client connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    
    // Receive message from server
    char buff[512];
    char computerName[128];
    char diskNumber;
    char diskName;
    short diskSize = 0;

    while (1)
    {
        int ret = recv(clientSocket, buff, sizeof(buff), 0);
        if (ret <= 0) break;

        int pos = 0;
        memcpy(&diskNumber, buff + pos, sizeof(diskNumber));
        pos += sizeof(diskNumber);

        memcpy(computerName, buff + 1 + atoi(&diskNumber) * 3, ret - 1 - atoi(&diskNumber) * 3);
        printf("Computer name: %s\n", computerName);
        printf("Numbers of disk: %c\n", diskNumber);

        for (int i = 0; i < atoi(&diskNumber); ++i)
        {
            memcpy(&diskName, buff + pos, sizeof(diskName));
            pos += sizeof(diskName);

            memcpy(&diskSize, buff + pos, sizeof(diskSize));
            pos += sizeof(diskSize);

            printf("%c : %hd\n", diskName, diskSize);
        }
    }

    // Close
    close(clientSocket);
    close(listener);
    printf("Socket closed\n");
    return 0;
}