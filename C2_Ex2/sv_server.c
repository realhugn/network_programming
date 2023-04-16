#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

struct SinhVien {
    char mssv[9];
    char hoTen[64];
    char ngaySinh[11];
    float avg;
};

int main(int argc, char* argv[])
{
    // Check 
    if (argc != 2)
    {
        printf("Missing arguments\n");
        exit(1);
    }

    // Tao socket
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("Create socket failed: ");
        exit(1);
    }

    // Khai bao dia chi cua server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);;
    addr.sin_port = htons(atoi(argv[1]));

    // Bind
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

    // Chap nhan ket noi 
    struct sockaddr_in clientAddr;
    int clientAddrLength = sizeof(clientAddr);
    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLength);
    if (client == -1)
    {
        printf("Connection failed\n");
        exit(1);
    }
    printf("Client has connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    
    // Begin Write To File
    char *filename = "sv_log.txt";
    FILE *f = fopen(filename, "a");
    if (f == NULL)
    {
        printf("Open file failed\n");
        exit(1);
    }

    int ret;
    time_t t;
    t = time(NULL);
    struct tm tm = *localtime(&t);
    char buff[512];
    while (1)
    {
        struct SinhVien sv;    
        ret = recv(client, &sv, sizeof(sv), 0);
        if (ret <= 0) break;

        sprintf(buff, "%s:%d %d-%d-%d %d:%d:%d %s %s %s %.2f\n", 
        inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec, sv.mssv, sv.hoTen, sv.ngaySinh, sv.avg);

        printf("%s", buff);
        fwrite(buff, 1, strlen(buff), f);
    }
    fclose(f);
    //End Write to File

    close(client);
    close(listener);
    printf("Socket closed\n");
    return 0;
}