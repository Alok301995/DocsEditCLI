#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#define BZERO bzero(buffer, 1024);
#define INT_MIN -2147483648;
#define INT_MAX 2147483647;

// *******Global**********
char download_file_name[100];

// ***********************
char file_name[30];

void print_and_exit(char *msg)
{
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}
int byte_count(FILE *file)
{
    fseek(file, 0, SEEK_END);
    int count = ftell(file);
    rewind(file);
    return count;
}
int _atoi(char *str)
{
    int sign = 1, base = 0, i = 0, count = 0;
    while (str[i] == ' ')
    {
        i++;
    }
    if (str[i] == '-')
    {
        sign = 1 - 2 * (str[i++] == '-');
        count++;
    }
    while (str[i] >= '0' && str[i] <= '9')
    {
        if (base > __INT32_MAX__ / 10 || (base == __INT32_MAX__ / 10 && str[i] - '0' > 7))
        {
            if (sign == 1)
                return __INT32_MAX__;
            else
                return INT_MIN;
        }
        base = 10 * base + (str[i++] - '0');
        count++;
    }
    if (count == strlen(str))
    {
        return base * sign;
    }
    else
    {
        return __INT32_MAX__;
    }
}

// ***********************
// ****Client Functions***
// Parser
// parser will return 1 when command is valid and if the command is invalid it will return 0.

