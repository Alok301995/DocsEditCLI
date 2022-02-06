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
int invite_count = 0;
int reply = 0;

// *******Client record structure*****
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

// ************************************

// ******File Record Structure ********
typedef struct collab
{
    int collab_flag;
    int client_id;
    // if access =1 means user can view the file and if access =2 means edit access
    int access;
} Collab;

typedef struct File
{
    char file_name[30];
    int origin_client_id;
    Collab *c[4];
} File;

typedef struct File_record
{
    File *file;
    struct File_record *next;
} File_record;

// *************************************

// **********Invite Record**************
typedef struct invite
{
    int sender_sock;
    int rec_sock;
    int sender_id;
    int rec_id;
    // 0:pending ,1:accepted ,2:rejeted
    int status;
    // 1:V 2:E
    int access;
    int flag;
    char file_name[30];

} Invite;

typedef struct Invite_list
{
    Invite *invite;
    struct Invite_list *next;

} Invite_list;
// ***************************************

// *********Global variables**************
char file_name[100];

File_record *head = NULL;
Invite_list *invite_head = NULL;
int new_invite = 0;

// ***************************************

// ***********Utility Functions***********
int genereate_id()
{
    int base = 10000;
    int generated_id = base + id_count;
    id_count++;
    return generated_id;
}

int NLINEX(FILE *file)
{
    int count = 0;
    char buffer[100];
    while (!feof(file) && fgets(buffer, 1000, file))
    {
        if (strlen(buffer) > 0)
        {
            count++;
        }
    }
    rewind(file);
    fclose(file);
    return count;
}

int get_client_id(int *fd, Client *client_info)
{
    for (int i = 0; i < 5; i++)
    {
        if (client_info->fd[i]->flag == 1 && client_info->fd[i]->sockfd == *fd)
        {
            return client_info->fd[i]->client_id;
        }
    }
}
int get_fd(int c_id, Client *client_info)
{
    for (int i = 0; i < client_info->MAX_CON; i++)
    {
        if (client_info->fd[i]->client_id == c_id)
        {
            return client_info->fd[i]->sockfd;
        }
    }
    return -1;
}
// **************************************

// ********Connected Client**************
void get_connected_client(char *buffer, Client *client_info, int *fd)
{
    if (client_info->connected_client == 0)
    {
        sprintf(buffer, "%s", "No connected client");
    }
    else
    {
        sprintf(buffer, "%s", "Active Users: ");
        for (int i = 0; i < client_info->MAX_CON; i++)
        {
            char msg[10];
            bzero(msg, 10);
            if (client_info->fd[i]->flag == 1)
            {
                if (client_info->fd[i]->sockfd == *fd)
                {

                    sprintf(msg, "%d(M)", client_info->fd[i]->client_id);
                }
                else
                {
                    sprintf(msg, "%d", client_info->fd[i]->client_id);
                }
                strcat(buffer, msg);
                bzero(msg, 10);
                sprintf(msg, "%s", " | ");
                strcat(buffer, msg);
            }
        }
    }
}
// *************************************

// *********Invite Record Functions**********
Invite_list *invite_list_init(int sender_sock, int rec_id, int access, char *file_name, Client *client_info)
{
    Invite_list *node = (Invite_list *)malloc(sizeof(Invite_list));
    node->next = NULL;
    node->invite = (Invite *)malloc(sizeof(Invite));
    node->invite->sender_sock = sender_sock;
    node->invite->rec_id = rec_id;
    node->invite->access = access;
    node->invite->rec_sock = get_fd(rec_id, client_info);
    node->invite->status = 0;
    node->invite->flag = 1;
    node->invite->sender_id = get_client_id(&sender_sock, client_info);
    sprintf(node->invite->file_name, "%s", file_name);
    return node;
}

void add_invite_node(int sender_sock, int rec_id, int access, char *file_name, Client *client_info)
{
    Invite_list *node = invite_list_init(sender_sock, rec_id, access, file_name, client_info);
    if (invite_head == NULL)
    {
        invite_head = node;
    }
    else
    {
        node->next = invite_head;
        invite_head = node;
    }
}

