#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

// *************Global***********

int id_count = 1;

typedef struct client_fd
{
    int flag;
    int sockfd;
    int client_id;
} Client_fd;

typedef struct client
{
    int MAX_CON;
    int connected_client;
    Client_fd *fd[5];
} Client;

// file permision record structure

// ******************************

// Function to generate 5 digit unique id
int genereate_id()
{
    int base = 10000;
    int generated_id = base + id_count;
    id_count++;
    return generated_id;
}

// Function to get number of connected client
void get_connected_client(char *buffer, Client *client_info)
{
    if (client_info->connected_client == 0)
    {
        sprintf(buffer, "%s", "No connected client");
    }
    else
    {
        for (int i = 0; i < client_info->MAX_CON; i++)
        {
            char msg[10];
            if (client_info->fd[i]->flag == 1)
            {
                sprintf(msg, "%d", client_info->fd[i]->client_id);
                strcat(buffer, msg);
                sprintf(msg, "%s", " || ");
                strcat(buffer, msg);
            }
        }
    }
}
void parser(char *buffer, Client *client_info)
{
    char command[10];
    char msg[100];
    int count = sscanf(buffer, "%[^' '\n] %[^\n]", command, msg);
    bzero(buffer, 1024);
    if (strcmp(command, "/user") == 0 && count == 1)
    {
        get_connected_client(buffer, client_info);
    }
    else
    {
        sprintf(buffer, "%s", "Invalid Command");
    }
}

// ***************************
// ******Server Functions*****

int build_fdset(int *socket, fd_set *reafd, Client *client_info)
{
    int max_fd = 0;
    FD_ZERO(reafd);
    FD_SET(*socket, reafd);
    if (*socket > max_fd)
    {
        max_fd = *socket;
    }
    for (int i = 0; i < client_info->MAX_CON; i++)
    {
        if (client_info->fd[i]->flag == 1)
        {
            FD_SET(client_info->fd[i]->sockfd, reafd);
            if (client_info->fd[i]->sockfd > max_fd)
            {
                max_fd = client_info->fd[i]->sockfd;
            }
        }
    }
    return max_fd;
}
int register_new_client(int *sockfd, fd_set *readfd, Client *client_info)
{

    if (client_info->connected_client >= client_info->MAX_CON)
    {
        return 0;
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            // search for free space
            if (client_info->fd[i]->flag == 0)
            {
                int id = genereate_id();
                client_info->fd[i]->flag = 1;
                client_info->fd[i]->sockfd = *sockfd;
                client_info->fd[i]->client_id = id;
                client_info->connected_client += 1;
                return 1;
            }
        }
    }
}

int disconnect_client(int *sockfd, Client *client_info)
{
    for (int i = 0; i < 5; i++)
    {
        if (client_info->fd[i]->sockfd == *sockfd)
        {
            client_info->fd[i]->flag = 0;
            client_info->connected_client -= 1;
            close(client_info->fd[i]->sockfd);
            return 1;
        }
    }
    return 0;
}

// ****************************

// ********Driver code********

int main(int argc, char *argv[])
{
    char buffer[1024];
    int PORT = atoi(argv[1]);
    Client *client_info = (Client *)malloc(sizeof(Client));
    client_info->MAX_CON = 5;
    client_info->connected_client = 0;
    for (int i = 0; i < client_info->MAX_CON; i++)
    {
        client_info->fd[i] = (Client_fd *)malloc(sizeof(Client_fd));
        client_info->fd[i]->sockfd = -1;
        client_info->fd[i]->client_id = 0;
        client_info->fd[i]->flag = 0;
    }

    // server initilization
    struct sockaddr_in addr, cli_addr;
    socklen_t addrlen;
    int master_sock;
    fd_set readfd;
    int max_fd;
    master_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (master_sock < 0)
    {
        printf("Error creating socket");
        exit(EXIT_FAILURE);
    }
    else
        printf("Socket creation successful\n");

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(master_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Bind failed");
        exit(EXIT_FAILURE);
    }
    else
    {

        printf("Bind Success\n");
    }

    if (listen(master_sock, 3) < 0)
    {
        printf("Error listening to socket");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Listening\n");
    }
    addrlen = sizeof(cli_addr);

    while (1)
    {
        int max_fd = build_fdset(&master_sock, &readfd, client_info);
        int action = select(max_fd + 1, &readfd, NULL, NULL, NULL);

        if (FD_ISSET(master_sock, &readfd))
        {
            int connfd = accept(master_sock, (struct sockaddr *)&cli_addr, &addrlen);
            // check for successfull connection
            if (connfd < 0)
            {
                printf("error in connection");
                exit(EXIT_FAILURE);
            }
            int success = register_new_client(&connfd, &readfd, client_info);
            if (success)
            {
                // send msg to client that connection successfull
                // wirte call to client about successful connection
                bzero(buffer, 1024);
                sprintf(buffer, "%s", "Welcome to Online File Editor");
                write(connfd, buffer, sizeof(buffer));
                bzero(buffer, 1024);
            }
            else
            {
                // inform client that connection is refused by the client.
                // write call to client about connection refusal.
                bzero(buffer, 1024);
                sprintf(buffer, "%s", "R");
                write(connfd, buffer, sizeof(buffer));
                bzero(buffer, 1024);
            }
        }

        for (int i = 0; i < client_info->MAX_CON; i++)
        {
            if (FD_ISSET(client_info->fd[i]->sockfd, &readfd))
            {
                int read_count = read(client_info->fd[i]->sockfd, buffer, sizeof(buffer));
                if (read_count < 0)
                {
                    printf("Error reading from server\n");
                    continue;
                }
                else if (read_count == 0)
                {
                    // client disconnected;
                    int success = disconnect_client(&client_info->fd[i]->sockfd, client_info);
                    if (success)
                    {
                        // send a msg to the client about the disconnection
                        printf("[%d:]Client Disconnected\n", client_info->fd[i]->client_id);
                        continue;
                    }
                    else
                    {
                        printf("Error disconnecting client\n");
                        continue;
                    }
                }
                else
                {
                    printf("[%d]:%s", client_info->fd[i]->client_id, buffer);
                    parser(buffer, client_info);

                    // All the processing are done in this function and server return the result;

                    write(client_info->fd[i]->sockfd, buffer, sizeof(buffer));
                    bzero(buffer, 1024);
                }
            }
        }
    }
}
