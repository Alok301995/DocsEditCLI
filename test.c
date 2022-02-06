#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
    Collab *c[5];
} File;

typedef struct File_record
{
    File *file;
    struct File_record *next;
} File_record;

char file_name[100];

File_record *head = NULL;

File_record *file_node_init(char *file_name)
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
    node->file->origin_client_id = 0;
    return node;
}

void add_file_node(char *file_name)
{
    File_record *node = file_node_init(file_name);
    if (head == NULL)
    {
        head = node;
    }
    else
    {
        // add node at the starting of the head
        node->next = head;
        head = node;
    }
}
void print_record()
{

    File_record *temp = head;
    if (head == NULL)
    {
        printf("Record empty\n");
    }
    else
    {

        while (temp != NULL)
        {
            printf("%s %d\n", temp->file->file_name, temp->file->origin_client_id);
            temp = temp->next;
        }
    }
}

int main()
{
    add_file_node("hello.txt");
    add_file_node("alok.txt");
    add_file_node("hellow.txt");
    print_record();
    add_file_node("newfile.txt");
    printf("\n");
    print_record();
}