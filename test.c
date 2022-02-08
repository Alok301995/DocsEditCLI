#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// read functionality

typedef struct Node
{
    /* data */
    int data;
    struct Node *next;
} Node;

Node *head = NULL;

void add_node(int data)
{
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = data;
    temp->next = NULL;
    if (head == NULL)
    {
        head = temp;
    }
    else
    {
        temp->next = head;
        head = temp;
    }
}
void print_data()
{

    Node *temp = head;
    while (temp != NULL)
    {
        printf("%d | ", temp->data);
        temp = temp->next;
    }
}

void find_and_delete(int data)
{
    Node *temp = head;
    Node *prev = NULL;
    while (temp != NULL)
    {
        if (temp->data == data)
        {
            Node *x = NULL;
            // delete node is at start
            if (prev == NULL)
            {
                x = temp;
                head = temp->next;
                temp = head;
            }
            else
            {
                x = temp;
                prev->next = temp->next;
                temp = temp->next;
            }
            x->next = NULL;
            free(x);
        }
        else
        {
            prev = temp;
            temp = temp->next;
        }
    }
}

int main()
{
    add_node(2);
    add_node(3);
    add_node(4);
    add_node(5);
    add_node(4);
    print_data();
    find_and_delete(4);
    printf("\n");
    print_data();
}