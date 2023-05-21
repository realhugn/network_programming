#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Missing arguments\n");
        exit(1);
    }

    // Create socket
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == -1)
    {
        perror("Create socket failed: ");
        exit(1);
    }

    // Connect to server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));
    
    if (connect(client, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("Connecting failed\n");
        exit(1);
    }

    char buff[512];
    fd_set fdread;
    int maxFd, ret;

    while (1)
    {
        FD_ZERO(&fdread);
        FD_SET(STDIN_FILENO, &fdread);
        FD_SET(client, &fdread);
        maxFd = client + 1;   // STDIN_FILENO is 0

        ret = select(maxFd + 1, &fdread, NULL, NULL, NULL);

        if (ret <= 0)
        {
            printf("Select Failed\n");
            exit(1);
        }

        if (FD_ISSET(STDIN_FILENO, &fdread))
        {
            fgets(buff, sizeof(buff), stdin);
            buff[strlen(buff) - 1] = 0;

            // exit signal
            if (strncmp(buff, "exit", 4) == 0) break;
            
            // send message to server
            send(client, buff, strlen(buff), 0);
        }

        if (FD_ISSET(client, &fdread))
        {
            ret = recv(client, buff, sizeof(buff), 0);            
            // Disconnected
            if (ret <= 0) break;
            
            buff[ret] = 0;
            printf("%s\n", buff);
        }
    }

    close(client);
    printf("Socket closed\n");
    return 0;
}