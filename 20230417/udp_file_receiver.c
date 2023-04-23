#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define TIME_OUT 9

int main(int argc, char* argv[])    
{
    // Check enough arguments
    // Arguments contain run_file_name, port_number, write_file_name 
    if (argc != 3)
    {
        printf("Not enough arguments\n");
        exit(1);
    }

    // Create socket
    int 
    listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (
        listener == -1)
    {
        perror("Create Socket Failed\n");
        exit(1);
    }


    // Bind socket to ip address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    if (bind(
        listener, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("Bind Failed\n");
        exit(1);
    }

    printf("Waiting for message ...\n");
    
    // Receive message from client
    char buff[512];
    struct sockaddr_in clientAddr;
    int size = sizeof(clientAddr);


    int ret;
    while (1)
    {
        
        FILE *f = fopen(argv[2], "a");
        if (f == NULL)
        {
            printf("Open file failed\n");
            exit(1);
        }
        ret = recvfrom(
            listener, buff, sizeof(buff), 0, (struct sockaddr *)&clientAddr, &size);
        if (ret <= 0)
            break;

        fwrite(buff, 1, ret, f);
        printf("Receive %d bytes message from: %s:%d\n", ret, inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
        fclose(f);
    }


    // Close socket
    close(listener);
    printf("Time out!\n");
    printf("Socket closed\n");
    return 0;
}