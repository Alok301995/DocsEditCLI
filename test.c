#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

int validate_read_args(FILE *file, int s_idx, int e_idx, int s_flag, int e_flag)
{
    int total_lines = NLINEX(file);
    int start;
    int end;
    if (s_flag == 1)
    {
        start = s_idx >= 0 ? s_idx : total_lines + s_idx;
    }
    if (e_flag == 1)
    {
        end = e_idx >= 0 ? e_idx : total_lines + e_idx;
    }
    if (start < 0 || end < 0)
    {
        return 0;
    }

    else if (s_flag == 1 && e_flag == 1)
    {
        if (start < total_lines && end < total_lines && start <= end)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if ((s_flag == 1 && e_flag == 0) || (s_flag == 0 && e_flag == 1))
    {
        if (s_flag == 1 && e_flag == 0)
        {
            if (start < total_lines)
            {
                return 1;
            }
            else
                return 0;
        }
        else
        {
            if (end < total_lines)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }

    else
    {
        return 1;
    }
}

int main()
{

    FILE *file = fopen("test_file.txt", "r");
    int count = NLINEX(file);
    int validate = validate_read_args(file, -1, 0, 1, 1);
    printf("%d\n", validate);
    char s_idx[10];
    bzero(s_idx, 10);
    sprintf(s_idx, "%s", "-6");
    printf("%d\n", atoi(s_idx));
    fclose(file);
}