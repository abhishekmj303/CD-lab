#include <stdio.h>

int main()
{
    // open file
    FILE *infile = fopen("input.txt", "r");
    if (infile == NULL)
    {
        printf("File not found!\n");
        return 1;
    }
    FILE *outfile = fopen("output.txt", "w");

    // write file
    printf("Writing input.txt to output.txt\n");
    char line[100];
    while (fgets(line, 100, infile))
    {
        fputs(line, outfile);
    }

    // close files
    fclose(infile);
    fclose(outfile);
    return 0;
}