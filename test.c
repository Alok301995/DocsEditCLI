#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// read functionality

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
    return count;
}

int read_content_from_file(FILE *file)
{
}

int main()
{
    char in[100];
    scanf("%[^\n]", in);
    FILE *file = fopen("test_file.txt", "r");
    char command[10];
    char msg[100];
    int count = sscanf(in, "%[^' '] %[^\n]", command, msg);
}