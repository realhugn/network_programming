#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

struct SinhVien {
    char mssv[9];
    char hoTen[64];
    char ngaySinh[11];
    float avg;
};

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

    // Khai bao cau truc dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));
    
    if (connect(client, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("Connecting failed\n");
        exit(1);
    }

    // Begin Send Msg
    struct SinhVien sv;
    char tmp[4];
    while (1)
    {
        printf("Nhap mssv: ");
        scanf("%s", sv.mssv);

        printf("Nhap ho va ten: ");
        while (getchar() != '\n');
        fgets(sv.hoTen, sizeof(sv.hoTen), stdin);
        if (sv.hoTen[strlen(sv.hoTen) - 1] == '\n')
            sv.hoTen[strlen(sv.hoTen) -1] = 0;

        printf("Nhap ngay sinh: ");
        scanf("%s", sv.ngaySinh);

        printf("Nhap diem trung binh: ");
        scanf("%f", &sv.avg);
        
        if (send(client, &sv, sizeof(sv), 0) == -1)
        {
            printf("Send Error\n");
            break;
        }
        
        printf("Tiep tuc nhap :(Y/N) ");
        scanf("%s", tmp);
        if (strncmp(tmp, "N", 2) == 0) break;
    }
    // End Sen msg

    close(client);
    printf("Socket closed\n");
    return 0;
}