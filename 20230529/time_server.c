#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void signalHandler(int signal)
{
    int status;
    int pid = wait(&status);
    if (status == EXIT_SUCCESS)
        printf("Child process %d terminated\n", pid);
}

int main(int argc, char* argv[])
{
    // Check enough arguments
    // Argument contains ./run_file_name + port
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

    // For logging address of connection
    struct sockaddr_in clientAddr;
    int clientAddrLength = sizeof(clientAddr);

    // For logging time
    time_t t;
    t = time(NULL);
    struct tm tm = *localtime(&t);
    
    // Listening
    signal(SIGCHLD, signalHandler);
    while (1)
    {
        int client = accept(listener, (struct sockaddr*)&clientAddr, &clientAddrLength);
        printf("Client has connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        
        if (fork() == 0)
        {
            // Child process
            close(listener);
            char buff[256];
            char cmd[32], format[32], tmp[32];

            while (1)
            {
                int ret = recv(client, buff, sizeof(buff), 0);
                if (ret <= 0)
                    break;
                buff[ret] = 0;
                if (buff[ret - 1] == '\n')
                    buff[ret - 1] = 0;
                printf("Receive: %s\n", buff);

                ret = sscanf(buff, "%s%s%s", cmd, format, tmp);
                if (ret == 2)
                {
                    if (strcmp(cmd, "GET_TIME") == 0)
                    {
                        if (strcmp(format, "dd/mm/yyyy") == 0)
                        {
                            sprintf(buff, "%02d/%02d/%04d\n", tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900);
                            send(client, buff, strlen(buff), 0);
                            break;
                        }
                        else if (strcmp(format, "dd/mm/yy") == 0)
                        {
                            sprintf(buff, "%02d/%02d/%02d\n", tm.tm_mday, tm.tm_mon+1, (tm.tm_year+1900)%100);
                            send(client, buff, strlen(buff), 0);
                            break;
                        }
                        else if (strcmp(format, "mm/dd/yyyy") == 0)
                        {
                            sprintf(buff, "%02d/%02d/%04d\n", tm.tm_mon+1, tm.tm_mday, tm.tm_year+1900);
                            send(client, buff, strlen(buff), 0);
                            break;
                        }
                        else if (strcmp(format, "mm/dd/yy") == 0)
                        {
                            sprintf(buff, "%02d/%02d/%02d\n", tm.tm_mon+1, tm.tm_mday, (tm.tm_year+1900)%100);
                            send(client, buff, strlen(buff), 0);
                            break;
                        }
                    }
                }

                char *msg = "Wrong format\n";
                send(client, msg, strlen(msg), 0);
            }

            close(client);
            exit(EXIT_SUCCESS);
        }   
        else
        {
            close(client);
        }
    }

    // Close
    close(listener);
    printf("Socket closed\n");
    printf("Main process terminated\n");
    killpg(0, SIGKILL);
    return 0;
}