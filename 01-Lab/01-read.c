#include <stdio.h>

int main()
{
    // read file
    FILE *file = fopen("input.txt", "r");
    if (file == NULL)
    {
        printf("File not found!\n");
        return 1;
    }

    // print file
    printf("File content:\n");
    while (1)
    {
        char *line = NULL;
        size_t len = 0;
        if (getline(&line, &len, file) == -1)
        {
            break;
        }
        printf("%s", line);
    }
    fclose(file);
    return 0;
}