int parser(char *buffer)
{
    char command[10];
    char msg[100];
    bzero(command, 10);
    bzero(msg, 100);
    int count = sscanf(buffer, "%[^' '\n] %[^\n]", command, msg);
    if (strcmp(command, "/users") == 0 && count == 1)
    {
        if (strlen(msg) == 0)
        {
            return 1;
        }
        else
        {
            printf("Invalid Command\n");
            return 0;
        }
    }
    else if (strcmp(command, "/files") == 0 && count == 1)
    {
        if (strlen(msg) == 0)
        {
            return 1;
        }
        else
        {
            printf("Invalid Command\n");

            return 0;
        }
    }
    else if (strcmp(command, "/upload") == 0 && count == 2)
    {
        if (strlen(msg) > 0)
        {
            if (access(msg, F_OK) < 0)
            {
                printf("File Does not exit\n");
                return 0;
            }
            else
            {
                snprintf(file_name, 100, "%s", msg);
                return 1;
            }
        }
        else
        {
            printf("Invalid upload command\n");
            return 0;
        }
    }
    else if (strcmp(command, "/download") == 0)
    {
        if (strlen(msg) == 0)
        {
            printf("Invalid Download command\n");
            return 0;
        }
        else
        {
            bzero(download_file_name, 100);
            sprintf(download_file_name, "%s", msg);
            return 1;
        }
    }
    else if (strcmp(command, "/exit") == 0 && count == 1)
    {
        if (strlen(msg) == 0)
        {
            return 1;
        }
        else
        {
            printf("Invalid Command\n");
            return 0;
        }
    }
    else if (strcmp(command, "/invite") == 0 && count == 2)
    {
        if (strlen(msg) == 0)
        {
            printf("Invalid Command\n");
            return 0;
        }
        else
        {
            // break the command into 3 parts and check the length of each part
            char file_name[30];
            char client_id[30];
            char permission[30];
            int msg_count = sscanf(msg, "%[^' '] %[^' '] %[^\n]", file_name, client_id, permission);
            if (msg_count == 3 && strlen(file_name) > 0 && strlen(client_id) == 5 && (strcmp(permission, "V") == 0 || strcmp(permission, "E") == 0))
            {
                return 1;
            }
            else
            {
                printf("Invalid Command\n");
                return 0;
            }
        }
    }
    else if (strcmp(command, "YES") == 0)
    {
        if (strlen(msg) == 0)
        {
            return 1;
        }
        else
        {
            printf("Invalid Command\n");
            return 0;
        }
    }
    else if (strcmp(command, "NO") == 0)
    {
        if (strlen(msg) == 0)
        {
            return 1;
        }
        else
        {
            printf("Invalid Command\n");
            return 0;
        }
    }
    else if (strcmp(command, "/read") == 0)
    {
        char file_name[30];
        char s_idx[10];
        char e_idx[10];
        int msg_count = sscanf(msg, "%[^' '] %[^' '] %[^\n]", file_name, s_idx, e_idx);
        if (msg_count >= 1 && msg_count <= 3)
        {
            // 1. check whether the s_idx and e_idx are valid number or not
            // check file_name length
            if (msg_count == 1)
            {
                if (strlen(file_name) == 0)
                {

                    printf("Invalid Command\n");
                    return 0;
                }
                else
                {

                    return 1;
                }
            }
            else if (msg_count == 2)
            {
                if (strlen(file_name) == 0)
                {
                    printf("Invalid Command\n");
                    return 0;
                }
                else if (_atoi(s_idx) == __INT32_MAX__)
                {
                    printf("Invalid Command\n");
                    return 0;
                }
                else
                    return 1;
            }
            else if (msg_count == 3)
            {
                if (strlen(file_name) == 0 || strlen(s_idx) == 0 || strlen(e_idx) == 0)
                {
                    printf("Invalid Command\n");
                    return 0;
                }
                else if (_atoi(s_idx) == __INT32_MAX__ || _atoi(e_idx) == __INT32_MAX__)
                {
                    printf("Invalid Command\n");
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
        }
        else
        {
            printf("Invalid Command\n");
            return 0;
        }
    }
    else if (strcmp(command, "/delete") == 0)
    {
        char file_name[30];
        char s_idx[10];
        char e_idx[10];
        int msg_count = sscanf(msg, "%[^' '] %[^' '] %[^\n]", file_name, s_idx, e_idx);
        if (msg_count >= 1 && msg_count <= 3)
        {
            // 1. check whether the s_idx and e_idx are valid number or not
            // check file_name length

            if (msg_count == 1)
            {
                if (strlen(file_name) == 0)
                {

                    printf("Invalid Command\n");
                    return 0;
                }
                else
                {

                    return 1;
                }
            }
            else if (msg_count == 2)
            {
                if (strlen(file_name) == 0)
                {
                    printf("Invalid Command\n");
                    return 0;
                }
                else if (_atoi(s_idx) == __INT32_MAX__)
                {
                    printf("Invalid Command\n");
                    return 0;
                }
                else
                    return 1;
            }
            else if (msg_count == 3)
            {
                if (strlen(file_name) == 0 || strlen(s_idx) == 0 || strlen(e_idx) == 0)
                {
                    printf("Invalid Command\n");
                    return 0;
                }
                else if (_atoi(s_idx) == __INT32_MAX__ || _atoi(e_idx) == __INT32_MAX__)
                {
                    printf("Invalid Command\n");
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
        }
        else
        {
            printf("Invalid Command\n");
            return 0;
        }
    }
    else if (strcmp(command, "/insert") == 0)
    {
        char file_name[30];
        char idx[1000];
        char c_msg[1000];
        char garbage[1000];
        bzero(file_name, 30);
        bzero(idx, 1000);
        bzero(c_msg, 1000);
        bzero(garbage, 1000);
        int msg_count = sscanf(msg, "%[^' '] %[^' '] %[^\n]", file_name, idx, c_msg);
        if (strlen(file_name) == 0)
        {

            printf("Invalid Command.Please check parameters\n");
            return 0;
        }

        else if (msg_count == 1)
        {
            printf("Invalid Command.Please check parameters\n");
            return 0;
        }

        else if (msg_count == 2)
        {
            char client_msg[2000];
            bzero(client_msg, 2000);
            sscanf(idx, "\"%[^\"]\" %[^\n]", client_msg, garbage);
            // printf("%d\n", msg_count);
            // printf("%ld\n", strlen(client_msg));
            // printf("%ld\n", strlen(garbage));

            if ((strlen(client_msg) == 0 && strlen(garbage) == 0) || strlen(garbage) != 0)
            {
                printf("Invalid Command.Please check parameters\n");
                return 0;
            }
            else
                return 1;
        }
        else if (msg_count == 3)
        {
            char client_msg[2000];
            bzero(client_msg, 2000);
            sscanf(c_msg, "\"%[^\"]\" %[^\n]", client_msg, garbage);
            // printf("%d\n", msg_count);
            // printf("%ld\n", strlen(client_msg));
            // printf("%ld\n", strlen(garbage));
            if (strlen(garbage) != 0)
            {
                printf("Invalid Command.Please check parameters\n");

                return 0;
            }
            else
            {
                return 1;
            }
        }

        return 1;
    }

    else
    {
        printf("Invalid Command.Please check parameters\n");
        return 0;
    }
}

// ********Driver*********

int main(int argc, char *argv[])
{
    int sockfd;
    fd_set readfd, writefd;
    int PORT = atoi(argv[1]);
    struct sockaddr_in servaddr, cliaddr;
    int max_fd;
    char buffer[1024];
    bzero(buffer, 1024);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...");
        exit(0);
    }

    while (1)
    {

        max_fd = sockfd;
        FD_ZERO(&readfd);
        FD_SET(sockfd, &writefd);
        FD_SET(sockfd, &readfd);
        FD_SET(STDIN_FILENO, &readfd);
        int action = select(max_fd + 1, &readfd, &writefd, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &readfd))
        {
            // through this socket we are sending msg from client to server
            BZERO;
            fgets(buffer, 1024, stdin);
            int check = parser(buffer);
            if (check == 1)
            {
                // if command is valid then only client send the command to the server else it will read from stdin
                write(sockfd, buffer, sizeof(buffer));
                BZERO;
            }
        }
        if (FD_ISSET(sockfd, &readfd))
        {
            // This socket will handle all the read operations
            // if server send a "R" than it means that server has reached the maximum number of client or client exit by using /exit

            read(sockfd, buffer, sizeof(buffer));
            if (strcmp(buffer, "R") == 0)
            {
                printf("Server Rejected the connection\n");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            else if (strcmp(buffer, "exit") == 0)
            {
                printf("All Records Deleted, Thank you for using Online File Editor\n");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            else if (strcmp(buffer, "upload") == 0)
            {
                BZERO;
                FILE *file = fopen(file_name, "r");
                int bytes = byte_count(file);
                sprintf(buffer, "%d", bytes);
                write(sockfd, buffer, sizeof(buffer));
                BZERO;
                int count = fread(buffer, sizeof(char), bytes, file);
                int pos = 0;
                int chunk = 256;
                while (count > chunk)
                {
                    int send_bytes = write(sockfd, buffer + pos, chunk);
                    count -= send_bytes;
                    pos += send_bytes;
                }
                if (count > 0)
                {
                    write(sockfd, buffer + pos, count);
                }

                // upload complete.
            }
            else if (strcmp(buffer, "download") == 0)
            {
                FILE *file = fopen(download_file_name, "w+");
                read(sockfd, buffer, sizeof(buffer));
                int read_count = atoi(buffer);
                bzero(buffer, 1024);
                int chunk = 256;
                int pos = 0;
                while (read_count > chunk)
                {
                    int rec_byte = read(sockfd, buffer + pos, chunk);
                    read_count -= rec_byte;
                    pos += rec_byte;
                }
                if (read_count > 0)
                {
                    read(sockfd, buffer + pos, read_count);
                }
                fwrite(buffer, sizeof(char), strlen(buffer), file);
                fclose(file);
            }

            else if (strcmp(buffer, "read") == 0)
            {
                // send a signal to the server
                bzero(buffer, 1024);
                read(sockfd, buffer, sizeof(buffer));
                int read_count = atoi(buffer);
                bzero(buffer, 1024);
                int chunk = 256;
                int pos = 0;
                while (read_count > chunk)
                {
                    int rec_byte = read(sockfd, buffer + pos, chunk);
                    read_count -= rec_byte;
                    pos += rec_byte;
                }
                if (read_count > 0)
                {
                    read(sockfd, buffer + pos, read_count);
                }
                puts(buffer);
            }
            else if (strcmp(buffer, "delete") == 0)
            {
                bzero(buffer, 1024);
                read(sockfd, buffer, sizeof(buffer));
                int read_count = atoi(buffer);
                bzero(buffer, 1024);
                int chunk = 256;
                int pos = 0;
                while (read_count > chunk)
                {
                    int rec_byte = read(sockfd, buffer + pos, chunk);
                    read_count -= rec_byte;
                    pos += rec_byte;
                }
                if (read_count > 0)
                {
                    read(sockfd, buffer + pos, read_count);
                }
                puts(buffer);
                bzero(buffer, 1024);
            }
            else
            {
                printf("%s\n", buffer);
                bzero(download_file_name, 100);
                BZERO;
            }
        }
    }
    close(sockfd);
}
