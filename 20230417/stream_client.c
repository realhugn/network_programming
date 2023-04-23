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
    // Arguments contain ip_address, port_number, file_name
    if (argc != 4)
    {
        printf("Wrong number of arguments\n");
        exit(1);
    }

    // Create socket
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == -1)
    {
        perror("Create socket failed: ");
        exit(1);
    }

    // ip struct
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    //connect 
    if (connect(client, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("Connecting failed\n");
        exit(1);
    }

    // send message to server
    char *fileName = argv[3];
    FILE *f = fopen(fileName, "r");
    if (f == NULL)
    {
        printf("Open file failed\n");
        exit(1);
    }

    char buf[512];
    int ret;
    while (!feof(f))
    {
        ret = fread(buf, 1, sizeof(buf), f);
        if (ret <= 0)
            break;
        if (send(client, buf, ret, 0) == -1)
        {
            printf("Send Failed\n");
            exit(1);
        }
    }
    printf("Sended\n");
    fclose(f);

    // Close socket
    close(client);
    printf("Socket closed\n");
    return 0;
}