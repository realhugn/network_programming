#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>

int cntStr(const char *haystack, const char *needle){
    int count = 0;
    const char *tmp = haystack;
    while(tmp = strstr(tmp, needle))
    {
        count++;
        tmp+= sizeof(needle);
    }
    return count;
}

int main(int argc, char* argv[])
{
    // Check enough arguments
    // Arguments contain run_file_name, port_number 
    if (argc != 2)
    {
        printf("Missing arguments\n");
        exit(1);
    }

    // Create socket
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("Create socket failed: ");
        exit(1);
    }

    // Create struct sockaddr
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);;
    addr.sin_port = htons(atoi(argv[1]));

    // Bind socket to sockaddr
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("Binding Failed\n");
        exit(1);
    }

    // Listen 
    if (listen(listener, 5) == -1)
    {
        printf("Listening Failed\n");
        exit(1);
    }
    printf("Waiting for client connecting ...\n");

    // Accept connection
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int 
    client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (
        client == -1)
    {
        printf("Accept connection failed\n");
        exit(1);
    }
    printf("Client has connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    
    // Receive message from client
    int count = 0, ret = 0;
    char buff[20];
    char mem[9];
    char bridgeString[18];
    bool firstBlock = true;

    while (1)
    {
        ret = recv(client, buff, sizeof(buff), 0);
        if (ret <= 0) break;
        buff[ret] = 0;

        if (firstBlock)
            firstBlock = false;
        else
        {
            memcpy(bridgeString, mem, 9);
            memcpy(bridgeString + 9, buff, 9);
            count += cntStr(bridgeString, "0123456789");
        }

        count += cntStr(buff, "0123456789");
        memcpy(mem, buff + sizeof(buff) - 9, 9);
    }

    printf("Number of %s is %d\n", "0123456789", count);
    
    // Close socket
    close(client);
    close(listener);
    printf("Socket closed\n");
    return 0;
}