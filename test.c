#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Node
{
    int data;
    struct Node *next;
} Node;

Node *head = NULL;

void add_node(int data)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
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

void print_Node()
{
    Node *temp = head;
    while (temp != NULL)
    {
        printf("%d -->", temp->data);
        temp = temp->next;
    }
}

void delete_print(int data)
{
    Node *temp = head;
    Node *prev = NULL;
    while (temp != NULL)
    {
        if (temp->data == data)
        {
            // if temp is the head node;
            Node *x = NULL;
            if (prev == NULL)
            {
                head = temp->next;
                x = temp;
                temp = head;
            }
            else
            {
                x = temp;
                prev->next = temp->next;
                temp = temp->next;
            }
            x->next = NULL;
            printf("%d ", x->data);
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

    add_node(1);
    add_node(2);
    add_node(3);
    add_node(1);
    add_node(3);
    delete_print(5);
}