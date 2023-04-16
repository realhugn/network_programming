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
    if (argc != 4)
    {
        printf("Missing arguments\n");
        exit(1);
    }
    //tao socket
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("Create socket failed: ");
        exit(1);
    } else {
        printf("Socket created: %d\n", listener);
    }
    // Khai bao cau truc dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);;
    addr.sin_port = htons(atoi(argv[1]));

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("Binding Failed\n");
        exit(1);
    }

    if (listen(listener, 5) == -1)
    {
        printf("Listening Failed\n");
        exit(1);
    }
    printf("Waiting for client connecting ...\n");

    //chap nhan ket noi
    struct sockaddr_in clientAddr;
    int clientAddrLength = sizeof(clientAddr);
    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLength);
    if (client == -1)
    {
        printf("Accept connection failed\n");
        exit(1);
    }
    printf("Client has connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // gui file cho client
    char *filename = argv[2];
    FILE *f = fopen(filename, "r");
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
            printf("Send hello to client failed\n");
            exit(1);
        }
    }
    send(client, "\r\n\r\n", 4, 0);   
    fclose(f);

    // Luu out put vao file
    filename = argv[3];
    f = fopen(filename, "a");
    if (f == NULL)
    {
        printf("Open file failed\n");
        exit(1);
    }

    while (1)
    {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0) break;
        fwrite(buf, 1, ret, f);
        printf("Writed\n");
    }
    fclose(f);

    close(client);
    close(listener);
    printf("Socket closed\n");
    return 0;
}