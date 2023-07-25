#include <stdio.h>

int main()
{
    // open file
    FILE *file = fopen("output.txt", "a");
    if (file == NULL)
    {
        printf("File not found!\n");
        return 1;
    }

    // append file
    printf("Appending to output.txt\n");
    fputs("Abhishek M J - CS21B2018\n", file);
    fputs("Compiler Design Lab - 01\n", file);

    // close file
    fclose(file);
    return 0;
}