int check_invite(int *fd)
{
    Invite_list *temp = invite_head;
    while (temp != NULL)
    {
        if (temp->invite->sender_sock == *fd)
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

int valid_invite(int client_id)
{
    Invite_list *temp = invite_head;
    while (temp != NULL)
    {
        if (temp->invite->rec_id == client_id)
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

int check_valid_invite(int *fd)
{
    Invite_list *temp = invite_head;
    while (temp != NULL)
    {
        if (temp->invite->rec_sock == *fd)
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

void delete_invite(int *fd)
{
    Invite_list *temp = invite_head;
    Invite_list *prev = NULL;
    if (temp != NULL && temp->invite->rec_sock == *fd)
    {
        invite_head = temp->next;
        free(temp->invite);
        free(temp);
        return;
    }
    while (temp != NULL && temp->invite->rec_sock != *fd)
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL)
    {

        return;
    }
    prev->next = temp->next;
    free(temp->invite);
    free(temp);
}

void assign_invite_perm(int *fd, int status, int flag, Client *client_info)
{
    Invite_list *temp = invite_head;
    File_record *file_temp = head;
    int empty_index = -1;
    int found = -1;
    int b = 0;
    while (temp != NULL)
    {
        if (temp->invite->rec_sock == *fd)
        {
            if (flag)
            {
                while (file_temp != NULL)
                {
                    if (strcmp(file_temp->file->file_name, temp->invite->file_name) == 0)
                    {
                        int id = get_client_id(fd, client_info);
                        for (int i = 0; i < 4; i++)
                        {
                            if (file_temp->file->c[i]->collab_flag == 0)
                            {
                                empty_index = i;
                                break;
                            }
                        }
                        for (int i = 0; i < 4; i++)
                        {
                            if (file_temp->file->c[i]->client_id == id)
                            {
                                found = i;
                                break;
                            }
                        }
                        if (found != -1)
                        {
                            file_temp->file->c[found]->access = temp->invite->access;
                            b = 1;
                        }
                        else if (empty_index != -1)
                        {
                            file_temp->file->c[empty_index]->access = temp->invite->access;
                            file_temp->file->c[empty_index]->collab_flag = 1;
                            file_temp->file->c[empty_index]->client_id = temp->invite->rec_id;
                            b = 1;
                        }
                    }

                    if (b == 1)
                    {
                        break;
                    }
                    else
                    {
                        file_temp = file_temp->next;
                    }
                }
            }

            temp->invite->status = status;
            return;
        }
        temp = temp->next;
    }
}

// ************************************************
// ************Server Functions********************

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
// ****************************************************

// **********File Record Functions*********************

File_record *file_node_init(char *file_name, int *fd, Client *client_info)
{
    File_record *node = (File_record *)malloc(sizeof(File_record));
    node->next = NULL;
    node->file = (File *)malloc(sizeof(File));
    for (int i = 0; i < 4; i++)
    {
        node->file->c[i] = (Collab *)malloc(sizeof(Collab));
        node->file->c[i]->access = 0;
        node->file->c[i]->client_id = 0;
        node->file->c[i]->collab_flag = 0;
    }
    snprintf(node->file->file_name, 100, "%s", file_name);
    node->file->origin_client_id = get_client_id(fd, client_info);
    return node;
}

void add_file_node(char *file_name, int *fd, Client *client_info)
{
    File_record *node = file_node_init(file_name, fd, client_info);
    if (head == NULL)
    {
        head = node;
    }
    else
    {
        node->next = head;
        head = node;
    }
}

void get_file_record(char *buffer)
{
    char msg[100];
    File_record *temp = head;
    if (temp == NULL)
    {
        sprintf(buffer, "%s", "No Record Found");
    }
    else
    {

        while (temp != NULL)
        {
            bzero(msg, 100);
            FILE *file = fopen(temp->file->file_name, "r");
            int line_count = NLINEX(file);
            sprintf(msg, "%s %d %d |", temp->file->file_name, temp->file->origin_client_id, line_count);
            strcat(buffer, msg);
            for (int i = 0; i < 4; i++)
            {
                bzero(msg, 100);
                if (temp->file->c[i]->client_id != 0)
                {
                    sprintf(msg, "%d (%d)|", temp->file->c[i]->client_id, temp->file->c[i]->access);
                    strcat(buffer, msg);
                }
            }
            buffer[strlen(buffer)] = '\n';
            temp = temp->next;
        }
    }
}

// *****************************************************
// ********Invite command check function****************

int check_file(char *file_name)
{
    if (access(file_name, F_OK) == 0)
    {
        return 1;
    }
    else
        return 0;
}
// find whether client is connected or not for invitation
int find_client_id(int *fd, Client *client_info, int id)
{
    for (int i = 0; i < 5; i++)
    {
        if (client_info->fd[i]->client_id == id)
        {
            if (client_info->fd[i]->sockfd != *fd)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    return 0;
}

int check_file_name_owner(char *file_name, int *fd, Client *client_info)
{
    File_record *temp = head;
    int id = get_client_id(fd, client_info);
    while (temp != NULL)
    {
        if (strcmp(temp->file->file_name, file_name) == 0)
        {
            if (temp->file->origin_client_id == id)
            {
                return 1;
            }
        }
        temp = temp->next;
    }
    return 0;
}

// ********************************************

// ******************Parser********************
void parser(char *buffer, Client *client_info, int *fd)
{
    char command[10];
    char msg[100];
    int count = sscanf(buffer, "%[^' '\n] %[^\n]", command, msg);
    bzero(buffer, 1024);
    if (strcmp(command, "/user") == 0 && count == 1)
    {
        get_connected_client(buffer, client_info, fd);
    }
    else if (strcmp(command, "/files") == 0)
    {
        get_file_record(buffer);
    }
    else if (strcmp(command, "/exit") == 0 && count == 1)
    {

        sprintf(buffer, "%s", "exit");
    }
    else if (strcmp(command, "/upload") == 0)
    {
        // check weather file already exist or not and also the upload.. same uploader can upload file but different uploaded can not upload same file
        if (access(msg, F_OK) < 0)
        {
            snprintf(file_name, 100, "%s", msg);
            add_file_node(file_name, fd, client_info);
            sprintf(buffer, "%s", "upload");
        }
        else
        {
            sprintf(buffer, "%s", "File Already Exits");
        }
        // sinal to start upload
    }
    else if (strcmp(command, "/invite") == 0)
    {
        char file_name[30];
        char client_id[30];
        char permission[30];
        int access;
        int msg_count = sscanf(msg, "%[^' '] %[^' '] %[^\n]", file_name, client_id, permission);
        int id = atoi(client_id);
        if (strcmp(permission, "V") == 0)
        {
            access = 1;
        }
        if (strcmp(permission, "E") == 0)
        {
            access = 2;
        }
        if (check_file(file_name) && check_file_name_owner(file_name, fd, client_info) && find_client_id(fd, client_info, id))
        {
            // send invite from here
            bzero(buffer, 1024);
            if (check_invite(fd))
            {
                sprintf(buffer, "%s", "Invite Already sent Wait for reply");
            }
            else
            {
                add_invite_node(*fd, id, access, file_name, client_info);
                invite_count = 1;
                sprintf(buffer, "%s", "Invite sent");
            }
        }
        else
        {
            sprintf(buffer, "%s", "Invite Rejected");
        }
    }
    else if (strcmp(command, "YES") == 0)
    {
        int id = get_client_id(fd, client_info);
        if (valid_invite(id))
        {
            assign_invite_perm(fd, 1, 1, client_info);
            reply = 1;
            bzero(buffer, 1024);
            sprintf(buffer, "%s", "You are Collaborator");
        }
        else
        {
            bzero(buffer, 1024);
            sprintf(buffer, "%s", "Invalid Command");
        }
    }
    else if (strcmp(command, "NO") == 0)
    {
        int id = get_client_id(fd, client_info);
        if (valid_invite(id))
        {
            assign_invite_perm(fd, -1, 0, client_info);
            reply = 1;
            bzero(buffer, 1024);
            sprintf(buffer, "%s", "You reject the invite");
        }
        else
        {
            bzero(buffer, 1024);
            sprintf(buffer, "%s", "Invalid Command");
        }
    }
    else
    {
        sprintf(buffer, "%s", "Invalid Command");
    }
}
// ********************************************************

// ******************Driver code***************************

int main(int argc, char *argv[])
{
    int x = 1;
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
        // *******************Invite*********************************
        if (invite_count == 1)
        {
            Invite_list *temp = invite_head;
            while (temp != NULL)
            {
                if (temp->invite->flag == 1)
                {
                    bzero(buffer, 1024);
                    sprintf(buffer, "%s %d", "Invitation from ", temp->invite->sender_id);
                    write(temp->invite->rec_sock, buffer, sizeof(buffer));
                    temp->invite->flag = 0;
                }
                temp = temp->next;
            }
            invite_count = 0;
        }
        // ***********************Invite Reply********************
        if (reply = 1)
        {
            Invite_list *temp = invite_head;
            Invite_list *prev = NULL;
            while (temp != NULL)
            {
                if (temp->invite->status != 0)
                {
                    Invite_list *x = NULL;
                    if (prev == NULL)
                    {
                        invite_head = temp->next;
                        x = temp;
                        temp = invite_head;
                    }
                    else
                    {
                        x = temp;
                        prev->next = temp->next;
                        temp = temp->next;
                    }
                    x->next = NULL;
                    bzero(buffer, 1024);
                    if (x->invite->status == -1)
                    {
                        sprintf(buffer, "%s", "Invite rejected");
                    }
                    else if (x->invite->status == 1)
                    {
                        sprintf(buffer, "%s", "Invite Accepted");
                    }
                    write(x->invite->sender_sock, buffer, sizeof(buffer));
                    free(x->invite);
                    free(x);
                }
                else
                {
                    prev = temp;
                    temp = temp->next;
                }
            }
            reply = 0;
        }

        // ******************************************************************
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

                    parser(buffer, client_info, &client_info->fd[i]->sockfd);

                    // check for client exit
                    if (strcmp(buffer, "exit") == 0)
                    {
                        write(client_info->fd[i]->sockfd, buffer, sizeof(buffer));
                        int check = disconnect_client(&client_info->fd[i]->sockfd, client_info);
                        bzero(buffer, 1024);
                    }
                    else if (strcmp(buffer, "upload") == 0)
                    {
                        write(client_info->fd[i]->sockfd, buffer, sizeof(buffer));
                        bzero(buffer, 1024);
                        // file download started
                        FILE *file = fopen(file_name, "w");
                        read(client_info->fd[i]->sockfd, buffer, sizeof(buffer));
                        int read_count = atoi(buffer);
                        bzero(buffer, 1024);
                        int chunk = 256;
                        int pos = 0;
                        while (read_count > chunk)
                        {
                            int rec_byte = read(client_info->fd[i]->sockfd, buffer + pos, chunk);
                            read_count -= rec_byte;
                            pos += rec_byte;
                        }
                        if (read_count > 0)
                        {
                            read(client_info->fd[i]->sockfd, buffer + pos, read_count);
                        }

                        fwrite(buffer, sizeof(char), strlen(buffer), file);
                        fclose(file);

                        bzero(buffer, 1024);
                        sprintf(buffer, "%s", "File uploaded");
                        write(client_info->fd[i]->sockfd, buffer, sizeof(buffer));
                        bzero(buffer, 1024);
                    }
                    // for general case
                    else
                    {
                        write(client_info->fd[i]->sockfd, buffer, sizeof(buffer));
                        bzero(buffer, 1024);
                    }
                }
            }
        }
    }
}
