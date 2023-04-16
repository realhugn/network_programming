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
    if (argc != 3)
    {
        printf("Missing arguments\n");
        exit(1);
    }

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == -1)
    {
        perror("Create socket failed: ");
        exit(1);
    }

    // Khai bao dia chi cua server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));
    
    //ket noi den server
    if (connect(client, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("Connecting failed\n");
        exit(1);
    }

    // Nhan tin dau tu server
    char buff[512];
    char *message = NULL;
    int size = 0;
    while (1)
    {
        int len = recv(client, buff, sizeof(buff), 0);
        if (len <= 0)
        {
            printf("Connection closed\n");
            exit(1);
        }
        message = realloc(message, size + len);
        memcpy(message + size, buff, len);
        size += len;
        if (strstr(buff, "\r\n\r\n") != NULL) break;
    }
    printf("%s\n", message);
    free(message);

    //Gui tin den server
    while (1)
    {
        printf("Enter string: ");
        fgets(buff, sizeof(buff), stdin);
        if (strncmp(buff, "\n", 1) == 0) break;
        if (send(client, buff, strlen(buff), 0) == -1)
        {
            printf("Send Error\n");
            break;
        }
    }

    close(client);
    printf("Socket closed\n");
    return 0;
}