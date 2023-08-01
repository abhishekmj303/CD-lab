#include <stdio.h>
#include <string.h>

int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

int is_vowel(char c)
{
    int lower = c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
    int upper = c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
    return lower || upper;
}

int is_consonant(char c)
{
    if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') )
    {
        return !is_vowel(c);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <input_file>\n", argv[0]);
    }

    int lc = 0, wc = 0, cc = 0;
    int vowels = 0, consonants = 0, digits = 0, others = 0;
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    char line[200];
    while (fgets(line, 200, fp) != NULL)
    {
        lc++;

        // tokenize line: split into words based on space, tab, newline
        char *token = strtok(line, " \t\n");
        while (token != NULL)
        {
            wc++;

            // next token
            token = strtok(NULL, " \t\n");
        }
    }

    // reset file pointer to beginning of file
    fseek(fp, 0, SEEK_SET);

    char c;
    c = fgetc(fp);
    while (c != EOF)
    {
        cc++;

        if (is_digit(c))
        {
            digits++;
        }
        else if (is_vowel(c))
        {
            vowels++;
        }
        else if (is_consonant(c))
        {
            consonants++;
        }
        else
        {
            others++;
        }

        c = fgetc(fp);
    }
    
    printf("Lines: %d\nWords: %d\nCharacters: %d\n", lc, wc, cc);
    printf("\n");

    printf("Vowels: %d\nConsonants: %d\nDigits: %d\nOthers: %d\n", vowels, consonants, digits, others);
    printf("Characters Sum: %d\n", vowels + consonants + digits + others);

    fclose(fp);
    return 0;
}